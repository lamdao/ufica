#ifndef __FASTICA_VECTOR_H
#define __FASTICA_VECTOR_H
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
// Extended Vector class using for FastICA
class Vector : public BVector
{
public:
	Vector() : BVector() {}
	template<typename OtherDerived>
	Vector(const MatrixBase<OtherDerived> &other) : BVector(other) {}

	Vector operator + (const Scalar v)
	{
		return BVector(array() + v);
	}

	Vector operator - (const Scalar v)
	{
		return BVector(array() - v);
	}

	friend Vector operator + (const Scalar s, const Vector &v)
	{
		return BVector(s + v.array());
	}

	friend Vector operator - (const Scalar s, const Vector &v)
	{
		return BVector(s - v.array());
	}

	friend Vector operator * (const Vector &v1, const Vector &v2)
	{
		return BVector(v1.array() * v2.array());
	}

	static Vector RandomU(int n)
	{
		Vector vx = BVector::Random(n);
		return (vx + (Scalar)1.0) / (Scalar)2.0;
	}

	static Vector RandomU(int n, Scalar vmin, Scalar vmax = (Scalar)1.0)
	{
		if (vmin > vmax) {
			Scalar v = vmin;
			vmin = vmax;
			vmax = v;
		}
		Vector vx = (vmax - vmin) * RandomU(n);
		return vx + vmin;
	}
};
}
//--------------------------------------------------------------------------
#endif
