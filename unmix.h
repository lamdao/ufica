//--------------------------------------------------------------------------
// unmix.h - Utility functions to unmix signals after having mixing-matrix
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
#ifndef __FASTICA_UNMIX_H
#define __FASTICA_UNMIX_H
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
Vector Cmax = Vector::Zero(MAX_NUM_CHANNELS);
//--------------------------------------------------------------------------
void Unmix(US *data, int nc, int ns)
{
	extern Matrix Mx;

	clayout = true;
	if (nc > ns) {
		int tt = nc;
		nc = ns;
		ns = tt;
		clayout = false;
	}
	Matrix Ux = Mx.Invert();
	Vector rv = Vector::Zero(nc);
	register US *dp = data;

	Cmax.setZero();
	if (clayout) {
		for (int k = 0; k < ns; k++) {
			for (int i = 0; i < nc; i++) rv[i] = dp[i];
			for (int i = 0; i < nc; i++) {
				wtype v = rv.dot(Ux.row(i));
				dp[i] = v < 0 ? 0 : (US)(v + 0.5);
				if (v > Cmax[i]) Cmax[i] = v;
			}
			dp += nc;
		}
	}
	else {
		US **ds = new US * [nc];
		for (int n = 0; n < nc; n++) {
			ds[n] = dp;
			dp += ns;
		}

		for (int k = 0; k < ns; k++) {
			for (int i = 0; i < nc; i++) rv[i] = *ds[i];
			for (int i = 0; i < nc; i++) {
				wtype v = rv.dot(Ux.row(i));
				*ds[i]++ = v < 0 ? 0 : (US)(v + 0.5);
				if (v > Cmax[i]) Cmax[i] = v;
			}
		}
		delete[] ds;
	}
}
//--------------------------------------------------------------------------
Vector Smax = Vector::Zero(MAX_NUM_CHANNELS);
//--------------------------------------------------------------------------
void CalculateStatisticMax(US *data, int nc, int ns)
{
	clayout = true;
	if (nc > ns) {
		int tt = nc;
		nc = ns;
		ns = tt;
		clayout = false;
	}
	// TODO:
	// If this function is called after Umix, Cmax contains the maximum
	// values of each channel. Otherwise, it has to estimate maximum values.

	UI cmx[MAX_NUM_CHANNELS], cts[MAX_NUM_CHANNELS];
	std::vector<UI> cfs[MAX_NUM_CHANNELS];
	for (int n = 0; n < nc; n++) {
		cmx[n] = (UI)(Cmax[n] + 0.5);
		cfs[n] = std::vector<UI>(cmx[n] + 1);
		cts[n] = 0;
	}

	US *dp = data;
	if (clayout) {
		for (int k = 0; k < ns; k++) {
			for (int i = 0; i < nc; i++) {
				US sv = dp[i];
				if (sv > 0) {
					cfs[i][sv] += 1;
					cts[i] += 1;
				}
			}
			dp += nc;
		}
	}
	else {
		for (int i = 0; i < nc; i++) {
			for (int k = 0; k < ns; k++) {
				US sv = *dp++;
				if (sv > 0) {
					cfs[i][sv] += 1;
					cts[i] += 1;
				}
			}
		}
	}

	for (int i = 0; i < nc; i++) {
		std::vector<UI> &cf = cfs[i];
		UI n = cmx[i], l = n / 3;
		UI t = cts[i], s = cf[n];
		while (n > l && (((double)s / t) < 0.0001)) {
			s += cf[--n];
		}
		Smax[i] = n;
	}
}
//--------------------------------------------------------------------------
} // namespace FastICA
//--------------------------------------------------------------------------
#endif
