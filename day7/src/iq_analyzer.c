#include "iq_analyzer.h"

/*
 * iq_analyzer — 图像质量分析器
 * 用法: ./iq_analyzer <file> <width> <height> [--json out.json]
 *
 * 输入：YUYV 原始帧
 * 输出：终端报告 + 可选 JSON
 */
int main(int argc, char *argv[])
{
    const char *filename;
    int w, h, size, json_mode;
    const char *fmt;
    const char *json_out;
    unsigned char *data;
    unsigned char *y_plane;
    LumReport lum;
    ColorReport col;
    NoiseReport noise;
    SharpReport sharp;

    /* 1. 参数解析 */
    json_mode = 0;
    json_out = NULL;

    if (argc < 4) {
        printf("Usage: %s <file> <width> <height> [--json out.json]\n", argv[0]);
        return 1;
    }

    filename = argv[1];
    w = atoi(argv[2]);
    h = atoi(argv[3]);
    fmt = "YUYV (4:2:2)";
    size = w * h * 2;

    if (argc >= 6 && strcmp(argv[4], "--json") == 0) {
        json_mode = 1;
        json_out = argv[5];
    }

    /* 2. 读文件 */
    data = read_frame(filename, size);
    if (!data) {
        printf("Error: cannot read %s\n", filename);
        return 1;
    }

    /* 3. 提取 Y 平面 */
    y_plane = extract_y_yuyv(data, w, h);
    if (!y_plane) {
        printf("Error: cannot extract Y plane\n");
        free(data);
        return 1;
    }

    /* 4. 执行分析（等你填完各模块就能跑） */
    lum   = analyze_luminance(y_plane, w, h);
    col   = analyze_color(data, w, h);
    noise = analyze_noise(y_plane, w, h);
    sharp = analyze_sharpness(y_plane, w, h);

    /* 5. 输出报告 */
    print_report(lum, col, noise, sharp, filename, w, h, fmt);

    if (json_mode && json_out) {
        export_json(lum, col, noise, sharp, filename, w, h, fmt, json_out);
        printf("JSON report saved to %s\n", json_out);
    }

    /* 6. 清理 */
    free(data);
    free(y_plane);
    return 0;
}
