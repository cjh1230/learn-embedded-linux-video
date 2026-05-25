#include "iq_analyzer.h"

/* 读二进制帧文件，返回 malloc 的缓冲区 */
unsigned char* read_frame(const char* filename, int size) {
	/* TODO: fopen rb, fread, 错误返回 NULL */
	FILE* fp;
	unsigned char* buf;
	fp = fopen(filename, "rb");
	if (!fp) {
		return NULL;
	}

	buf = malloc(size);
	if (!buf) {
		fclose(fp);
		return NULL;
	}

	fread(buf, 1, size, fp);
	fclose(fp);
	return buf;
}

/* 从 YUYV 提取 Y 平面（Y0 Y1 交错取） */
unsigned char* extract_y_yuyv(unsigned char* yuyv, int w, int h) {
	/* TODO: malloc w*h, 遍历取 yuyv[(i*w+j)*2] */
	unsigned char* y;
	int i, j;
	y = malloc(w * h);
	if (!y) {
		return NULL;
	}

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
	int i;
	memset(hist, 0, 256 * sizeof(int));
	for (i = 0; i < w * h; i++) {
		hist[y_plane[i]]++;
	}
}

/* 亮度综合分析 */
LumReport analyze_luminance(unsigned char* y_plane, int w, int h) {
	LumReport r;
	int i;
	int hist[256];
	int total;
	double sum = 0.0;
	double over_pct = 0.0, under_pct = 0.0;
	int cdf_val = 0, p1 = 0, p99 = 255;
	total = w * h;
	build_histogram(y_plane, w, h, hist);
	memset(&r, 0, sizeof(r));

	/* 1. 调用 build_histogram，结果写 r.hist */
	for (i = 0; i < 256; i++) {
		r.hist[i] = hist[i];
	}

	/* 2. min: 从 0 往上找第一个 hist[i] > 0 */
	for (i = 0; i < 256; i++) {
		if (hist[i] > 0) {
			r.min = i;
			break;
		}
	}

	/* 3. max: 从 255 往下找第一个 hist[i] > 0 */
	for (i = 255; i >= 0; i--) {
		if (hist[i] > 0) {
			r.max = i;
			break;
		}
	}

	/* 4. mean = sum(i * hist[i]) / total */
	for (i = 0; i < 256; i++) {
		sum += i * hist[i];
	}
	r.mean = sum / total;
	/* 5. stddev = sqrt(sum( (i-mean)^2 * hist[i] ) / total) */
	sum = 0;
	for (i = 0; i < 256; i++) {
		double diff = i - r.mean;
		sum += diff * diff * hist[i];
	}
	r.stddev = sqrt(sum / total);
	/* 6. median: 累加 hist 到 total/2 */
	sum = 0;
	for (i = 0; i < 256; i++) {
		sum += hist[i];
		if (sum >= total / 2) {
			r.median = i;
			break;
		}
	}

	/* 7. over_pct: hist[241..255] 之和 / total * 100 */
	for (i = 241; i < 256; i++) {
		over_pct += hist[i];
	}
	r.over_pct = (double)over_pct / total * 100;

	/* 8. under_pct: hist[0..9] 之和 / total * 100 */
	for (i = 0; i < 10; i++) {
		under_pct += hist[i];
	}
	r.under_pct = (double)under_pct / total * 100;

	/* 9. dyn_range: CDF 取 P1 和 P99，P99 - P1 */
	for (i = 0; i < 256; i++) {
		cdf_val += hist[i];
		if (cdf_val >= total * 0.01) {
			p1 = i;
			break;
		}
	}
	cdf_val = 0;
	for (i = 0; i < 256; i++) {
		cdf_val += hist[i];
		if (cdf_val >= total * 0.99) {
			p99 = i;
			break;
		}
	}
	r.dyn_range = p99 - p1;
	/* 10. status: mean<60→"too dark", mean>200→"too bright",
				stddev<20→"low contrast", 其余→"normal" */
	if (r.mean < 60)
		r.status = "too dark";
	else if (r.mean > 200)
		r.status = "too bright";
	else if (r.stddev < 20)
		r.status = "low contrast";
	else
		r.status = "normal";
	return r;
}
