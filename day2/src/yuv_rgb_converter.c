#include <stdio.h>
#include <stdlib.h>

#define CLAMP(x) ((x) < 0 ? 0 : (x) > 255 ? 255 : (x))

/*
 * yuyv_to_rgb - YUYV (4:2:2) → RGB24
 * yuyv: 输入, w*h*2 字节, 每4字节=2像素[Y0][U][Y1][V]
 * rgb:  输出, w*h*3 字节, [R0][G0][B0][R1][G1][B1]...
 */
void yuyv_to_rgb(unsigned char *yuyv, unsigned char *rgb, int w, int h)
{
    int i, j;
    int y0, y1, u, v;
    int r, g, b;
    int yuyv_idx, rgb_idx;
    int C, D, E;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j += 2) {
            /* 每 4 字节 = 2 个像素 */
            yuyv_idx = (i * w + j) * 2;

            y0 = yuyv[yuyv_idx + 0];   /* Y0 */
            u  = yuyv[yuyv_idx + 1];   /* U  (两个像素共用) */
            y1 = yuyv[yuyv_idx + 2];   /* Y1 */
            v  = yuyv[yuyv_idx + 3];   /* V  (两个像素共用) */

            /* 像素 0 */
            C = y0;
            D = u - 128;
            E = v - 128;
            r = (298 * C + 409 * E + 128) >> 8;
            g = (298 * C - 100 * D - 208 * E + 128) >> 8;
            b = (298 * C + 516 * D + 128) >> 8;

            rgb_idx = (i * w + j) * 3;
            rgb[rgb_idx + 0] = CLAMP(r);
            rgb[rgb_idx + 1] = CLAMP(g);
            rgb[rgb_idx + 2] = CLAMP(b);

            /* 像素 1 (用同一组 U/V) */
            C = y1;
            /* D 和 E 不变 */
            r = (298 * C + 409 * E + 128) >> 8;
            g = (298 * C - 100 * D - 208 * E + 128) >> 8;
            b = (298 * C + 516 * D + 128) >> 8;

            rgb_idx = (i * w + j + 1) * 3;
            rgb[rgb_idx + 0] = CLAMP(r);
            rgb[rgb_idx + 1] = CLAMP(g);
            rgb[rgb_idx + 2] = CLAMP(b);
        }
    }
}

void rgb_to_yuyv(unsigned char *rgb, unsigned char *yuyv, int w, int h)
{
    int i, j;
    int r0, g0, b0, r1, g1, b1;
    int y0, y1, u0, u1, v0, v1, u, v;
    int yuyv_idx, rgb_idx;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j += 2) {
            rgb_idx = (i * w + j) * 3;
            r0 = rgb[rgb_idx + 0]; g0 = rgb[rgb_idx + 1]; b0 = rgb[rgb_idx + 2];
            r1 = rgb[rgb_idx + 3]; g1 = rgb[rgb_idx + 4]; b1 = rgb[rgb_idx + 5];

            y0 = ( 77 * r0 + 150 * g0 +  29 * b0) >> 8;
            y1 = ( 77 * r1 + 150 * g1 +  29 * b1) >> 8;

            u0 = ((-43 * r0 -  85 * g0 + 128 * b0) >> 8) + 128;
            u1 = ((-43 * r1 -  85 * g1 + 128 * b1) >> 8) + 128;
            v0 = ((128 * r0 - 107 * g0 -  21 * b0) >> 8) + 128;
            v1 = ((128 * r1 - 107 * g1 -  21 * b1) >> 8) + 128;
            u = CLAMP((u0 + u1) >> 1);
            v = CLAMP((v0 + v1) >> 1);

            yuyv_idx = (i * w + j) * 2;
            yuyv[yuyv_idx + 0] = CLAMP(y0);
            yuyv[yuyv_idx + 1] = u;
            yuyv[yuyv_idx + 2] = CLAMP(y1);
            yuyv[yuyv_idx + 3] = v;
        }
    }
}

void nv12_to_rgb(unsigned char *nv12, unsigned char *rgb, int w, int h)
{
    int i, j;
    int y0, y1, y2, y3, u, v;
    int nv12_idx, rgb_idx, uv_off;
    int r, g, b;
    int C, D, E;

    for (i = 0; i < h; i += 2) {
        for (j = 0; j < w; j += 2) {
            nv12_idx = i * w + j;
            y0 = nv12[nv12_idx + 0];
            y1 = nv12[nv12_idx + 1];
            y2 = nv12[nv12_idx + w];
            y3 = nv12[nv12_idx + w + 1];

            uv_off = (i/2) * w + (j/2) * 2;
            u = nv12[w * h + uv_off];
            v = nv12[w * h + uv_off + 1];

            D = u - 128; E = v - 128;

            /* pixel (i, j) */
            C = y0;
            r = (298*C + 409*E + 128) >> 8;
            g = (298*C - 100*D - 208*E + 128) >> 8;
            b = (298*C + 516*D + 128) >> 8;
            rgb_idx = (i*w + j) * 3;
            rgb[rgb_idx+0]=CLAMP(r); rgb[rgb_idx+1]=CLAMP(g); rgb[rgb_idx+2]=CLAMP(b);

            /* pixel (i, j+1) */
            C = y1;
            r = (298*C + 409*E + 128) >> 8;
            g = (298*C - 100*D - 208*E + 128) >> 8;
            b = (298*C + 516*D + 128) >> 8;
            rgb_idx = (i*w + j + 1) * 3;
            rgb[rgb_idx+0]=CLAMP(r); rgb[rgb_idx+1]=CLAMP(g); rgb[rgb_idx+2]=CLAMP(b);

            /* pixel (i+1, j) */
            C = y2;
            r = (298*C + 409*E + 128) >> 8;
            g = (298*C - 100*D - 208*E + 128) >> 8;
            b = (298*C + 516*D + 128) >> 8;
            rgb_idx = ((i+1)*w + j) * 3;
            rgb[rgb_idx+0]=CLAMP(r); rgb[rgb_idx+1]=CLAMP(g); rgb[rgb_idx+2]=CLAMP(b);

            /* pixel (i+1, j+1) */
            C = y3;
            r = (298*C + 409*E + 128) >> 8;
            g = (298*C - 100*D - 208*E + 128) >> 8;
            b = (298*C + 516*D + 128) >> 8;
            rgb_idx = ((i+1)*w + j + 1) * 3;
            rgb[rgb_idx+0]=CLAMP(r); rgb[rgb_idx+1]=CLAMP(g); rgb[rgb_idx+2]=CLAMP(b);
        }
    }
}

void rgb_to_nv12(unsigned char *rgb, unsigned char *nv12, int w, int h)
{
    int i, j;
    int r0, g0, b0, r1, g1, b1, r2, g2, b2, r3, g3, b3;
    int y0, y1, y2, y3, u0, u1, u2, u3, v0, v1, v2, v3, u, v;
    int nv12_idx, rgb_idx, uv_off;

    for (i = 0; i < h; i += 2) {
        for (j = 0; j < w; j += 2) {
            rgb_idx = (i*w + j) * 3;
            r0 = rgb[rgb_idx+0]; g0 = rgb[rgb_idx+1]; b0 = rgb[rgb_idx+2];
            r1 = rgb[rgb_idx+3]; g1 = rgb[rgb_idx+4]; b1 = rgb[rgb_idx+5];

            rgb_idx = ((i+1)*w + j) * 3;
            r2 = rgb[rgb_idx+0]; g2 = rgb[rgb_idx+1]; b2 = rgb[rgb_idx+2];
            r3 = rgb[rgb_idx+3]; g3 = rgb[rgb_idx+4]; b3 = rgb[rgb_idx+5];

            y0 = ( 77*r0 + 150*g0 +  29*b0) >> 8;
            y1 = ( 77*r1 + 150*g1 +  29*b1) >> 8;
            y2 = ( 77*r2 + 150*g2 +  29*b2) >> 8;
            y3 = ( 77*r3 + 150*g3 +  29*b3) >> 8;

            u0 = ((-43*r0 -  85*g0 + 128*b0) >> 8) + 128;
            u1 = ((-43*r1 -  85*g1 + 128*b1) >> 8) + 128;
            u2 = ((-43*r2 -  85*g2 + 128*b2) >> 8) + 128;
            u3 = ((-43*r3 -  85*g3 + 128*b3) >> 8) + 128;
            v0 = ((128*r0 - 107*g0 -  21*b0) >> 8) + 128;
            v1 = ((128*r1 - 107*g1 -  21*b1) >> 8) + 128;
            v2 = ((128*r2 - 107*g2 -  21*b2) >> 8) + 128;
            v3 = ((128*r3 - 107*g3 -  21*b3) >> 8) + 128;

            u = CLAMP((u0+u1+u2+u3) >> 2);
            v = CLAMP((v0+v1+v2+v3) >> 2);

            nv12_idx = i*w + j;
            nv12[nv12_idx + 0] = CLAMP(y0);
            nv12[nv12_idx + 1] = CLAMP(y1);
            nv12[nv12_idx + w + 0] = CLAMP(y2);
            nv12[nv12_idx + w + 1] = CLAMP(y3);

            uv_off = (i/2)*w + (j/2)*2;
            nv12[w*h + uv_off + 0] = u;
            nv12[w*h + uv_off + 1] = v;
        }
    }
}

int main(int argc, char *argv[])
{
    const char *in_file;
    int w, h;
    FILE *fp;
    unsigned char *yuyv, *rgb, *nv12, *rgb2;
    int size_in, size_out, nv12_size;

    in_file = (argc >= 2) ? argv[1] : "frame.raw";
    w = (argc >= 3) ? atoi(argv[2]) : 640;
    h = (argc >= 4) ? atoi(argv[3]) : 480;

    size_in  = w * h * 2;
    size_out = w * h * 3;
    nv12_size = w * h * 3 / 2;

    yuyv = malloc(size_in);
    rgb  = malloc(size_out);
    nv12 = malloc(nv12_size);
    rgb2 = malloc(size_out);
    if (yuyv==NULL || rgb==NULL || nv12==NULL || rgb2==NULL) {
        fprintf(stderr, "malloc failed\n");
        return -1;
    }

    /* 读 YUYV */
    fp = fopen(in_file, "rb");
    if (fp==NULL) { perror("fopen"); return -1; }
    fread(yuyv, 1, size_in, fp);
    fclose(fp);

    /* YUYV → RGB */
    yuyv_to_rgb(yuyv, rgb, w, h);
    fp = fopen("output.rgb", "wb");
    fwrite(rgb, 1, size_out, fp);
    fclose(fp);
    printf("YUYV -> RGB:   output.rgb (%d bytes)\n", size_out);

    /* 往返: RGB → YUYV */
    rgb_to_yuyv(rgb, yuyv, w, h);
    fp = fopen("roundtrip.yuyv", "wb");
    fwrite(yuyv, 1, size_in, fp);
    fclose(fp);
    printf("RGB -> YUYV:  roundtrip.yuyv (%d bytes)\n", size_in);

    /* NV12 往返: RGB → NV12 → RGB */
    rgb_to_nv12(rgb, nv12, w, h);
    nv12_to_rgb(nv12, rgb2, w, h);
    fp = fopen("nv12_rt.rgb", "wb");
    fwrite(rgb2, 1, size_out, fp);
    fclose(fp);
    printf("NV12 -> RGB:  nv12_rt.rgb (%d bytes)\n", size_out);

    printf("\nDone. View with:\n");
    printf("  ffplay -f rawvideo -pixel_format yuyv422 -video_size %dx%d roundtrip.yuyv\n", w, h);
    printf("  ffplay -f rawvideo -pixel_format rgb24   -video_size %dx%d nv12_rt.rgb\n", w, h);

    free(yuyv); free(rgb); free(nv12); free(rgb2);
    return 0;
}
