#include "DFT.h"

void ReadFile(uchar* re_memory, char* name, int size) {
	FILE *fin;

	fin = fopen(name, "rb");

	if (!fin) {
		fprintf(stderr, "no input");
		exit(1);
	}

	fread(re_memory, sizeof(uchar), size, fin);

	fclose(fin);
}

void WriteFile(uchar* wr_memory, char* name, int size) {
	FILE *fout;

	fout = fopen(name, "wb");

	if (!fout) {
		fprintf(stderr, "no output");
		exit(1);
	}

	fwrite(wr_memory, sizeof(uchar), size, fout);

	fclose(fout);
}

uchar* alloc_pic(int size) {
	uchar* pic = (uchar*)calloc(size, sizeof(uchar));
	if (pic == NULL) {
		fprintf(stderr, "wrong allocation");
		exit(1);
	}
	return pic;
}

double* alloc_pic_double(int size) {
	double* pic = (double*)calloc(size, sizeof(double));
	if (pic == NULL) {
		fprintf(stderr, "wrong allocation");
		exit(1);
	}
	return pic;
}

double PSNR(uchar* img_in, uchar* img_out) {
	double MSE;
	double result = 0;
	double psnr_;

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			result += (img_in[i*WIDTH + j] - img_out[i*WIDTH + j])*(img_in[i*WIDTH + j] - img_out[i*WIDTH + j]);
		}
	}
	MSE = result / (WIDTH*HEIGHT);

	psnr_ = 20 * log10(255 / sqrt(MSE));

	return psnr_;
}

int GetMax(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}

int GetMin(int a, int b) {
	if (a > b)
		return b;
	else
		return a;
}

uchar* my_Gaussian_Noise(uchar* ori_img, double ave) {
	uchar* noised_img = alloc_pic(WIDTH*HEIGHT);

	for (int h = 0; h < HEIGHT; h++) {
		for (int w = 0; w < WIDTH; w++) {

			double x = 10 * (double)rand() / 0x7fff;

			double ro;

			ro = (1 / (SIGMA*sqrt(2 * PI)))*exp(((-1)*pow((x - ave), 2)) / (2 * pow(SIGMA, 2)));

			if (0 == rand() % 2) {
				noised_img[h*WIDTH + w] = floor(ori_img[h*WIDTH + w] + ori_img[h*WIDTH + w] * ro);
			}
			else {
				noised_img[h*WIDTH + w] = floor(ori_img[h*WIDTH + w] - ori_img[h*WIDTH + w] * ro);
			}

			if (floor(ori_img[h*WIDTH + w] + ori_img[h*WIDTH + w] * ro)>255) {
				noised_img[h*WIDTH + w] = 255;
			}
			else if (floor(ori_img[h*WIDTH + w] - ori_img[h*WIDTH + w] * ro) < 0) {
				noised_img[h*WIDTH + w] = 0;
			}
		}
	}
	return noised_img;
}

uchar* my_DFT(uchar* noised_img, double* DFT_re, double* DFT_im) {
	uchar* DFT_img = alloc_pic(WIDTH*HEIGHT);
	double* DFT_re_hor_img = alloc_pic_double(WIDTH*HEIGHT);
	double* DFT_im_hor_img = alloc_pic_double(WIDTH*HEIGHT);

	double real;
	double imaginary;
	double sum;

	for (int h = 0; h < HEIGHT; h++) {
		for (int w = 0; w < WIDTH; w++) {

			real = imaginary = 0;

			for (int n = 0; n < WIDTH; n++) {

				real += noised_img[h*WIDTH + n] * cos(((-2)*PI*w*n) / WIDTH);
				imaginary += noised_img[h*WIDTH + n] * sin(((-2)*PI*w*n) / WIDTH);
			}
			DFT_re_hor_img[h*WIDTH + w] = real;
			DFT_im_hor_img[h*WIDTH + w] = imaginary;
		}
	}
	for (int w = 0; w < WIDTH; w++) {
		for (int h = 0; h < HEIGHT; h++) {

			real = imaginary = sum = 0;

			for (int n = 0; n < HEIGHT; n++) {

				real += DFT_re_hor_img[n*WIDTH + w] * cos(((-2)*PI*h*n) / WIDTH)
					- DFT_im_hor_img[n*WIDTH + w] * sin(((-2)*PI*h*n) / WIDTH);

				imaginary += DFT_re_hor_img[n*WIDTH + w] * sin(((-2)*PI*h*n) / WIDTH)
					+ DFT_im_hor_img[n*WIDTH + w] * cos(((-2)*PI*h*n) / WIDTH);
			}
			DFT_re[h*WIDTH + w] = real;
			DFT_im[h*WIDTH + w] = imaginary;
			sum = sqrt(real*real + imaginary*imaginary) / 1000;
			DFT_img[h*WIDTH + w] = sum>255 ? 255 : sum < 0 ? 0 : sum;
		}
	}
	free(DFT_re_hor_img);
	free(DFT_im_hor_img);

	return DFT_img;
}

uchar* my_Shuffle(double* DFT_re, double* DFT_im, double* Shuffled_DFT_re, double* Shuffled_DFT_im) {
	uchar* Shuffled_img = alloc_pic(WIDTH*HEIGHT);

	double result = 0;

	for (int h = 0; h < HEIGHT / 2; h++) {
		for (int w = 0; w < WIDTH / 2; w++) {
			Shuffled_DFT_re[h*WIDTH + w] = DFT_re[((HEIGHT / 2) + h)*WIDTH + (WIDTH / 2) + w];
			Shuffled_DFT_im[h*WIDTH + w] = DFT_im[((HEIGHT / 2) + h)*WIDTH + (WIDTH / 2) + w];
		}
	}
	for (int h = 0; h < HEIGHT / 2; h++) {
		for (int w = WIDTH / 2; w < WIDTH; w++) {
			Shuffled_DFT_re[h*WIDTH + w] = DFT_re[((HEIGHT / 2) + h)*WIDTH + w - (WIDTH / 2)];
			Shuffled_DFT_im[h*WIDTH + w] = DFT_im[((HEIGHT / 2) + h)*WIDTH + w - (WIDTH / 2)];
		}
	}
	for (int h = HEIGHT / 2; h < HEIGHT; h++) {
		for (int w = 0; w < WIDTH / 2; w++) {
			Shuffled_DFT_re[h*WIDTH + w] = DFT_re[(h - (HEIGHT / 2))*WIDTH + (WIDTH / 2) + w];
			Shuffled_DFT_im[h*WIDTH + w] = DFT_im[(h - (HEIGHT / 2))*WIDTH + (WIDTH / 2) + w];
		}
	}
	for (int h = HEIGHT / 2; h < HEIGHT; h++) {
		for (int w = WIDTH / 2; w < WIDTH; w++) {
			Shuffled_DFT_re[h*WIDTH + w] = DFT_re[(h - (HEIGHT / 2))*WIDTH + w - (WIDTH / 2)];
			Shuffled_DFT_im[h*WIDTH + w] = DFT_im[(h - (HEIGHT / 2))*WIDTH + w - (WIDTH / 2)];
		}
	}
	for (int h = 0; h < HEIGHT; h++) {
		for (int w = 0; w < WIDTH; w++) {
			result = sqrt(pow(Shuffled_DFT_re[h*WIDTH + w], 2) + pow(Shuffled_DFT_im[h*WIDTH + w], 2)) / 1000;
			Shuffled_img[h*WIDTH + w] = result>255 ? 255 : result < 0 ? 0 : result;
		}
	}
	return Shuffled_img;
}

uchar* my_LPF(double* Shuffled_DFT_re, double* Shuffled_DFT_im, double* Filtered_DFT_re, double* Filtered_DFT_im) {
	uchar* Filtered_img = alloc_pic(WIDTH*HEIGHT);

	double result = 0;

	for (int h = 1 * HEIGHT / 4; h < 3 * WIDTH / 4; h++) {
		for (int w = 1 * WIDTH / 4; w < 3 * WIDTH / 4; w++) {
			Filtered_DFT_re[h*WIDTH + w] = Shuffled_DFT_re[h*WIDTH + w];
			Filtered_DFT_im[h*WIDTH + w] = Shuffled_DFT_im[h*WIDTH + w];
		}
	}
	for (int h = 0; h < HEIGHT; h++) {
		for (int w = 0; w < WIDTH; w++) {
			result = sqrt(pow(Filtered_DFT_re[h*WIDTH + w], 2) + pow(Filtered_DFT_im[h*WIDTH + w], 2)) / 1000;
			Filtered_img[h*WIDTH + w] = result>255 ? 255 : result < 0 ? 0 : result;
		}
	}
	return Filtered_img;
}

uchar* my_IDFT(double* Filtered_DFT_re, double* Filtered_DFT_im, double* IDFT_re, double* IDFT_im) {
	uchar* IDFT_img = alloc_pic(WIDTH*HEIGHT);

	double* IDFT_re_ver_img = alloc_pic_double(WIDTH*HEIGHT);
	double* IDFT_im_ver_img = alloc_pic_double(WIDTH*HEIGHT);

	double real, imaginary, sum;

	for (int w = 0; w < WIDTH; w++) {
		for (int h = 0; h < HEIGHT; h++) {

			real = imaginary = 0;

			for (int k = 0; k < HEIGHT; k++) {

				real += Filtered_DFT_re[k*WIDTH + w] * cos((2 * PI*k*h) / HEIGHT)
					- Filtered_DFT_im[k*WIDTH + w] * sin((2 * PI*k*h) / HEIGHT);

				imaginary += Filtered_DFT_re[k*WIDTH + w] * sin((2 * PI*k*h) / HEIGHT)
					+ Filtered_DFT_im[k*WIDTH + w] * cos((2 * PI*k*h) / HEIGHT);
			}
			IDFT_re_ver_img[h*WIDTH + w] = real / HEIGHT;
			IDFT_im_ver_img[h*WIDTH + w] = imaginary / HEIGHT;
		}
	}
	for (int h = 0; h < HEIGHT; h++) {
		for (int w = 0; w < WIDTH; w++) {

			real = imaginary = sum = 0;

			for (int k = 0; k < WIDTH; k++) {

				real += IDFT_re_ver_img[h*WIDTH + k] * cos((2 * PI*k*w) / HEIGHT)
					- IDFT_im_ver_img[h*WIDTH + k] * sin((2 * PI*k*w) / HEIGHT);

				imaginary += IDFT_re_ver_img[h*WIDTH + k] * sin((2 * PI*k*w) / HEIGHT)
					+ IDFT_im_ver_img[h*WIDTH + k] * cos((2 * PI*k*w) / HEIGHT);
			}
			IDFT_re[h*WIDTH + w] = real / WIDTH;
			IDFT_im[h*WIDTH + w] = imaginary / WIDTH;
			sum = sqrt((real / WIDTH)*(real / WIDTH) + (imaginary / WIDTH)*(imaginary / WIDTH));
			IDFT_img[h*WIDTH + w] = sum>255 ? 255 : sum < 0 ? 0 : sum;
		}
	}
	return IDFT_img;
}