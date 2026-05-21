#ifndef IQ_ANALYZER_H
#define IQ_ANALYZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ===== 类型定义 ===== */

typedef struct {
    double mean;
    double stddev;
    double median;
    int min;
    int max;
    int hist[256];
    double over_pct;
    double under_pct;
    int dyn_range;
    const char *status;
} LumReport;

typedef struct {
    double u_mean;
    double v_mean;
    const char *cast;
} ColorReport;

typedef struct {
    double y_noise_var;
    double uv_noise_var;
    const char *y_level;
    const char *uv_level;
} NoiseReport;

typedef struct {
    double edge_energy;
    const char *level;
} SharpReport;

/* ===== IO 函数 ===== */

/* 读二进制帧文件 */
unsigned char* read_frame(const char *filename, int size);

/* 从 YUYV 提取 Y 平面，返回 malloc 的缓冲区 */
unsigned char* extract_y_yuyv(unsigned char *yuyv, int w, int h);

/* 从 Y 平面构建直方图 hist[256] */
void build_histogram(unsigned char *y_plane, int w, int h, int hist[256]);

/* ===== 四大分析模块 ===== */

LumReport    analyze_luminance(unsigned char *y_plane, int w, int h);
ColorReport  analyze_color(unsigned char *yuyv, int w, int h);
NoiseReport  analyze_noise(unsigned char *y_plane, int w, int h);
SharpReport  analyze_sharpness(unsigned char *y_plane, int w, int h);

/* ===== 报告输出 ===== */

double compute_score(LumReport l, ColorReport c, NoiseReport n, SharpReport s);

void print_report(LumReport l, ColorReport c, NoiseReport n, SharpReport s,
                  const char *filename, int w, int h, const char *fmt);

void export_json(LumReport l, ColorReport c, NoiseReport n, SharpReport s,
                 const char *filename, int w, int h, const char *fmt,
                 const char *outfile);

#endif
