#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLAMP(x) ((x) < 0 ? 0 : (x) > 255 ? 255 : (x))

void yuyv_to_rgb(unsigned char *yuyv, unsigned char *rgb, int w, int h)
{
    int i, j;
    int y0, y1, u, v;
    int C, D, E, r, g, b;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j += 2) {
            int off = (i * w + j) * 2;
            y0 = yuyv[off+0]; u = yuyv[off+1];
            y1 = yuyv[off+2]; v = yuyv[off+3];
            C = y0; D = u - 128; E = v - 128;
            r = (298*C + 409*E + 128) >> 8;
            g = (298*C - 100*D - 208*E + 128) >> 8;
            b = (298*C + 516*D + 128) >> 8;
            rgb[(i*w+j)*3+0]=CLAMP(r); rgb[(i*w+j)*3+1]=CLAMP(g); rgb[(i*w+j)*3+2]=CLAMP(b);
            C = y1;
            r = (298*C + 409*E + 128) >> 8;
            g = (298*C - 100*D - 208*E + 128) >> 8;
            b = (298*C + 516*D + 128) >> 8;
            rgb[(i*w+j+1)*3+0]=CLAMP(r); rgb[(i*w+j+1)*3+1]=CLAMP(g); rgb[(i*w+j+1)*3+2]=CLAMP(b);
        }
    }
}

void rgb_to_yuyv(unsigned char *rgb, unsigned char *yuyv, int w, int h)
{
    int i, j;
    int r0,g0,b0,r1,g1,b1;
    int y0,y1,u0,u1,v0,v1,u,v;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j += 2) {
            int off0 = (i*w+j)*3, off1 = (i*w+j+1)*3;
            r0=rgb[off0+0]; g0=rgb[off0+1]; b0=rgb[off0+2];
            r1=rgb[off1+0]; g1=rgb[off1+1]; b1=rgb[off1+2];
            y0 = ( 77*r0 + 150*g0 +  29*b0) >> 8;
            y1 = ( 77*r1 + 150*g1 +  29*b1) >> 8;
            u0 = ((-43*r0 -  85*g0 + 128*b0) >> 8) + 128;
            u1 = ((-43*r1 -  85*g1 + 128*b1) >> 8) + 128;
            v0 = ((128*r0 - 107*g0 -  21*b0) >> 8) + 128;
            v1 = ((128*r1 - 107*g1 -  21*b1) >> 8) + 128;
            u = CLAMP((u0+u1)>>1);
            v = CLAMP((v0+v1)>>1);
            int off = (i*w+j)*2;
            yuyv[off+0]=CLAMP(y0); yuyv[off+1]=u;
            yuyv[off+2]=CLAMP(y1); yuyv[off+3]=v;
        }
    }
}

void gray_world_wb(unsigned char *rgb, int pixels, double *out_gainR, double *out_gainB)
{
    int sumR = 0, sumG = 0, sumB = 0, i, k;
    double avgR, avgG, avgB;
    double gainR, gainB;

    for (i = 0; i < pixels * 3; i += 3) {
        sumR += rgb[i+0]; sumG += rgb[i+1]; sumB += rgb[i+2];
    }
    avgR = (double)sumR / pixels;
    avgG = (double)sumG / pixels;
    avgB = (double)sumB / pixels;
    printf("GW Before: avgR=%.1f avgG=%.1f avgB=%.1f\n", avgR, avgG, avgB);

    gainR = avgG / avgR;
    gainB = avgG / avgB;
    printf("GW Gains:  gainR=%.4f  gainB=%.4f\n", gainR, gainB);

    for (i = 0; i < pixels * 3; i += 3) {
        rgb[i+0] = CLAMP((int)(rgb[i+0] * gainR));
        rgb[i+2] = CLAMP((int)(rgb[i+2] * gainB));
    }

    sumR = sumG = sumB = 0;
    for (k = 0; k < pixels * 3; k += 3) {
        sumR += rgb[k+0]; sumG += rgb[k+1]; sumB += rgb[k+2];
    }
    printf("GW After:  avgR=%.1f avgG=%.1f avgB=%.1f\n",
           (double)sumR/pixels, (double)sumG/pixels, (double)sumB/pixels);

    *out_gainR = gainR;
    *out_gainB = gainB;
}

void white_patch_wb(unsigned char *rgb, int pixels, double *out_gainR, double *out_gainB)
{
    int maxR = 0, maxG = 0, maxB = 0, i, k;
    int sumR, sumG, sumB;
    double gainR, gainB;

    for (i = 0; i < pixels * 3; i += 3) {
        if (rgb[i+0] > maxR) maxR = rgb[i+0];
        if (rgb[i+1] > maxG) maxG = rgb[i+1];
        if (rgb[i+2] > maxB) maxB = rgb[i+2];
    }
    printf("WP Before: maxR=%d maxG=%d maxB=%d\n", maxR, maxG, maxB);

    gainR = 255.0 / maxR;
    gainB = 255.0 / maxB;
    printf("WP Gains:  gainR=%.4f  gainB=%.4f\n", gainR, gainB);

    for (i = 0; i < pixels * 3; i += 3) {
        rgb[i+0] = CLAMP((int)(rgb[i+0] * gainR));
        rgb[i+2] = CLAMP((int)(rgb[i+2] * gainB));
    }

    sumR = sumG = sumB = 0;
    for (k = 0; k < pixels * 3; k += 3) {
        sumR += rgb[k+0]; sumG += rgb[k+1]; sumB += rgb[k+2];
    }
    printf("WP After:  avgR=%.1f avgG=%.1f avgB=%.1f\n",
           (double)sumR/pixels, (double)sumG/pixels, (double)sumB/pixels);

    *out_gainR = gainR;
    *out_gainB = gainB;
}

int main(int argc, char *argv[])
{
    const char *in_file;
    int w, h, size_in, size_rgb;
    FILE *fp;
    unsigned char *yuyv, *rgb, *yuyv_orig;
    double gw_gainR, gw_gainB, wp_gainR, wp_gainB;
    double mix_gainR, mix_gainB;
    int k;

    in_file = (argc >= 2) ? argv[1] : "frame.raw";
    w = (argc >= 3) ? atoi(argv[2]) : 640;
    h = (argc >= 4) ? atoi(argv[3]) : 480;
    size_in  = w * h * 2;
    size_rgb = w * h * 3;

    yuyv = malloc(size_in);
    rgb  = malloc(size_rgb);
    yuyv_orig = malloc(size_in);
    if (!yuyv || !rgb || !yuyv_orig) { fprintf(stderr, "malloc\n"); return -1; }

    fp = fopen(in_file, "rb");
    if (!fp) { perror("fopen"); free(yuyv); free(rgb); free(yuyv_orig); return -1; }
    fread(yuyv_orig, 1, size_in, fp);
    fclose(fp);

    /* 1. Gray World */
    memcpy(yuyv, yuyv_orig, size_in);
    yuyv_to_rgb(yuyv, rgb, w, h);
    gray_world_wb(rgb, w*h, &gw_gainR, &gw_gainB);
    rgb_to_yuyv(rgb, yuyv, w, h);
    fp = fopen("corrected_gw.yuyv", "wb");
    fwrite(yuyv, 1, size_in, fp); fclose(fp);

    /* 2. White Patch */
    memcpy(yuyv, yuyv_orig, size_in);
    yuyv_to_rgb(yuyv, rgb, w, h);
    white_patch_wb(rgb, w*h, &wp_gainR, &wp_gainB);
    rgb_to_yuyv(rgb, yuyv, w, h);
    fp = fopen("corrected_wp.yuyv", "wb");
    fwrite(yuyv, 1, size_in, fp); fclose(fp);

    /* 3. Mixed */
    mix_gainR = (gw_gainR + wp_gainR) / 2.0;
    mix_gainB = (gw_gainB + wp_gainB) / 2.0;
    printf("Mixed WB:  gainR=%.4f  gainB=%.4f\n", mix_gainR, mix_gainB);
    memcpy(yuyv, yuyv_orig, size_in);
    yuyv_to_rgb(yuyv, rgb, w, h);
    for (k = 0; k < w*h*3; k += 3) {
        rgb[k+0] = CLAMP((int)(rgb[k+0] * mix_gainR));
        rgb[k+2] = CLAMP((int)(rgb[k+2] * mix_gainB));
    }
    rgb_to_yuyv(rgb, yuyv, w, h);
    fp = fopen("corrected_mix.yuyv", "wb");
    fwrite(yuyv, 1, size_in, fp); fclose(fp);

    printf("\nDone: corrected_gw.yuyv / corrected_wp.yuyv / corrected_mix.yuyv\n");
    free(yuyv_orig); free(yuyv); free(rgb);
    return 0;
}
