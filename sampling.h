//--------------------------------------------------------------------------
// sampling.h - Implementation of stochastic data sampling
//--------------------------------------------------------------------------
// Author: Lam H. Dao <daohailam(at)yahoo(dot)com>
//--------------------------------------------------------------------------
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//--------------------------------------------------------------------------
#ifndef __FASTICA_SAMPLING_H
#define __FASTICA_SAMPLING_H
//--------------------------------------------------------------------------
#include "bsets.h"
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
const int SAMPLING_SIZE = 512 * 1024;
//--------------------------------------------------------------------------
struct OutlierOption {
	bool Detection;
	double Threshold;
} Outlier = { false, 0.01 };
//--------------------------------------------------------------------------
bool clayout = true;
//--------------------------------------------------------------------------
void CalculateValueRange(double rmax, double threshold, double &vmin, double &vmax)
{
	vmin = ceil(threshold * rmax);
	vmax = rmax - vmin;
	vmin = vmin - 1;
}
//--------------------------------------------------------------------------
template<typename dtype>
double FindDataRange(dtype *data, int size)
{
	double rmax = 0;
	for (int n = 0; n < size; n++) {
		if (data[n] > rmax) rmax = data[n];
	}
	if (rmax < 256)
		rmax = 255;
	else if (rmax < 4096)
		rmax = 4095;
	else
		rmax = 65535;
	return rmax;
}
//--------------------------------------------------------------------------
template<typename dtype>
dtype *RemoveOutliers(dtype *data, int nc, int &size)
{
	if (!Outlier.Detection)
		return data;

	int n = 0;
	int t = nc * size;
	dtype *r = new dtype[t];

	double rmax = FindDataRange(data, t);
	double vmin, vmax, threshold = Outlier.Threshold;
__start__:
	CalculateValueRange(rmax, threshold, vmin, vmax);
	if (clayout) {
		for (int idx = 0; idx < t; idx += nc) {
			int i;
			for (i = 0; i < nc; i++) {
				double d = data[idx + i];
				// skip if this position has outlier
				if (d < vmin || d > vmax) break;
				r[n + i] = (dtype)d;
			}
			if (i >= nc) {
				n += nc;
			}
		}
	}
	else {
		BitSet os(size);
		dtype *dp = data;
		// mark all outliers' position
		for (int i = 0; i < nc; i++) {
			for (int idx = 0; idx < size; idx++) {
				double d = dp[idx];
				if (d < vmin || d > vmax) {
					os += idx;
				}
			}
			dp += size;
		}
		dp = data;
		// copy data where it is not an outlier
		for (int i = 0; i < nc; i++) {
			for (int idx = 0; idx < size; idx++) {
				if (!os[idx]) {
					r[n++] = dp[idx];
				}
			}
			dp += size;
		}
	}
	size = n / nc;
	if (size == 0) {
		if (vmax == rmax) {
			throw E_EMPTY_DATA_SET;
		}
		threshold = vmin <= 1.0 ? 0.0 : threshold / 2;
		goto __start__;
	}
	return r;
}
//--------------------------------------------------------------------------
template<typename dtype>
int FilterSamples(dtype *output, dtype *data, int nc, int size,
				RandomSet &candidates, double vmin, double vmax)
{
	int n = 0;
	if (clayout) { // column layout
		for (int idx = candidates.next(); idx >= 0; idx = candidates.next()) {
			int i, b = nc * idx;
			for (i = 0; i < nc; i++) {
				double d = data[b + i];
				if (d < vmin || d > vmax) break;
				output[n + i] = (dtype)d;
			}
			if (i >= nc) {
				n += nc;
			}
		}
	}
	else { // row layout
		dtype *dp = data;
		RandomSet cs(candidates);
		// clear all positions in randomset if it has outlier
		for (int n = 0; n < nc; n++) {
			for (int idx = cs.next(); idx >= 0; idx = cs.next()) {
				double d = dp[idx];
				if (d < vmin || d > vmax) {
					cs -= idx;
				}
			}
			dp += size;
		}
		dp = data;
		// copy the remains
		for (int n = 0; n < nc; n++) {
			for (int idx = cs.next(); idx >= 0; idx = cs.next()) {
				output[n++] = dp[idx];
			}
			dp += size;
		}
	}
	return n;
}
//--------------------------------------------------------------------------
template<typename dtype>
int SelectSamples(dtype *output, dtype *data, int nc, int size, RandomSet &candidates)
{
	int n = 0;
	if (clayout) {
		for (int idx = candidates.next(); idx >= 0; idx = candidates.next()) {
			dtype *dp = &data[nc * idx];
			for (int i = 0; i < nc; i++) {
				output[n++] = dp[i];
			}
		}
	}
	else {
		for (int c = 0; c < nc; c++) {
			dtype *dp = &data[c * size];
			for (int idx = candidates.next(); idx >= 0; idx = candidates.next()) {
				output[n++] = dp[idx];
			}
		}
	}
	return n;
}
//--------------------------------------------------------------------------
template<typename dtype>
dtype *Sampling(dtype *data, int nc, int &size)
{
	int rsize = Outlier.Detection ? (int)(1.25 * SAMPLING_SIZE) : SAMPLING_SIZE;
	RandomSet rs(size, rsize);
	dtype *r = new dtype[nc * rsize];

	int n;
	if (Outlier.Detection) {
		double rmax = FindDataRange(data, nc * size);
		double vmin, vmax, threshold = Outlier.Threshold;
	__filter_start__:
		CalculateValueRange(rmax, threshold, vmin, vmax);
		n = FilterSamples(r, data, nc, size, rs, vmin, vmax);
		if (n == 0) {
			if (vmax == rmax) {
				throw E_EMPTY_DATA_SET;
			}
			threshold = vmin <= 1.0 ? 0.0 : threshold / 2;
			goto __filter_start__;
		}
	}
	else {
		n = SelectSamples(r, data, nc, size, rs);
	}
	size = n / nc;
	return r;
}
//--------------------------------------------------------------------------
} // namespace FastICA
//--------------------------------------------------------------------------
#endif
