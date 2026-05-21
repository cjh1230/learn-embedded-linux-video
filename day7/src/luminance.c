#include "iq_analyzer.h"

/* 读二进制帧文件，返回 malloc 的缓冲区 */
unsigned char* read_frame(const char* filename, int size) {
	/* TODO: fopen rb, fread, 错误返回 NULL */
	fopen(filename, size);
	return NULL;
}

/* 从 YUYV 提取 Y 平面（Y0 Y1 交错取） */
unsigned char* extract_y_yuyv(unsigned char* yuyv, int w, int h) {
	/* TODO: malloc w*h, 遍历取 yuyv[(i*w+j)*2] */
	unsigned char* y = malloc(w * h);
	if (!y) {
		return NULL;
	}
	int i, j;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			y[i * w + j] = yuyv[(i * w + j) * 2];
		}
	}

	return y;
}

/* 构建 0-255 直方图 */
void build_histogram(unsigned char* y_plane, int w, int h, int hist[256]) {
	/* TODO: 先清零 hist，遍历 y_plane，hist[值]++ */
}

/* 亮度综合分析 */
LumReport analyze_luminance(unsigned char* y_plane, int w, int h) {
	LumReport r;
	int i;
	int hist[256];
	int total;
	double sum;
	int cdf;

	total = w * h;
	memset(&r, 0, sizeof(r));

	/* 1. 调用 build_histogram，结果写 r.hist */

	/* 2. min: 从 0 往上找第一个 hist[i] > 0 */

	/* 3. max: 从 255 往下找第一个 hist[i] > 0 */

	/* 4. mean = sum(i * hist[i]) / total */

	/* 5. stddev = sqrt(sum( (i-mean)^2 * hist[i] ) / total) */

	/* 6. median: 累加 hist 到 total/2 */

	/* 7. over_pct: hist[241..255] 之和 / total * 100 */

	/* 8. under_pct: hist[0..9] 之和 / total * 100 */

	/* 9. dyn_range: CDF 取 P1 和 P99，P99 - P1 */

	/* 10. status: mean<60→"too dark", mean>200→"too bright",
				stddev<20→"low contrast", 其余→"normal" */

	return r;
}
