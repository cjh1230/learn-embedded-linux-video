#include "iq_analyzer.h"

/* 噪点分析：相邻像素差方差 */
NoiseReport analyze_noise(unsigned char *y_plane, int w, int h)
{
    NoiseReport r;
    double y_sum;
    int x, y_idx, y_cnt;
    int diff;
    double uv_sum;
    int uv_cnt;

    memset(&r, 0, sizeof(r));

    /* TODO: Y 噪点 — 水平方向相邻像素差平方的均值 */
    /* diff = y_plane[y*w+x] - y_plane[y*w+x+1]; y_sum += diff*diff; */

    /* TODO: UV 噪点 — 隔 2 行 2 列采样，同样算差平方的均值 */

    /* TODO: y_level 判断
       var<5  → "clean"
       var<15 → "slight"
       var<30 → "moderate"
       其余   → "heavy"
       uv_level 同样阈值
    */

    /* 临时填充，防止 uninitialized 警告 */
    r.y_noise_var = 0.0;
    r.uv_noise_var = 0.0;
    r.y_level = "TODO";
    r.uv_level = "TODO";

    return r;
}
