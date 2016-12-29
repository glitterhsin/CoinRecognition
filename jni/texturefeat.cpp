#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "global.h"

using namespace std;
using namespace cv;

vector<Mat> texture_feat(Mat src)
{
	LOGI("texture_feat");
	Mat gray, smooth, median, lbp;
	vector<Mat> lpcm_vec;
	vector<Mat> hist(Region);//Concentruc*Quadrant KRing
	int radius = 1, neighbors = 8;
	int k_smooth, k_med;

	//preprocessing
	cvtColor(src, gray, CV_BGR2GRAY);
	//if (mean(gray)[0] > light_th)
	//{
	//	k_smooth = 11;
	//	k_med = 9;
	//}
	//else
	//{
	//	k_smooth = 5;
	//	k_med = 5;
	//}
	//GaussianBlur(gray, smooth, Size(k_smooth, k_smooth), 5, 3, BORDER_CONSTANT);
	//medianBlur(smooth, median, k_med);

	//local binary pattern
	//gen_lbp(median, lbp, radius, neighbors);
	gen_lbp(gray, lbp, radius, neighbors);
	gen_lpcm(lbp, lpcm_vec, neighbors);

	int index;
	for (int i = 0; i < lbp.rows; i++)
	{
		for (int j = 0; j < lbp.cols; j++)
		{
			//Concentruc
			int index,quaIndex, largeCir;
			largeCir = Concentric_index(j, i, (lbp.cols / 2.0), (lbp.rows / 2.0), quaIndex);
			if (largeCir < 0)continue;
			index = largeCir*Quadrant + quaIndex;

			if (hist[index].empty())
				hist[index] = Mat::zeros(1, feat_len, CV_32SC1);
			//---------
			//count lbp histogram
			int val = lbp.at<int>(i, j);
			hist[index].at<int>(val) += 1;
		}
	}
	for (int k = 0; k < Region; k++)
	{
		for (int l = 0; l < lpcm_his; l++)
		{
			hist[k].at<int>(l + lbp_his) = lpcm_vec[k].at<int>(l);
		}
	}
	return hist;

}
