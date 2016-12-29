#include <jni.h>
//#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include "svm.h"
//#include "bow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"

using namespace std;
using namespace cv;

struct svm_node *x;
int max_nr_attr = 64;
int predict_probability = 1;
struct svm_model* model_1;
int predict_nofile(Mat test_hist);
int svm_predict(Mat input, string model_str)
{
	int predict;
	char *model_file_name = strcpy((char*)malloc(model_str.length() + 1), model_str.c_str());

	if ((model_1 = svm_load_model(model_file_name)) == 0)
	{
		cout<<"can't open model file "<<endl;
	}

	x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
	if (predict_probability)
	{
		if (svm_check_probability_model(model_1) == 0)
		{
			cout<<"Model does not support probabiliy estimates"<<endl;
		}
	}
	else
	{
		if (svm_check_probability_model(model_1) != 0){
			cout<<"Model supports probability estimates, but disabled in prediction."<<endl;
		}
	}
	predict = predict_nofile(input);
	svm_free_and_destroy_model(&model_1);
	free(x);

	return predict;
}
int predict_nofile(Mat test_hist)
{
	int svm_type = svm_get_svm_type(model_1);
	int nr_class = svm_get_nr_class(model_1);
	double *prob_estimates = NULL;
	int j;
	int test_predict_label;

	if (predict_probability)
	{
		if (svm_type == NU_SVR || svm_type == EPSILON_SVR){
			cout << "Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma= " << svm_get_svr_probability(model_1) << endl;
		}else{
			int *labels = (int *)malloc(nr_class*sizeof(int));
			svm_get_labels(model_1, labels);
			prob_estimates = (double *)malloc(nr_class*sizeof(double));
			free(labels);
		}
	}
	int h;
	int i = 0;
	double predict_label; //target_label
	int inst_max_index = -1;
	for (h = 0; h < test_hist.cols; h++)
	{
		if (i >= max_nr_attr - 1)	// need one more for index = -1
		{
			max_nr_attr *= 2;
			x = (struct svm_node *) realloc(x, max_nr_attr*sizeof(struct svm_node));
		}
		x[i].index = h;
		if (x[i].index <= inst_max_index){
			cout << "Wrong input format" << endl;
		}else{
			inst_max_index = x[i].index;
		}
		x[i].value = (double)test_hist.at<int>(h);
		++i;
	}
	x[i].index = -1;
	if (predict_probability && (svm_type == C_SVC || svm_type == NU_SVC))
	{
		predict_label = svm_predict_probability(model_1, x, prob_estimates);
		test_predict_label = predict_label;
	}
	else
	{
		predict_label = svm_predict(model_1, x);
		test_predict_label = predict_label;
	}

	if (predict_probability){
		free(prob_estimates);
	}
	return test_predict_label;
}


