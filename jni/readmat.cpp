#include <jni.h>
#include <opencv2/core/core.hpp>
#include <vector>
#include <string>
#include <sstream>
#include "global.h"

using namespace std;
using namespace cv;

vector<Mat> readmat(string filename, string key)
{
	LOGI("readmat");
	vector<Mat> mat;
	int Iimg = 0;
	string key_str;
	FileStorage fs;
	fs.open(filename, FileStorage::READ);
	key_str = key;
	key_str.append(to_string(Iimg));
	while (!fs[key_str].empty())
	{
		Mat temp;
		fs[key_str] >> temp;
		mat.push_back(temp);
		Iimg++;
		key_str = key;
		key_str.append(to_string(Iimg));
	}
	fs.release();

	return mat;
}

