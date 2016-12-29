#include <jni.h>
#include <opencv2/core/core.hpp>
#include <vector>

using namespace std;
using namespace cv;

void compare_inter(vector<Mat> a, vector<Mat> b, vector<Mat>& kernel);

void histogram_intersection(Mat a_hist, vector<Mat> b_hist, Mat& kernel)
{
	int n = b_hist.size(), number = a_hist.cols;
	kernel = Mat::zeros(1, n + 1, CV_32SC1);
	kernel.at<int>(0) = 1;
	for (int j = 0; j < number; j++)
	{
		for (int k = 0; k < n; k++)
		{
			if (a_hist.at<int>(j) == 0)
				continue;
			kernel.at<int>(k + 1) += min(a_hist.at<int>(j), b_hist[k].at<int>(j));
		}
	}
}

void compare_inter(vector<Mat> a, vector<Mat> b, vector<Mat>& kernel)
{
	int i, j, k;
	int m = a.size(), n=b.size(), number=a[0].cols;
	kernel = vector<Mat>(m);
	for (i = 0; i < m; i++)
	{
		kernel[i] = Mat::zeros(1, n + 1, CV_32SC1);
		kernel[i].at<int>(0) = i + 1;
		for (j = 0; j < number; j++)
		{
			for (k = 0; k < n; k++)
			{
				if (a[i].at<int>(j) == 0)
					continue;
				kernel[i].at<int>(k + 1) += min(a[i].at<int>(j), b[k].at<int>(j));
			}
		}
		//cout << hi_kernel[i] << endl;
	}
}
