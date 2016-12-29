//global variance
#include <jni.h>
#include <opencv2/core/core.hpp>
#include <vector>
#include <string>
#include <android/log.h>

#define  LOG_TAG    "CoinDetect"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

#define light_th 160
#define Region 64
#define ColorRing 8
#define lbp_his (8+1)
#define lpcm_his (((8+1)*(8+2))/2)
#define feat_len (lbp_his + lpcm_his)
#define Concentruc 8
#define Quadrant 8
#define section (Quadrant*Concentruc)
#define Carea (Quadrant*Concentruc)

cv::Mat color_feat(cv::Mat src);
std::vector<cv::Mat> texture_feat(cv::Mat src);

Mat color_feat(Mat src);
void gen_lbp(const Mat src, Mat& lbpri, int radius, int neighbors);
void gen_lpcm(Mat src, vector<Mat>& lpcm, int neighbors);
int Concentric_index(int x, int y, double x0, double y0, int& quadarea);
void histogram_intersection(Mat a_hist, vector<Mat> b_hist, Mat& hi_kernel);
void compare_inter(vector<Mat> a, vector<Mat> b, vector<Mat>& kernel);
vector<Mat> readmat(string filename, string key);
void assignmeant(vector<Mat> input, vector<Mat> centerin, Mat& bow_hist);
void kmeans_clustering(vector<Mat> input, vector<Mat> centerin, vector<Mat>& centerout, bool Ishist);
int intercircle(int x, int y, int x0, int y0);
int region_index(int x, int y, int x0, int y0, int nblock);
double chi(Mat hist1, Mat hist2);
int svm_predict(Mat input, string model_str);
int color_clustering(Mat input, vector<Mat> centres);

template <typename T>
std::string to_string(T value)
{
	std::ostringstream os ;
	os << value ;
	return os.str() ;
}

