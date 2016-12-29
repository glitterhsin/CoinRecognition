#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>


using namespace std;
using namespace cv;


void assignmeant(vector<Mat> input, vector<Mat> centerin, Mat& bow_hist)
{
	//cout << "assignmeant" << endl;
	int img_cnt = 0;
	Mat tofloat;
	bow_hist= Mat::zeros(1, centerin.size(), CV_32SC1);
	for (int i = 0; i < input.size(); i++)
	{
		double neardist, dist;
		int nearindes = 0;
		input[i].convertTo(tofloat, CV_32FC1);
		neardist = norm(tofloat, centerin[0], NORM_L2);
		for (int c = 0; c < centerin.size(); c++)
		{
			dist = norm(tofloat, centerin[c], NORM_L2);
			if (dist < neardist)
			{
				neardist = dist;
				nearindes = c;
			}
		}
		bow_hist.at<int>(nearindes) += 1;
	}
}
