#include <jni.h>
#include <opencv2/core/core.hpp>
#include <vector>
#include "global.h"

using namespace std;
using namespace cv;

int color_clustering(Mat input, vector<Mat> centres)
{
	LOGI("color_clustering");
	int ncentres = centres.size();// , start = 0;
	Mat tofloat;

	vector<int> hist(ncentres);
	double neardist, dist;
	int nearindes = 0;

	LOGI("color_clustering - convertTo");
	input.convertTo(tofloat, CV_32FC1);

	LOGI("color_clustering - norm");

	if(tofloat.empty()){
		LOGI("tofloat empty");
	}else{
		LOGI("tofloat not empty");
	}


	if(centres.empty()){
		LOGI("centres empty");
	}else{
		LOGI("centres not empty");
	}

	LOGI("%d, %d", tofloat.cols, centres[0].cols);

	neardist = norm(tofloat, centres[0], NORM_L2);

	LOGI("color_clustering - for");
	for (int c = 1; c < centres.size(); c++)
	{
		dist = norm(tofloat, centres[c], NORM_L2);
		if (dist < neardist)
		{
				neardist = dist;
				nearindes = c;
		}
	}
	return nearindes;
}
