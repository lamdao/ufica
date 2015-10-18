//--------------------------------------------------------------------------
// utils.h - Utility functions for saving/setting mixing-matrix
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
#ifndef __FASTICA_UTILS_H
#define __FASTICA_UTILS_H
//--------------------------------------------------------------------------
namespace FastICA {
extern Matrix Mx;
//--------------------------------------------------------------------------
void SaveMx(const char *fn)
{
	FILE *fp = fopen(fn, "wt+");
	if (fp) {
		Mx.Show(fp);
		fclose(fp);
	}
}
//--------------------------------------------------------------------------
void SetMx(double *mx, int nc)
{
	Mx = Matrix::Zero(nc, nc);
	memcpy(Mx.data(), mx, nc * nc * sizeof(wtype));
}
//------------------------------------------------------------------------
} // namespace FastICA
//--------------------------------------------------------------------------
#endif
