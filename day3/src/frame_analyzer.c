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

double compute_mean(unsigned char *y, int pixels)
{
    double sum;
    int i;
    sum = 0;
    for (i = 0; i < pixels; i++)
        sum += y[i];
    return sum / pixels;
}

double compute_stddev(unsigned char *y, int pixels, double mean)
{
    double d, sum;
    int i;
    sum = 0.0;
    for (i = 0; i < pixels; i++) {
        d = y[i] - mean;
        sum += d * d;
    }
    return sqrt(sum / pixels);
}

int compute_median(int hist[256], int pixels)
{
    int cum, i;
    cum = 0;
    for (i = 0; i < 256; i++) {
        cum += hist[i];
        if (cum >= pixels / 2)
            return i;
    }
    return 0;
}

void find_min_max(unsigned char *y, int pixels, int *min, int *max)
{
    int i;
    *min = 255;
    *max = 0;
    for (i = 0; i < pixels; i++) {
        if (y[i] < *min) *min = y[i];
        if (y[i] > *max) *max = y[i];
    }
}

int main(int argc, char *argv[])
{
    const char *in_file;
    int w, h, size_in;
    FILE *fp;
    unsigned char *yuyv, *y_plane;
    int hist[256];
    double mean, stddev;
    int median, min, max;
    int over_count, under_count, i;
    double over_pct, under_pct;
    long long diff_sum;
    int diff;
    double noise_var;

    in_file = (argc >= 2) ? argv[1] : "frame.raw";
    w = (argc >= 3) ? atoi(argv[2]) : 640;
    h = (argc >= 4) ? atoi(argv[3]) : 480;
    size_in = w * h * 2;

    yuyv = malloc(size_in);
    y_plane = malloc(w * h);
    if (yuyv == NULL || y_plane == NULL) {
        fprintf(stderr, "malloc failed\n");
        return -1;
    }

    fp = fopen(in_file, "rb");
    if (fp == NULL) { perror("fopen"); free(yuyv); free(y_plane); return -1; }
    fread(yuyv, 1, size_in, fp);
    fclose(fp);

    /* 分析 */
    extract_y_plane(yuyv, y_plane, w, h);
    build_histogram(y_plane, w * h, hist);
    mean = compute_mean(y_plane, w * h);
    stddev = compute_stddev(y_plane, w * h, mean);
    median = compute_median(hist, w * h);
    find_min_max(y_plane, w * h, &min, &max);

    /* 曝光评估 */
    over_count = 0; under_count = 0;
    for (i = 0; i < w * h; i++) {
        if (y_plane[i] > 240) over_count++;
        if (y_plane[i] < 10)  under_count++;
    }
    over_pct  = 100.0 * over_count / (w * h);
    under_pct = 100.0 * under_count / (w * h);

    /* 噪点估计 */
    diff_sum = 0;
    for (i = 0; i < w * h - 1; i++) {
        diff = (int)y_plane[i] - (int)y_plane[i + 1];
        diff_sum += (long long)diff * diff;
    }
    noise_var = (double)diff_sum / (w * h - 1);

    /* 输出报告 */
    printf("=== Frame Analysis ===\n");
    printf("Resolution:     %dx%d\n", w, h);
    printf("Mean:           %.1f\n", mean);
    printf("StdDev:         %.1f\n", stddev);
    printf("Median:         %d\n", median);
    printf("Min/Max:        %d / %d\n", min, max);
    printf("Over-exposed:   %.2f%%\n", over_pct);
    printf("Under-exposed:  %.2f%%\n", under_pct);
    printf("Noise Var:      %.1f\n", noise_var);

    free(yuyv);
    free(y_plane);
    return 0;
}
