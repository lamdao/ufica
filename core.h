#ifndef __FASTICA_CORE_H
#define __FASTICA_CORE_H
//--------------------------------------------------------------------------
// The core of FastICA algorithm
//--------------------------------------------------------------------------
namespace FastICA {
//--------------------------------------------------------------------------
Matrix Mx;	// Result of mixing matrix
Matrix WMx;	// Whitening matrix
Matrix DMx;	// De-whitening matrix
//--------------------------------------------------------------------------
// Fast ICA functions
//--------------------------------------------------------------------------
Matrix GradientUpdate(Matrix &Bx)
{
	EigenSolver<BMatrix> es(Bx.T() * Bx);
	Vector ev = es.eigenvalues().real();
	Matrix et = es.eigenvectors().real();
	return Bx * (et * DMatrixOf1Sqrt(ev) * et.T());
}
//--------------------------------------------------------------------------
template<typename dtype>
Matrix ZeroMean(dtype *data, int &n, int &size)
{
	// Detect data layout (column/row based form)
	// clayout == true represents column based form
	clayout = true;
	if (n > size) {
		int t = n;
		n = size;
		size = t;
		clayout = false;
	}

	dtype *dsrc = size <= SAMPLING_SIZE ?
					RemoveOutliers(data, n, size) :
					Sampling(data, n, size);
	dtype *d = dsrc;

	Matrix zx(n, size);
	if (clayout) {
		std::vector<wtype> sv(n);
		for (int i = 0; i < n; i++) sv[i] = 0;
		for (int k = 0; k < size; k++) {
			for (int i = 0; i < n; i++, d++) {
				zx(i, k) = (wtype)*d;
				sv[i] += (wtype)*d;
			}
		}
		for (int i = 0; i < n; i++) {
			zx.row(i).array() -= sv[i] / size;
		}
	}
	else {
		for (int i = 0; i < n; i++) {
			wtype sv = 0;
			for (int k = 0; k < size; k++, d++) {
				zx(i, k) = (wtype)*d;
				sv += (wtype)*d;
			}
			zx.row(i).array() -= sv / size;
		}
	}
	if (dsrc != data) {
		delete[] dsrc;
	}
	return zx;
}
//--------------------------------------------------------------------------
Matrix Whiten(const Matrix &Zx, Matrix &WMx, Matrix &DMx)
{
	Matrix Mx = Zx * Zx.T();
	EigenSolver<BMatrix> es(Mx / Mx.MaxSumByColumn());
	Vector Ev = es.eigenvalues().real();
	Matrix Et = es.eigenvectors().real();
	WMx = DMatrixOf1Sqrt(Ev) * Et.T();
	DMx = Et * DMatrixOfSqrt(Ev);
	return WMx * Zx;
}
//--------------------------------------------------------------------------
Matrix Init(void *data, int dtype, int &n, int &size)
{
	Matrix Zx;
	switch (dtype % 10) {
	default:
	case 1:		// byte or uchar
		Zx = ZeroMean((UC *)data, n, size);
		break;
	case 2:		// short & ushort
		Zx = ZeroMean((US *)data, n, size);
		break;
	case 3:		// int & uint
		Zx = ZeroMean((UI *)data, n, size);
		break;
	case 4:		// float
		Zx = ZeroMean((FL *)data, n, size);
		break;
	case 5:		// double
		Zx = ZeroMean((DB *)data, n, size);
		break;
	}

	return Whiten(Zx, WMx, DMx);
}
//--------------------------------------------------------------------------
void CalculateP3SP4(const Matrix &Tx, Matrix &P3, Matrix &Bt, Matrix &Dx, int nc, int ns)
{
	wtype *b = (wtype *)Bt.data();
	Bt.setZero();
	for (int i = 0; i < ns; i++) {
		for (int j = 0; j < nc; j++) {
			wtype d = Tx(i, j);
			b[j] += (P3(i, j) = pow(d, (wtype)3.0)) * d;
		}
	}

	for (int i = 0; i < nc; i++)
		Dx(i, i) = (wtype)1.0 / b[i];
}
//--------------------------------------------------------------------------
// Convergence condition
//--------------------------------------------------------------------------
wtype deps = (wtype)1E-3;	// quick convergence condition
wtype teps = (wtype)1E-4;	// fine tuning convergence condition
//--------------------------------------------------------------------------
// Function:
//    Execute(sources, dtype, nc, ns, niters)
//
// Paramteres:
//  - sources - 2D row-based array stores source signals
//  - dtype   - data type of source signals
//  - nc      - number of mixed signals (components)
//  - ns      - number of samples
//--------------------------------------------------------------------------
void Execute(void *sources, int dtype, int nc, int ns, int niters = 250)
{
	int ntry = 0;
	Matrix Wx = Init(sources, dtype, nc, ns);
	Matrix Wt = Wx.T();

	Matrix Bx(nc, nc), Bo(nc, nc), Bm(nc, nc);
	Matrix P3(ns, nc), Bt(1, nc);
	Matrix Tx(ns, nc), Dx = Matrix::Zero(nc, nc);

__start__:
	ntry += 1;
	Bx = GenerateBx(nc);
	Bx = GradientUpdate(Bx);

	wtype Bf = (wtype)0.01;
	wtype Er, Em = (wtype)1E32;
	wtype nsx = ns <= 1E6 ? ns : ns / (10 * nc);
	wtype eps = deps;

	bool tuning = false;
	for (int i = 0; i < niters; i++) {
		Bo = Bx;
		if (!tuning) {
			Bx = Wx * (Matrix(Wt * Bx) ^ (wtype)3.0) / nsx - (wtype)3.0 * Bx;
		}
		else {
			Tx.noalias() = Wt * Bx;
			CalculateP3SP4(Tx, P3, Bt, Dx, nc, ns);
			Bx += Bf * (Bx * Dx * (Tx.T() * P3 - DMatrix(Bt)));
		}
		Bx = GradientUpdate(Bx);
		Er = (wtype)1.0 - Matrix(Bx.T() * Bo).MinAbsDiagonalCoeff();
		if (Er < 0) {
			if (ntry > 3) {
				tuning = true;
				continue;
			}
			if (!tuning) {
				goto __start__;
			}
			Bx = Bo;
			Bf = Bf / 10;
			if (Bf < 1E-6) break;
		}
		else if (Er < eps) {
			if (tuning) break;
			if (eps < teps) break;
			eps = teps;

			tuning = true;
		}
		if (Er > 0 && Er < Em) {
			Em = Er;
			Bm = Bx;
		}
		dprintf("%4d: %d - %5.3e\n", i, tuning, Er);
	}

	if (Em < Er) {
		Bx = Bm;
	}

#ifdef USE_CONSTRAINTS
	Mx = ApplyConstraints(DMx * Bx);
#else
	Mx = DMx * Bx;
#endif
}
//--------------------------------------------------------------------------
} // namespace FastICA
//--------------------------------------------------------------------------
#endif
