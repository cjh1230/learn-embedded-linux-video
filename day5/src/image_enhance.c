#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_y_plane(unsigned char *yuyv, unsigned char *y, int w, int h)
{
    int i;
    for (i = 0; i < w * h; i++)
        y[i] = yuyv[i * 2];
}

void build_histogram(unsigned char *y, int pixels, int hist[256])
{ 
    int i;
    memset(hist, 0, 256 * sizeof(int));
    for (i = 0; i < pixels; i++)
        hist[y[i]]++;
}

void hist_equalize(unsigned char *y, int pixels)
{
    int hist[256], cdf[256];
    build_histogram(y, pixels, hist);

    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++)
        cdf[i] = cdf[i - 1] + hist[i];

    int cdf_min = cdf[0];
    for (int i = 0; cdf[i] == 0; i++)
        cdf_min = cdf[i + 1];

    for (int i = 0; i < pixels; i++)
        y[i] = (cdf[y[i]] - cdf_min) * 255 / (pixels - cdf_min);
}

void gamma_correct(unsigned char *y, int pixels, double gamma)
{
    unsigned char lut[256];
    for (int i = 0; i < 256; i++)
        lut[i] = (unsigned char)(255.0 * pow(i / 255.0, 1.0 / gamma));
    for (int i = 0; i < pixels; i++)
        y[i] = lut[y[i]];
}

void blur_3x3(unsigned char *y_in, unsigned char *y_out, int w, int h)
{
    int sum;
    for (int row = 1; row < h - 1; row++) {
        for (int col = 1; col < w - 1; col++) {
            int idx = row * w + col;
            sum = y_in[idx - w - 1] + y_in[idx - w] + y_in[idx - w + 1]
                + y_in[idx - 1]     + y_in[idx]     + y_in[idx + 1]
                + y_in[idx + w - 1] + y_in[idx + w] + y_in[idx + w + 1];
            y_out[row * w + col] = sum / 9;
        }
    }
}

void write_y_back(unsigned char *y_plane, unsigned char *yuyv, int w, int h)
{
    int i;
    for (i = 0; i < w * h; i++)
        yuyv[i * 2] = y_plane[i];
}

/* 统计工具：算 mean/std/min/max，打印 Before/After */
void print_stats(unsigned char *y, int pixels, const char *label)
{
    double sum = 0.0, d, mean, std;
    int min = 255, max = 0, i;
    for (i = 0; i < pixels; i++) {
        sum += y[i];
        if (y[i] < min) min = y[i];
        if (y[i] > max) max = y[i];
    }
    mean = sum / pixels;
    sum = 0.0;
    for (i = 0; i < pixels; i++) {
        d = y[i] - mean;
        sum += d * d;
    }
    std = sqrt(sum / pixels);
    printf("%s: Mean=%.1f Std=%.1f Min=%d Max=%d\n", label, mean, std, min, max);
}

int main(int argc, char *argv[])
{
    const char *in_file;
    int w, h, size_in, pixels;
    FILE *fp;
    unsigned char *yuyv, *yuyv_orig, *y_plane, *y_temp;

    in_file = (argc >= 2) ? argv[1] : "frame.raw";
    w = (argc >= 3) ? atoi(argv[2]) : 640;
    h = (argc >= 4) ? atoi(argv[3]) : 480;
    size_in = w * h * 2;
    pixels  = w * h;

    yuyv      = malloc(size_in);
    yuyv_orig = malloc(size_in);
    y_plane   = malloc(pixels);
    y_temp    = malloc(pixels);
    if (!yuyv || !yuyv_orig || !y_plane || !y_temp) {
        fprintf(stderr, "malloc\n"); return -1;
    }

    fp = fopen(in_file, "rb");
    if (!fp) { perror("fopen"); return -1; }
    fread(yuyv, 1, size_in, fp);
    fclose(fp);
    memcpy(yuyv_orig, yuyv, size_in);  /* 保存原始帧 */

    /* === 1. 直方图均衡化 === */
    extract_y_plane(yuyv, y_plane, w, h);
    print_stats(y_plane, pixels, "Before EQ ");
    hist_equalize(y_plane, pixels);
    print_stats(y_plane, pixels, "After  EQ ");
    write_y_back(y_plane, yuyv, w, h);
    fp = fopen("enhanced_eq.yuyv", "wb");
    fwrite(yuyv, 1, size_in, fp); fclose(fp);

    /* === 2. Gamma 校正（从原始帧重新开始） === */
    memcpy(yuyv, yuyv_orig, size_in);
    extract_y_plane(yuyv, y_plane, w, h);
    print_stats(y_plane, pixels, "Before Gm ");
    gamma_correct(y_plane, pixels, 2.2);
    print_stats(y_plane, pixels, "After  Gm ");
    write_y_back(y_plane, yuyv, w, h);
    fp = fopen("enhanced_gamma.yuyv", "wb");
    fwrite(yuyv, 1, size_in, fp); fclose(fp);

    /* === 3. 均值滤波（从原始帧重新开始） === */
    memcpy(yuyv, yuyv_orig, size_in);
    extract_y_plane(yuyv, y_plane, w, h);
    print_stats(y_plane, pixels, "Before Bl ");
    blur_3x3(y_plane, y_temp, w, h);
    print_stats(y_temp, pixels, "After  Bl ");
    write_y_back(y_temp, yuyv, w, h);
    fp = fopen("enhanced_blur.yuyv", "wb");
    fwrite(yuyv, 1, size_in, fp); fclose(fp);

    printf("\nDone: enhanced_eq.yuyv / enhanced_gamma.yuyv / enhanced_blur.yuyv\n");
    free(yuyv); free(yuyv_orig); free(y_plane); free(y_temp);
    return 0;}
