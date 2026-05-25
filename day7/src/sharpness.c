#include "iq_analyzer.h"

/* 清晰度分析：Sobel 3×3 边缘检测 */
SharpReport analyze_sharpness(unsigned char* y_plane, int w, int h) {
	SharpReport r;
	double sum;
	int x, y_idx, count, gx, gy;
	double mag;
	unsigned char* p;

	sum = 0.0;
	count = 0;
	p = y_plane;

	/* TODO: 对每个内部像素（跳过边界 1px）做 Sobel
	   Sobel X 核:  [-1  0 +1]    Sobel Y 核: [-1 -2 -1]
					[-2  0 +2]                 [ 0  0  0]
					[-1  0 +1]                 [+1 +2 +1]

	   gx = -1*P[(y-1)*w+(x-1)] + 1*P[(y-1)*w+(x+1)]
			-2*P[(y  )*w+(x-1)] + 2*P[(y  )*w+(x+1)]
			-1*P[(y+1)*w+(x-1)] + 1*P[(y+1)*w+(x+1)];

	   gy = -1*P[(y-1)*w+(x-1)] - 2*P[(y-1)*w+(x  )] - 1*P[(y-1)*w+(x+1)]
			+1*P[(y+1)*w+(x-1)] + 2*P[(y+1)*w+(x  )] + 1*P[(y+1)*w+(x+1)];

	   mag = sqrt(gx*gx + gy*gy);  sum += mag;  count++;
	*/
	for (y_idx = 1; y_idx < h - 1; y_idx++) {
		for (x = 1; x < w - 1; x++) {
			gx = -1 * p[(y_idx - 1) * w + (x - 1)] + 1 * p[(y_idx - 1) * w + (x + 1)] - 2 * p[(y_idx)*w + (x - 1)] + 2 * p[(y_idx)*w + (x + 1)] - 1 * p[(y_idx + 1) * w + (x - 1)] + 1 * p[(y_idx + 1) * w + (x + 1)];
			gy = -1 * p[(y_idx - 1) * w + (x - 1)] - 2 * p[(y_idx - 1) * w + (x)] - 1 * p[(y_idx - 1) * w + (x + 1)] + 1 * p[(y_idx + 1) * w + (x - 1)] + 2 * p[(y_idx + 1) * w + (x)] + 1 * p[(y_idx + 1) * w + (x + 1)];
			mag = sqrt(gx * gx + gy * gy);
			sum += mag;
			count++;
		}
	}

	r.edge_energy = sum / count;

	/* TODO: level 判断
	   edge_energy<15 → "soft"
	   edge_energy<30 → "normal"
	   其余            → "sharp"
	*/
	if (r.edge_energy < 15) {
		r.level = "soft";
	} else if (r.edge_energy < 30) {
		r.level = "normal";
	} else {
		r.level = "sharp";
	}

	return r;
}
