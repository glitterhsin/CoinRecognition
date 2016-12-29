#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "global.h"

using namespace std;
using namespace cv;

Mat color_feat(Mat src)
{
	LOGI("color_feat");
	Mat color2img, temp,temp2;
	vector<Mat> planes, ringHpixel(ColorRing), ringSpixel(ColorRing);

	cvtColor(src, color2img, CV_BGR2Lab);//CV_BGR2HSV
	split(color2img, planes);
	temp = planes[1];
	temp2 = planes[2];
	for (int i = 0; i < temp.rows; i++)
	{
		for (int j = 0; j < temp.cols; j++)
		{
			int index = region_index(j, i, temp.cols / 2, temp.rows / 2, ColorRing);
			if (index < 0)continue;
			ringHpixel[index].push_back(temp.at<uchar>(i, j));
			ringSpixel[index].push_back(temp2.at<uchar>(i, j));
		}
	}
	Mat meanstd = Mat::zeros(1,ColorRing*4,CV_32FC1);
	for (int k = 0; k < ColorRing; k++)
	{
		Scalar meanimg, stdimg;
		meanStdDev(ringHpixel[k], meanimg, stdimg);
		int locs = 4 * k;
		meanstd.at<float>(locs) = (float)meanimg.val[0];
		meanstd.at<float>(locs + 1) = (float)stdimg.val[0];
		Scalar mean2, std2;
		meanStdDev(ringSpixel[k], mean2, std2);
		meanstd.at<float>(locs + 2) = (float)mean2.val[0];
		meanstd.at<float>(locs + 3) = (float)std2.val[0];
	}
	return meanstd;
}
