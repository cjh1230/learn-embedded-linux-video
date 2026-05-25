#include "iq_analyzer.h"

/* 噪点分析：相邻像素差方差 */
NoiseReport analyze_noise(unsigned char* y_plane, int w, int h) {
	NoiseReport r;
	double y_sum = 0.0;
	int x, y_idx;
	int diff;
	double uv_sum = 0.0;
	int uv_cnt = 0, y_cnt = 0;

	memset(&r, 0, sizeof(r));

	/* TODO: Y 噪点 — 水平方向相邻像素差平方的均值 */
	/* diff = y_plane[y*w+x] - y_plane[y*w+x+1]; y_sum += diff*diff; */
	for (y_idx = 0; y_idx < h; y_idx++) {
		for (x = 0; x < w - 1; x++) {
			diff = y_plane[y_idx * w + x] - y_plane[y_idx * w + x + 1];
			y_sum += diff * diff;
			y_cnt++;
		}
	}
	r.y_noise_var = y_sum / y_cnt;
	/* TODO: UV 噪点 — 隔 2 行 2 列采样，同样算差平方的均值 */
	for (y_idx = 0; y_idx < h - 2; y_idx += 2) {
		for (x = 0; x < w - 2; x += 2) {
			diff = y_plane[y_idx * w + x] - y_plane[y_idx * w + x + 2];
			uv_sum += diff * diff;
			uv_cnt++;
		}
	}
	r.uv_noise_var = uv_sum / uv_cnt;

	/* TODO: y_level 判断
	   var<5  → "clean"
	   var<15 → "slight"
	   var<30 → "moderate"
	   其余   → "heavy"
	   uv_level 同样阈值
	*/
	if (r.y_noise_var < 5) {
		r.y_level = "clean";
	} else if (r.y_noise_var < 15) {
		r.y_level = "slight";
	} else if (r.y_noise_var < 30) {
		r.y_level = "moderate";
	} else {
		r.y_level = "heavy";
	}

	/* 临时填充，防止 uninitialized 警告 */
	if (r.uv_noise_var < 5) {
		r.uv_level = "clean";
	} else if (r.uv_noise_var < 15) {
		r.uv_level = "slight";
	} else if (r.uv_noise_var < 30) {
		r.uv_level = "moderate";
	} else {
		r.uv_level = "heavy";
	}

	return r;
}
