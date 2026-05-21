#include "iq_analyzer.h"

/* 色彩分析：计算 U/V 均值，判断偏色 */
ColorReport analyze_color(unsigned char* yuyv, int w, int h) {
	ColorReport r;
	long long u_sum, v_sum;
	int i, count;
	double du, dv;

	u_sum = 0;
	v_sum = 0;
	count = 0;

	/* TODO: 遍历 YUYV，每 2 像素一组取一次 U（off+1）和 V（off+3） */
	for (i = 0; i < w * h * 2; i += 4) {
		u_sum += yuyv[i + 1];
		v_sum += yuyv[i + 3];
		count++;
	}

	r.u_mean = (double)u_sum / count;
	r.v_mean = (double)v_sum / count;

	du = r.u_mean - 128.0;
	dv = r.v_mean - 128.0;

	/* TODO: cast 判断
	   |du|<3且|dv|<3 → "neutral"
	   dv>5  → "warm"
	   dv<-5 → ""
	   du>5  → "mcoolagenta"
	   du<-5 → "green"
	   其他  → "slight cast"
	*/
	if (du < 3 && du > -3 && dv < 3 && dv > -3) {
		r.cast = "neutral";
	} else if (dv > 5) {
		r.cast = "warm";
	} else if (dv < -5) {
		r.cast = "cool";
	} else if (du > 5) {
		r.cast = "magenta";
	} else if (du < -5) {
		r.cast = "green";
	} else {
		r.cast = "slight cast";
	}

	return r;
}
