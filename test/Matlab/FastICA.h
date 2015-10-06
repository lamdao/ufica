#ifndef __FAST_ICA_H
#define __FAST_ICA_H
double *Estimate(void *data, int dtype, int nc, int ns);
double *Unmix(double *ux, void *data, int dtype, int nc, int ns);
#endif