#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "global.h"

using namespace std;
using namespace cv;


void gen_lbp(const Mat src, Mat& lbpri, int radius, int neighbors)
{
	lbpri = Mat::zeros(src.rows - 2 * radius, src.cols - 2 * radius, CV_32SC1);
	neighbors = max(min(neighbors, 31), 1);
	//dst = Mat::zeros(src.rows - 2 * radius, src.cols - 2 * radius, CV_32SC1);
	//cout << src.type();
	for (int n = 0; n<neighbors; n++) {
		// sample points
		float x = static_cast<float>(radius)* cos(2.0*M_PI*n / static_cast<float>(neighbors));
		float y = static_cast<float>(radius)* -sin(2.0*M_PI*n / static_cast<float>(neighbors));
		// relative indices
		int fx = static_cast<int>(floor(x));
		int fy = static_cast<int>(floor(y));
		int cx = static_cast<int>(ceil(x));
		int cy = static_cast<int>(ceil(y));
		// fractional part
		float ty = y - fy;
		float tx = x - fx;
		// set interpolation weights
		float w1 = (1 - tx) * (1 - ty);
		float w2 = tx  * (1 - ty);
		float w3 = (1 - tx) *      ty;
		float w4 = tx  *      ty;
		// iterate through your data
		for (int i = radius; i < src.rows - radius; i++) {
			for (int j = radius; j < src.cols - radius; j++) {
				if (intercircle(j, i, src.cols / 2, src.rows / 2) < 0)
					continue;

				float t = w1*src.at<uchar>(i + fy, j + fx) +
					w2*src.at<uchar>(i + fy, j + cx) +
					w3*src.at<uchar>(i + cy, j + fx) +
					w4*src.at<uchar>(i + cy, j + cx);
				// we are dealing with floating point precision, so add some little tolerance
				unsigned int s_gp = ((t > src.at<uchar>(i, j)) && (abs(t - src.at<uchar>(i, j)) > std::numeric_limits<float>::epsilon()));
				//dst.at<unsigned int>(i - radius, j - radius) += s_gp << n;
				lbpri.at<unsigned int>(i - radius, j - radius) += s_gp;
			}
		}
	}
}
void gen_lpcm(Mat src, vector<Mat>& lpcm, int neighbors)
{
	lpcm = vector<Mat>(section);//Concentruc*Quadrant KRing
	int j_start, y_offset;
	int x_offset[4] = { 1, 0, 1, -1 };
	int src_row, src_col, stair_s;
	int glcm_size = neighbors + 1;
	//hoz:x+1,y  ver:x,y+1  r_slop:x+1,y+1  l_slop:x-1,y+1int
	for (int n = 0; n < 4; n++)
	{
		y_offset = n > 0 ? 1 : 0;
		j_start = n < 3 ? 0 : 1;
		for (int i = 0; i < src.rows - y_offset; i++)
		{
			for (int j = j_start; j < (src.cols - x_offset[n] - j_start); j++)
			{
				int index, quaIndex, largeCir;
				largeCir = Concentric_index(j, i, src.cols / 2, src.rows / 2, quaIndex);
				if (largeCir < 0)continue;
				index = largeCir*Quadrant + quaIndex;

				if (lpcm[index].empty())
					lpcm[index] = Mat::zeros(1, lpcm_his, CV_32SC1);
				//---------
				src_row = src.at<int>(i, j);
				src_col = src.at<int>(i + y_offset, j + x_offset[n]);

				stair_s = src_row*(2 * glcm_size - (src_row - 1)) / 2 + (src_col - src_row);
				if (src_col == src_row)
					lpcm[index].at<int>(stair_s) += 2;
				else if (src_col < src_row)
				{
					stair_s = src_col*(2 * glcm_size - (src_col - 1)) / 2 + (src_row - src_col);
					lpcm[index].at<int>(stair_s) += 1;
				}
				else
					lpcm[index].at<int>(stair_s) += 1;
			}
		}
	}
}
int Concentric_index(int x, int y, double x0, double y0, int& quadarea)//quadrant largeCir
{
	//Concentric
	int ringI;
	double shiftX, shiftY, distance, Radius = x0;
	shiftY = (double)y - (double)y0;
	shiftX = (double)x - (double)x0;
	distance = sqrt(pow(shiftY, 2) + pow(shiftX, 2));
	if (distance > Radius)
		return -1;
	ringI = ceil(distance / (Radius / Concentruc));
	if (ringI > 0)ringI = ringI - 1;
	if (Quadrant > 1)
	{
		double sin_ang = 360.0 / Quadrant;
		double angle = atan2(shiftY, shiftX);
		if (angle < 0)
			angle = angle + 2*M_PI;
		quadarea = floor(angle * 180 / M_PI / sin_ang);
	}
	else
		quadarea = 0;
	return ringI;
}

void kmeans_clustering(vector<Mat> input, vector<Mat> centerin, vector<Mat>& centerout, bool Ishist)
{
	vector<Mat> feat_sum(centerin.size());//save the sum of features in the same group
	centerout = vector<Mat>(centerin.size());
	vector<int> nfeats(centerin.size(),0);
	for (int i = 0; i < input.size(); i++)
	{
		Mat tofloat;
		double neardist,dist;
		int nearindes = 0;
		input[i].convertTo(tofloat, CV_32FC1);
		if (Ishist)
			neardist = chi(tofloat, centerin[0]);
		else{
			if (centerin[0].empty())
				neardist = 0;
			else
				neardist = norm(tofloat, centerin[0], NORM_L2);
		}
		for (int c = 1; c < centerin.size(); c++)
		{
			if (Ishist)
				dist = chi(tofloat, centerin[c]);
			else
			{
				if (centerin[c].empty())
					dist = 0;
				else
					dist = norm(tofloat, centerin[c], NORM_L2);
			}


			if (dist < neardist)
			{
				neardist = dist;
				nearindes = c;
			}
		}
		nfeats[nearindes]++;
		if (feat_sum[nearindes].empty())
			feat_sum[nearindes] = Mat::zeros(1, input[i].cols, CV_32FC1);

		feat_sum[nearindes] += tofloat;
	}
	for (int c = 0; c < centerin.size(); c++)
	{
		centerout[c] = feat_sum[c] / nfeats[c];
	}

}
int region_index(int x, int y, int x0, int y0, int nblock)
{
	//Ring structure
	int ringI;
	double shiftX, shiftY, distance, Radius = x0;
	shiftY = y - (double)y0;
	shiftX = x - (double)x0;
	distance = sqrt(pow(shiftY, 2) + pow(shiftX, 2));
	if (distance > Radius)
		return -1;
	ringI = ceil(distance / (Radius / nblock));
	if (ringI > 0)ringI = ringI - 1;
	return ringI;
}

int intercircle(int x, int y, int x0, int y0)
{
	//Ring structure
	int ringI;
	double shiftX, shiftY, distance, Radius = x0;
	shiftY = y - (double)y0;
	shiftX = x - (double)x0;
	distance = sqrt(pow(shiftY, 2) + pow(shiftX, 2));
	if (distance > Radius)
		return -1;
	return 1;
}
double chi(Mat hist1, Mat hist2)
{
	double dist = 0.0;
	if (hist1.empty() || hist2.empty())
		return 0.0;
	dist = compareHist(hist1, hist2, CV_COMP_CHISQR);//#define CV_COMP_CHISQR 1
	return dist;
}

