//--------------------------------------------------------------------------
// FastICA.cpp - Library API
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
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
//------------------------------------------------------------------------
#include "typedefs.h"
#include "vector.h"
#include "matrix.h"
#include "sampling.h"
#include "dmatrix.h"
#include "guess.h"
#include "core.h"
#include "unmix.h"
#include "utils.h"
//--------------------------------------------------------------------------
// Library interfaces
//--------------------------------------------------------------------------
DLAPI wtype *Estimate(void *data, int dtype, int nc, int ns)
{
	try {
		FastICA::error = 0;
		FastICA::Execute(data, dtype, nc, ns);
		return FastICA::Mx.data();
	}
	catch (const char *msg) {
		FastICA::error = msg;
	}
	return 0;
}
//--------------------------------------------------------------------------
DLAPI wtype *Unmix(wtype *ux, void *data, int dtype, int nc, int ns)
{
	if (ux == NULL) {
		if (!Estimate(data, dtype, nc, ns)) {
			return 0;
		}
	}
	else {
		FastICA::SetMx(ux, nc > ns ? ns : nc);
	}
	FastICA::Unmix((FastICA::US *)data, nc, ns);
	return FastICA::Mx.data();
}
//--------------------------------------------------------------------------
DLAPI wtype *GetChannelMax()
{
	return FastICA::Cmax.data();
}
//--------------------------------------------------------------------------
DLAPI wtype *GetStatisticMax(FastICA::US *data, int nc, int ns)
{
	FastICA::CalculateStatisticMax(data, nc, ns);
	return FastICA::Smax.data();
}
//--------------------------------------------------------------------------
DLAPI const char *GetErrorMessage()
{
	return FastICA::error;
}
//--------------------------------------------------------------------------
// Interfaces for IDL style when working with shared libraries
//--------------------------------------------------------------------------
DLAPI int DEstimate(int argc, char *argv[])
{
	double *mx = (double *)argv[0];
	void *data = (void *)argv[1];
	int dtype = *(int *)argv[2];
	int nc = *(int *)argv[3];
	int ns = *(int *)argv[4];
	if (!Estimate(data, dtype, nc, ns))
		return -1;
	memcpy(mx, FastICA::Mx.data(), FastICA::Mx.size() * sizeof(wtype));
	return 0;
}
//--------------------------------------------------------------------------
DLAPI int DUnmix(int argc, char *argv[])
{
	double *mx = (double *)argv[0];
	void *data = (void *)argv[1];
	int dtype = *(int *)argv[2];
	int nc = *(int *)argv[3];
	int ns = *(int *)argv[4];

	if (mx[0] == 0) {
		if (!Unmix(0, data, dtype, nc, ns))
			return -1;
	}
	else {
		Unmix(mx, data, dtype, nc, ns);
	}
	memcpy(mx, FastICA::Mx.data(), FastICA::Mx.size() * sizeof(wtype));
	return 0;
}
//--------------------------------------------------------------------------
// A dummy function used to call when unload DLL from IDL
//--------------------------------------------------------------------------
DLAPI int DUnload(int argc, char *argv[])
{
	return 0;
}
//--------------------------------------------------------------------------
#ifdef _WIN32
#include "dllmain.cpp"
#endif
//--------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------
DLAPI void About()
{
#ifdef _WIN32
	WinDLL::About();
#else
	printf("--------------------------------------------\n");
	printf("FastICA Engine for Microscope Image Unmixing\n");
	printf("               Version 1.0.1\n");
	printf("--------------------------------------------\n");
//	printf("  Lam H. Dao <daohailam(at)yahoo(dot)com>\n\n");
#endif
}
//--------------------------------------------------------------------------
#ifndef _WIN32
static void __attribute__ ((constructor)) init(void)
{
	About();
}
#endif
//--------------------------------------------------------------------------
