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
