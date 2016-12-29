#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>
#include "math.h"
#include "global.h"
#include <string>
#include "time.h"

using namespace std;
using namespace cv;




#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobjectArray JNICALL Java_org_opencv_samples_tutorial2_CircleDetect_HoughCircle(JNIEnv* env, jobject obj, jlong addrRgba);
JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_CircleDetect_DrawCircle(JNIEnv* env, jobject obj, jlong addrRgba, jobjectArray arr_circle);
int coin_detect(Mat src);
int range_check(int value, int min, int max);

/* time */
clock_t start_time, end_time;
float hough_time, feature_time, predict_time;

	JNIEXPORT jobjectArray JNICALL Java_org_opencv_samples_tutorial2_CircleDetect_HoughCircle(JNIEnv *env, jobject obj, jlong addrRgba){


		Mat& mRgb = *(Mat*)addrRgba;
		Mat gray, smooth, median, out_rect;
		vector<Vec3f> circles;
		int x_left_top, y_left_top, diameter, x_right_btm, y_right_btm, coin_idx = -1;
		float x, y, z;

		cvtColor(mRgb, gray, CV_BGR2GRAY);
		GaussianBlur(gray, smooth, Size(3, 3), 5, 3);
		medianBlur(smooth, median, 9);

		start_time = clock();
		HoughCircles(median, circles, CV_HOUGH_GRADIENT, 1, median.rows/4, 50, 75, 50, 400);//50 100
		end_time = clock();
		hough_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;
		//median, circles, CV_HOUGH_GRADIENT, 2, median.rows, 20, 50, 50, 400


		/* circle_info obj */
		jclass java_circle_cls = env->FindClass("org/opencv/samples/tutorial2/CircleInfo");
		jmethodID java_circle_set = env->GetMethodID(java_circle_cls, "set", "(FFFIIIII)V");
		jmethodID java_circle_init = env->GetMethodID(java_circle_cls, "<init>", "()V");

		/* create return arr */
		jobjectArray arrObj = (jobjectArray) env->NewObjectArray((int)circles.size(), java_circle_cls, NULL);

		for(int i = 0; i < circles.size(); ++i){

			x = circles[i][0];
			y = circles[i][1];
			z = circles[i][2];

			/* coin detect */
			x_left_top = cvRound(x) - cvRound(z);
			y_left_top = cvRound(y) - cvRound(z);
			diameter = cvRound(z) * 2 ;
			x_right_btm = x_left_top + diameter;
			y_right_btm = y_left_top + diameter;

			/* rect range check */
			x_right_btm = range_check(x_right_btm, 0, mRgb.cols);
			x_left_top = range_check(x_left_top, 0, mRgb.cols);
			y_right_btm = range_check(y_right_btm, 0, mRgb.rows);
			y_left_top = range_check(y_left_top, 0, mRgb.rows);

			Mat tmp_rect = mRgb.colRange(x_left_top, x_right_btm).rowRange(y_left_top, y_right_btm);
			resize(tmp_rect, out_rect, Size(200, 200));
			coin_idx = coin_detect(out_rect);


			/* cur_obj = current object */
		    jobject cur_obj =  env->NewObject(java_circle_cls, java_circle_init);

		    env->CallVoidMethod(cur_obj, java_circle_set,
		    		            x,
		                        y,
		                        z,
		                        coin_idx,
		                        x_left_top,
		                        y_left_top,
		                        x_right_btm,
		                        y_right_btm);
		    /* add to arr */
		    env->SetObjectArrayElement(arrObj, i, cur_obj);
		}
		return arrObj;
	}

	JNIEXPORT void JNICALL Java_org_opencv_samples_tutorial2_Tutorial2Activity_DrawCircle(JNIEnv* env, jobject obj, jlong addrRgba, jobjectArray arr_circle){
		Mat& mRgb = *(Mat*)addrRgba;
		float x, y, z;
		int coin_idx, x_left_top, y_left_top, x_right_btm, y_right_btm;
		string result_str[]={"EUR10C","EUR1C","EUR1","EUR20C","EUR2C","EUR2","EUR50C","EUR5C","NTD10","NTD1","NTD50","NTD5","USD10C","USD1C","USD5C","USD25C"};
		string time_str;

		/* set arr_circle data structure */
		jclass java_circle_cls = env->FindClass("org/opencv/samples/tutorial2/CircleInfo");
		jfieldID fidX = env->GetFieldID(java_circle_cls, "x", "F");
		jfieldID fidY = env->GetFieldID(java_circle_cls, "y", "F");
		jfieldID fidZ = env->GetFieldID(java_circle_cls, "z", "F");
		jfieldID fidCoin = env->GetFieldID(java_circle_cls, "coin_idx", "I");
		jfieldID fidXLeftTop = env->GetFieldID(java_circle_cls, "x_left_top", "I");
		jfieldID fidYLeftTop = env->GetFieldID(java_circle_cls, "y_left_top", "I");
		jfieldID fidXRightBtm = env->GetFieldID(java_circle_cls, "x_right_btm", "I");
		jfieldID fidYRightBtm = env->GetFieldID(java_circle_cls, "y_right_btm", "I");


		int arrCount = env->GetArrayLength(arr_circle);

		for (int i = 0; i < arrCount; i++){

			/* get arr_circle info */
			jobject cur_obj = env->GetObjectArrayElement(arr_circle, i);

			x = env->GetFloatField(cur_obj, fidX);
			y = env->GetFloatField(cur_obj, fidY);
			z = env->GetFloatField(cur_obj, fidZ);
			coin_idx = env->GetIntField(cur_obj, fidCoin);
			x_left_top = env->GetIntField(cur_obj, fidXLeftTop);
			y_left_top = env->GetIntField(cur_obj, fidYLeftTop);
			x_right_btm = env->GetIntField(cur_obj, fidXRightBtm);
			y_right_btm = env->GetIntField(cur_obj, fidYRightBtm);



			/* draw circle */
			Point center(cvRound(x), cvRound(y));
			int radius = cvRound(z);

			// circle center
			circle(mRgb, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			circle(mRgb, center, radius, Scalar(0, 0, 255), 3, 8, 0);

			// result text
			// fontFace, Scale, Color, Thickness, LineType
			int txt_x = (x - (radius/2) < 0) ? 0:(x - (radius/2));
			putText(mRgb, result_str[(coin_idx-1)], Point(txt_x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 3, 8, false);

			/* time text */
			time_str.clear();
			time_str.append("H:");time_str.append(to_string(hough_time));
			time_str.append(", F:");time_str.append(to_string(feature_time));
			time_str.append(", P:");time_str.append(to_string(feature_time));

			putText(mRgb, time_str, Point(10, 25), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2, 8, false);

			/* check out_rect range */
			//rectangle(mRgb, Point(x_left_top, y_left_top), Point(x_right_btm, y_right_btm), (255, 255,0), 2);
		}
	}


	int coin_detect(Mat src){
		string cc_str = "/storage/sdcard0/coin_info/colorcentres.yaml", cc_key="colorcentres";
		string dict_str = "/storage/sdcard0/coin_info/dictionary/dictionary", dict_key = "dictionary";
		string texton_str = "/storage/sdcard0/coin_info/texton/texton", texton_key = "texton";
		string model_str = "/storage/sdcard0/coin_info/model/model";
		string key,save_str;
		vector<Mat> feat_ring, feat;
		vector<Mat> color_centres, dictionary, traintexton;


		Mat color_feature, hik_test, texton;
		int colorgroup;

		//src	= imread(img_filename, 1);

		//feature extraction
		LOGI("coin_detect - feature extraction");
		start_time = clock();
		color_feature = color_feat(src);//*
		feat = texture_feat(src);//*
		end_time = clock();
		feature_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;

		//color assignment
		LOGI("coin_detect - color assignment");
		start_time = clock();
		color_centres = readmat(cc_str, cc_key);//*
		colorgroup = color_clustering(color_feature, color_centres);//*

		//dictionary assignment
		LOGI("coin_detect - dictionary assignment");
		save_str = dict_str;
		save_str.append(to_string(colorgroup));
		save_str.append(".yaml");
		dictionary = readmat(save_str, dict_key);
		assignmeant(feat, dictionary, texton);//*

		//histogram generation
		LOGI("coin_detect - histogram generation");
		save_str = texton_str;
		save_str.append(to_string(colorgroup));
		save_str.append(".yaml");
		traintexton = readmat(save_str, texton_key);
		histogram_intersection(texton, traintexton, hik_test);//*

		//svm prediction
		LOGI("coin_detect - svm prediction");
		save_str = model_str;
		save_str.append(to_string(colorgroup));
		int result = svm_predict(hik_test, save_str);//*
		end_time = clock();

		predict_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;

		LOGI("coin_detect - return result");
		return result;
	}
	int range_check(int value, int min, int max){
		if(value < min){
			value = min;
		}else if(value > max){
			value = max;
		}
		return value;
	}

#ifdef __cplusplus
}
#endif
