//--------------------------------------------------------------------------
// typedefs.h - Data types and macros definition
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
#ifndef __FASTICA_TYPEDEFS_H
#define __FASTICA_TYPEDEFS_H
//--------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
//------------------------------------------------------------------------
#include <Eigen>
#include <Dense>
#include <Eigenvalues>
//------------------------------------------------------------------------
using namespace std;
using namespace Eigen;
//--------------------------------------------------------------------------
#ifdef _WIN32
#define DLAPI	extern "C" __declspec(dllexport)
#else
#define DLAPI	extern "C"
#endif
//------------------------------------------------------------------------
#ifdef USE_FLOAT
#define	BVector	VectorXf
#define	BMatrix	MatrixXf
#define wtype	float
#else
#define	BVector	VectorXd
#define	BMatrix	MatrixXd
#define wtype	double
#endif
//--------------------------------------------------------------------------
#ifdef DEBUG
#define dprintf(fmt,...)		fprintf(stderr,fmt,...)
#else
#define dprintf(fmt,...)
#endif
//------------------------------------------------------------------------
#ifndef MAX_NUM_CHANNELS
#define MAX_NUM_CHANNELS	8
#endif
//--------------------------------------------------------------------------
namespace FastICA {
	typedef unsigned long long	u64;
	typedef unsigned char	UC;
	typedef unsigned short	US;
	typedef unsigned int	UI;
	typedef float			FL;
	typedef double			DB;

	const char *E_EMPTY_DATA_SET = "Empty data set";
	const char *error = NULL;
}
//--------------------------------------------------------------------------
#endif
