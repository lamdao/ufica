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
