#include "iq_analyzer.h"

static double clamp(double v, double lo, double hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* 综合评分 0-10
   亮度 30%（mean 在 100-140 满分，偏离扣分）
   曝光 20%（over+under < 5% 满分）
   颜色 20%（neutral 满分，有 cast 扣分）
   噪点 15%（clean 满分）
   清晰度 15%（normal 满分）
*/
double compute_score(LumReport l, ColorReport c, NoiseReport n, SharpReport s)
{
    double score, lum_s, exp_s, col_s, noise_s, sharp_s;

    /* TODO: 计算各项得分，最后 score = (各项之和) * 10，clamp 到 0-10 */

    score = 0.0;
    return score;
}

/* 终端格式化输出（参考下面格式） */
void print_report(LumReport l, ColorReport c, NoiseReport n, SharpReport s,
                  const char *filename, int w, int h, const char *fmt)
{
    double score;
    int file_size;

    file_size = w * h * 2;
    score = compute_score(l, c, n, s);

    /*
    ╔══════════════════════════════════════╗
    ║     Image Quality Analysis Report    ║
    ╠══════════════════════════════════════╣
    ║ File:       frame.yuyv              ║
    ║ Resolution: 640 × 480               ║
    ║ Format:     YUYV (4:2:2)            ║
    ║ File Size:  614,400 bytes           ║
    ╠══════════════════════════════════════╣
    ║                                      ║
    ║  LUMINANCE                           ║
    ║  ├─ Mean:     118.3  (normal)       ║
    ║  ├─ StdDev:    45.7                 ║
    ║  ├─ Median:    112                  ║
    ║  ├─ Min/Max:   12 / 241             ║
    ║  └─ Histogram: well-distributed     ║
    ║                                      ║
    ║  EXPOSURE                            ║
    ║  ├─ Over-exposed:  0.3%             ║
    ║  ├─ Under-exposed: 1.2%             ║
    ║  ├─ Dynamic Range: 162 (P1-P99)     ║
    ║  └─ Status: Normal                  ║
    ║                                      ║
    ║  COLOR                               ║
    ║  ├─ U Mean:  127.8                  ║
    ║  ├─ V Mean:  128.5                  ║
    ║  └─ Cast:    Neutral                ║
    ║                                      ║
    ║  NOISE                               ║
    ║  ├─ Y  Noise Var:  4.8   (clean)    ║
    ║  └─ UV Noise Var:  6.2   (clean)    ║
    ║                                      ║
    ║  SHARPNESS                           ║
    ║  ├─ Edge Energy:   38.2             ║
    ║  └─ Score:         Normal sharpness ║
    ║                                      ║
    ╠══════════════════════════════════════╣
    ║  OVERALL SCORE: 8.2 / 10            ║
    ║  Verdict: Good image quality        ║
    ╚══════════════════════════════════════╝
    */

    printf("TODO: print report\n");
    printf("Score: %.1f / 10\n", score);
}

/* JSON 导出 */
void export_json(LumReport l, ColorReport c, NoiseReport n, SharpReport s,
                 const char *filename, int w, int h, const char *fmt,
                 const char *outfile)
{
    /* TODO: fopen(outfile,"w"), fprintf JSON 各字段, fclose */
    printf("TODO: export JSON to %s\n", outfile);
}
