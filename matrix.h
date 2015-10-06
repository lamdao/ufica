#ifndef __FASTICA_MATRIX_H
#define __FASTICA_MATRIX_H
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
// Extended Matrix class using for FastICA
class Matrix : public BMatrix
{
public:
	Matrix() : BMatrix() {}
	Matrix(int r, int c) : BMatrix(r, c) {}
	template<typename OtherDerived>
	Matrix(const MatrixBase<OtherDerived> &other) : BMatrix(other) {}
	Matrix(const BMatrix &other) : BMatrix(other) {}

	Matrix T() const
	{
		return transpose();
	}

	Matrix Invert() const
	{
		return BMatrix(inverse());
	}

	Matrix operator + (const Scalar v)
	{
		return BMatrix(array() + v);
	}

	Matrix operator - (const Scalar v)
	{
		return BMatrix(array() - v);
	}

	Matrix operator / (const Scalar v)
	{
		return BMatrix(array() / v);
	}

	Matrix operator ^ (const Scalar v)
	{
		return BMatrix(array().pow(v));
	}

	Matrix operator + (const BMatrix &m)
	{
		return BMatrix(array() + m.array());
	}

	Vector SumByColumn() const
	{
		return colwise().sum();
	}

	Scalar MinAbsDiagonalCoeff() const
	{
		return diagonal().array().abs().minCoeff();
	}

	Scalar MaxSumByColumn() const
	{
		return colwise().sum().maxCoeff();
	}

	void Show(FILE *out = stdout, const char *fmt = 0) const
	{
		if (!fmt) fmt = "% 10.3f";
		for (int r = 0; r < rows(); r++) {
			for (int c = 0; c < cols(); c++)
				fprintf(out, fmt, (*this)(r, c));
			fprintf(out, "\n");
		}
	}

	static Matrix RandomU(int r, int c)
	{
		Matrix mx = BMatrix::Random(r, c);
		return (mx + (Scalar)1.0) / (Scalar)2.0;
	}

	static Matrix RandomN(int r, int c)
	{
		return BMatrix::Random(r, c);
	}
};
}
//--------------------------------------------------------------------------
#endif
