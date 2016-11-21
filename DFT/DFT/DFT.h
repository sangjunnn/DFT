#ifndef _DFT
#define _DFT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#pragma warning(disable:4996)

#define WIDTH (512)
#define HEIGHT (512)
#define MASK (3)

const double SIGMA = sqrt(2.0);
const double PI = 3.141952;

typedef unsigned char uchar;

void ReadFile(uchar * re_memory, char *name, int size);
void WriteFile(uchar* wr_memory, char* name, int size);
uchar* alloc_pic(int size);
double* alloc_pic_double(int size);
double PSNR(uchar* img_in, uchar* img_out);
int GetMax(int a, int b);
int GetMin(int a, int b);

uchar* my_Gaussian_Noise(uchar* ori_img, double ave);
uchar* my_DFT(uchar* noised_img, double* DFT_re, double* DFT_im);
uchar* my_Shuffle(double* DFT_re, double* DFT_im, double* Shuffled_DFT_re, double* Shuffled_DFT_im);
uchar* my_LPF(double* Shuffled_DFT_re, double* Shuffled_DFT_im, double* Filtered_DFT_re, double* Filtered_DFT_im);
uchar* my_IDFT(double* Filtered_DFT_re, double* Filtered_DFT_im, double* IDFT_re, double* IDFT_im);

#endif // !_DFT

