//--------------------------------------------------------------------------
// guess.h - Heuristic guess for FastICA initialization
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
#ifndef __FASTICA_GUESS_H
#define __FASTICA_GUESS_H
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
enum GuessForms {
	RANDOM = 0,
	IDENTITY = 1,
	UNIFORM = 2,
	NORMAL = 3
};
//--------------------------------------------------------------------------
// Initial guess matrix generators
//--------------------------------------------------------------------------
Matrix GenerateMx(int n)
{
	Vector vx = Vector::RandomU(n, (wtype)0.5, (wtype)0.75);
	Vector vt = Vector::Zero(n);
	Matrix mx(n, n);

	int r = 0;
	while (r < n - 1) {
		vt += vx;
		for (int i = 0; i < n; i++)
			mx((r + i) % n, i) = vx[i];
		vx = ((wtype)1.0 - vt) * Vector::RandomU(n, (wtype)0.25, (wtype)0.5);
		r += 1;
	}
	for (int i = 0; i < n; i++) {
		mx((r + i) % n, i) = (wtype)1.0 - vt[i];
	}

	return mx;
}
//--------------------------------------------------------------------------
extern Matrix DMx;
//--------------------------------------------------------------------------
inline Matrix GenerateBx(int n, GuessForms mode = RANDOM)
{
	if (mode == IDENTITY)
		return Matrix::Identity(n, n);
	if (mode == UNIFORM)
		return Matrix::RandomU(n, n);
	if (mode == NORMAL)
		return Matrix::RandomN(n, n);

	// default mode = mixed form random
	return DMx.inverse() * GenerateMx(n);
}
//--------------------------------------------------------------------------
inline Matrix GenerateBx(const Matrix &mx)
{
	return DMx.inverse() * mx;
}
//--------------------------------------------------------------------------
} // namespace FastICA
//--------------------------------------------------------------------------
#endif
