//--------------------------------------------------------------------------
// dmatrix.h - Utility functions for diagonal matrix
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
#ifndef __FASTICA_DMATRIX_H
#define __FASTICA_DMATRIX_H
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
// Diagonal Matrix
//--------------------------------------------------------------------------
inline Matrix DMatrix(const BVector &v)
{
	return BMatrix(v.asDiagonal());
}
//--------------------------------------------------------------------------
inline Matrix DMatrixOf1Sqrt(const Vector &v)
{
	return DMatrix((wtype)1.0 / v.array().sqrt());
}
//--------------------------------------------------------------------------
inline Matrix DMatrixOfSqrt(const Vector &v)
{
	return DMatrix(v.array().sqrt());
}
//--------------------------------------------------------------------------
} // namespace FastICA
//--------------------------------------------------------------------------
#endif
