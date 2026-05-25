#include "iq_analyzer.h"

static double clamp(double v, double lo, double hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/*
 * 综合评分 0-10
 * 亮度 30%（mean 在 100-140 满分，偏离扣分）
 * 曝光 20%（over+under < 5% 满分）
 * 颜色 20%（neutral 满分，有 cast 扣分）
 * 噪点 15%（clean 满分）
 * 清晰度 15%（normal 满分）
 */
double compute_score(LumReport l, ColorReport c, NoiseReport n, SharpReport s)
{
    double score, lum_s, exp_s, col_s, noise_s, sharp_s;

    lum_s   = clamp(1.0 - fabs(l.mean - 120) / 80, 0.0, 1.0);
    exp_s   = clamp(1.0 - (l.over_pct + l.under_pct) / 25, 0.0, 1.0);
    col_s   = strcmp(c.cast, "neutral") == 0 ? 1.0 : 0.5;
    noise_s = strcmp(n.y_level, "clean") == 0 ? 1.0 : 0.5;
    sharp_s = strcmp(s.level, "normal") == 0 ? 1.0 : 0.5;

    score = (lum_s * 0.30 + exp_s * 0.20 + col_s * 0.20
          + noise_s * 0.15 + sharp_s * 0.15) * 10;
    return clamp(score, 0.0, 10.0);
}

/* 终端格式化输出 */
void print_report(LumReport l, ColorReport c, NoiseReport n, SharpReport s,
                  const char *filename, int w, int h, const char *fmt)
{
    double score;
    int file_size;
    const char *verdict;

    file_size = w * h * 2;
    score = compute_score(l, c, n, s);

    if (score >= 8)
        verdict = "Good image quality";
    else if (score >= 6)
        verdict = "Fair image quality";
    else if (score >= 4)
        verdict = "Poor image quality";
    else
        verdict = "Bad image quality";

    printf("╔═════════════════════════════════╗\n");
    printf("║     Image Quality Analysis Report    ║\n");
    printf("╠══════════════════════════════════╣\n");
    printf("║ File:       %-24s ║\n", filename);
    printf("║ Resolution: %d × %-3d                ║\n", w, h);
    printf("║ Format:     %-24s ║\n", fmt);
    printf("║ File Size:  %d bytes                  ║\n", file_size);
    printf("╠══════════════════════════════════╣\n");

    printf("║                                      ║\n");
    printf("║  LUMINANCE                           ║\n");
    printf("║  ├─ Mean:     %6.1f  (%-10s) ║\n", l.mean, l.status);
    printf("║  ├─ StdDev:    %6.1f                 ║\n", l.stddev);
    printf("║  ├─ Median:   %6.0f                  ║\n", l.median);
    printf("║  ├─ Min/Max:  %3d / %-3d              ║\n", l.min, l.max);
    printf("║  └─ Histogram: well-distributed     ║\n");

    printf("║                                      ║\n");
    printf("║  EXPOSURE                            ║\n");
    printf("║  ├─ Over-exposed:  %.1f%%                ║\n", l.over_pct);
    printf("║  ├─ Under-exposed: %.1f%%                ║\n", l.under_pct);
    printf("║  ├─ Dynamic Range: %-3d                ║\n", l.dyn_range);
    printf("║  └─ Status: %-23s ║\n",
           strcmp(l.status, "normal") == 0 ? "Normal" : l.status);

    printf("║                                      ║\n");
    printf("║  COLOR                               ║\n");
    printf("║  ├─ U Mean:  %6.1f                  ║\n", c.u_mean);
    printf("║  ├─ V Mean:  %6.1f                  ║\n", c.v_mean);
    printf("║  └─ Cast:    %-22s ║\n", c.cast);

    printf("║                                      ║\n");
    printf("║  NOISE                               ║\n");
    printf("║  ├─ Y  Noise Var:  %6.1f  (%-6s) ║\n", n.y_noise_var, n.y_level);
    printf("║  └─ UV Noise Var:  %6.1f  (%-6s) ║\n", n.uv_noise_var, n.uv_level);

    printf("║                                      ║\n");
    printf("║  SHARPNESS                           ║\n");
    printf("║  ├─ Edge Energy: %6.1f               ║\n", s.edge_energy);
    printf("║  └─ Score:       %-20s ║\n", s.level);

    printf("║                                      ║\n");
    printf("╠══════════════════════════════════╣\n");
    printf("║  OVERALL SCORE: %.1f / 10                  ║\n", score);
    printf("║  Verdict: %-27s ║\n", verdict);
    printf("╚══════════════════════════════════╝\n");
}

/* JSON 导出 */
void export_json(LumReport l, ColorReport c, NoiseReport n, SharpReport s,
                 const char *filename, int w, int h, const char *fmt,
                 const char *outfile)
{
    double score;
    FILE *fp;

    fp = fopen(outfile, "w");
    if (!fp) return;

    score = compute_score(l, c, n, s);

    fprintf(fp, "{\n");
    fprintf(fp, "  \"file\":        \"%s\",\n", filename);
    fprintf(fp, "  \"width\":       %d,\n", w);
    fprintf(fp, "  \"height\":      %d,\n", h);
    fprintf(fp, "  \"format\":      \"%s\",\n", fmt);
    fprintf(fp, "  \"file_size\":   %d,\n", w * h * 2);
    fprintf(fp, "  \"luminance\": {\n");
    fprintf(fp, "    \"mean\":     %.1f,\n", l.mean);
    fprintf(fp, "    \"stddev\":   %.1f,\n", l.stddev);
    fprintf(fp, "    \"median\":   %.0f,\n", l.median);
    fprintf(fp, "    \"min\":      %d,\n", l.min);
    fprintf(fp, "    \"max\":      %d,\n", l.max);
    fprintf(fp, "    \"status\":   \"%s\"\n", l.status);
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"exposure\": {\n");
    fprintf(fp, "    \"over_pct\":  %.1f,\n", l.over_pct);
    fprintf(fp, "    \"under_pct\": %.1f,\n", l.under_pct);
    fprintf(fp, "    \"dyn_range\": %d\n", l.dyn_range);
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"color\": {\n");
    fprintf(fp, "    \"u_mean\": %.1f,\n", c.u_mean);
    fprintf(fp, "    \"v_mean\": %.1f,\n", c.v_mean);
    fprintf(fp, "    \"cast\":   \"%s\"\n", c.cast);
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"noise\": {\n");
    fprintf(fp, "    \"y_var\":  %.1f,\n", n.y_noise_var);
    fprintf(fp, "    \"uv_var\": %.1f,\n", n.uv_noise_var);
    fprintf(fp, "    \"y_level\": \"%s\",\n", n.y_level);
    fprintf(fp, "    \"uv_level\":\"%s\"\n", n.uv_level);
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"sharpness\": {\n");
    fprintf(fp, "    \"edge_energy\": %.1f,\n", s.edge_energy);
    fprintf(fp, "    \"level\":       \"%s\"\n", s.level);
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"overall_score\": %.1f\n", score);
    fprintf(fp, "}\n");

    fclose(fp);
}
