#include "DFT.h"

void main() {
	double psnr;

	uchar* ori_pic = alloc_pic(WIDTH*HEIGHT);	//이미지를 보여주기 위해 usigned char형으로 저장된 데이터 
	uchar* noised_pic = alloc_pic(WIDTH*HEIGHT);
	uchar* DFT_pic = alloc_pic(WIDTH*HEIGHT);
	uchar* Shuffled_pic = alloc_pic(WIDTH*HEIGHT);
	uchar* LFiltered_pic = alloc_pic(WIDTH*HEIGHT);
	uchar* Low_IDFT_pic = alloc_pic(WIDTH*HEIGHT);

	double* DFT_re_val = alloc_pic_double(WIDTH*HEIGHT);	//실제 데이터를 가지는 동적 메모리 
	double* DFT_im_val = alloc_pic_double(WIDTH*HEIGHT);
	double* Shuffled_DFT_re_val = alloc_pic_double(WIDTH*HEIGHT);
	double* Shuffled_DFT_im_val = alloc_pic_double(WIDTH*HEIGHT);
	double* LFiltered_DFT_re_val = alloc_pic_double(WIDTH*HEIGHT);
	double* LFiltered_DFT_im_val = alloc_pic_double(WIDTH*HEIGHT);
	double* IDFT_re_val = alloc_pic_double(WIDTH*HEIGHT);
	double* IDFT_im_val = alloc_pic_double(WIDTH*HEIGHT);

	ReadFile(ori_pic, "Lena_512.raw", WIDTH*HEIGHT);

	noised_pic = my_Gaussian_Noise(ori_pic, 0);	//가우시안 노이즈 생성
	WriteFile(noised_pic, "Lena_512_noised.raw", WIDTH*HEIGHT);

	DFT_pic = my_DFT(noised_pic, DFT_re_val, DFT_im_val);	//이산 푸리에 변환 시행
	WriteFile(DFT_pic, "Lena_512_DFT.raw", WIDTH*HEIGHT);

	Shuffled_pic = my_Shuffle(DFT_re_val, DFT_im_val, Shuffled_DFT_re_val, Shuffled_DFT_im_val);	//이미지를 대각선 방향으로 셔플
	WriteFile(Shuffled_pic, "Lena_512_shuffled.raw", WIDTH*HEIGHT);

	LFiltered_pic = my_LPF(Shuffled_DFT_re_val, Shuffled_DFT_im_val, LFiltered_DFT_re_val, LFiltered_DFT_im_val);	//저역 통과 필터 추가
	WriteFile(LFiltered_pic, "Lena_512_low_filtered.raw", WIDTH*HEIGHT);

	Low_IDFT_pic = my_IDFT(LFiltered_DFT_re_val, LFiltered_DFT_im_val, IDFT_re_val, IDFT_im_val);	//저역 통과 필터를 거친 후 역 푸리에 변환
	WriteFile(Low_IDFT_pic, "Lena_512_IDFT_low.raw", WIDTH*HEIGHT);

	psnr = PSNR(ori_pic, Low_IDFT_pic);
	printf("Low pass filter PSNR : %g\n", psnr);

	free(ori_pic);
	free(noised_pic);
	free(DFT_pic);
	free(Shuffled_pic);
	free(LFiltered_pic);
	free(Low_IDFT_pic);

	free(DFT_re_val);
	free(DFT_im_val);
	free(Shuffled_DFT_re_val);
	free(Shuffled_DFT_im_val);
	free(LFiltered_DFT_re_val);
	free(LFiltered_DFT_im_val);
	free(IDFT_re_val);
	free(IDFT_im_val);
}