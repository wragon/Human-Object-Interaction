#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <io.h>

typedef struct {
	int found;
	float x, y;
	int ind;
} keypoint;

float cross(keypoint a, keypoint b);
void loadData(const string& file_name, cv::Ptr<cv::ml::TrainData>* data);

int main(int argc, char* argv[])
{
	//argv[1]="trainList.txt";
	//argv[1] =testList.txt";
	//argv[2]="train";

	//// --TRAIN-- //	 
	if (argc == 3) {
		FILE* fp_train = fopen(argv[1], "r");
		int nsample, ndim;
		fscanf(fp_train, "%d %d\n", &nsample, &ndim);
		printf("%d %d\n", nsample, ndim);

		Mat samples(nsample, ndim, CV_32F);
		Mat responses(nsample, 1, CV_32S);
		for (int k = 0; k < nsample; k++) {
			int lab;
			char filename[256];
			char dummy[5];
			fscanf(fp_train, "%s\n", &filename);
			FILE* fp_trainData = fopen(filename, "r");
			fscanf(fp_trainData, "%d%c", &lab, dummy);
			responses.at<int>(k, 0) = lab;
			for (int i = 0; i < 105; i++) {
				float val;
				fscanf(fp_trainData, "%f%c", &val, dummy);
				samples.at<float>(k, i) = val;
			}
			fclose(fp_trainData);
		}
		fclose(fp_train);
		Ptr<ml::TrainData> train_set = ml::TrainData::create(samples, 0, responses);
		int n_train_samples = train_set->getNSamples();
		printf("# of train samples: %d\n", n_train_samples);

		int layout = 0;
		Mat feature = train_set->getSamples();
		Mat label = train_set->getResponses();
		Ptr<ml::TrainData> TrainSamples = ml::TrainData::create(feature, layout, label);
		Ptr<ml::RTrees> dtree = ml::RTrees::create();

		float _priors[] = { 1.0, 1.0 };  // weight 
		Mat priors(1, 2, CV_32F, _priors);
		dtree->setMaxDepth(8);
		dtree->setMinSampleCount(10);
		dtree->setRegressionAccuracy(0.01f);
		dtree->setUseSurrogates(false /* true */);
		dtree->setMaxCategories(20);
		dtree->setCVFolds(0/*10*/); // nonzero causes core dump
		dtree->setUse1SERule(true);
		dtree->setTruncatePrunedTree(true);
		// dtree->setPriors( priors );
		dtree->setPriors(cv::Mat());
		dtree->train(TrainSamples);
		dtree->save("cfg/rf/train.xml");
	}
	else {
		//// --TEST-- //
		FILE* fp_test = fopen(argv[1], "r");
		int nsample, ndim;
		fscanf(fp_test, "%d %d\n", &nsample, &ndim);
		printf("%d %d\n", nsample, ndim);

		Mat samples(nsample, ndim, CV_32F);
		Mat responses(nsample, 1, CV_32S);
		for (int k = 0; k < nsample; k++) {
			int lab;
			char filename[256];
			char dummy[5];
			fscanf(fp_test, "%s\n", &filename);
			FILE* fp_testData = fopen(filename, "r");
			fscanf(fp_testData, "%d%c", &lab, dummy);
			responses.at<int>(k, 0) = lab;
			for (int i = 0; i < 105; i++) {
				float val;
				fscanf(fp_testData, "%f%c", &val, dummy);
				samples.at<float>(k, i) = val;
			}
			fclose(fp_testData);
		}
		fclose(fp_test);

		Ptr<ml::TrainData> test_set = ml::TrainData::create(samples, 0, responses);
		int n_test_samples = test_set->getNSamples();
		const Mat test_sample = test_set->getSamples();
		Mat lab = test_set->getResponses();

		Mat results;
		Ptr<ml::RTrees> rtree = ml::RTrees::load("cfg/rf/train.xml");
		float performance = rtree->predict(test_sample, results);

		FILE* fp = fopen("out/rf.txt", "w");
		int sum = 0;
		for (int i = 0; i < n_test_samples; i++) {
			int gt = lab.at<int>(i, 0);
			int prediction = int(results.at<float>(i, 0));
			sum += (abs(gt - prediction) > 0);
			printf("[%4d] gt:%2d, pr:%2d\n", i, gt, prediction);
			fprintf(fp, "%d %d %d\n", i, gt, prediction);
		}
		printf("#err: %d, #total: %d\n", sum, n_test_samples);
		fclose(fp);
	}
	//system("pause");
	return 0;
}

float cross(keypoint a, keypoint b) {

	float res = a.x * b.y - b.x * a.y;
	return res;
}

void loadData(const string& file_name, cv::Ptr<cv::ml::TrainData >* data) {

	ifstream outf(file_name);
	string line;

	int n_sample;		// data sample °¹¼ö
	int n_dim;			// feature °¹¼ö
	int i = 0, d = -1;

	outf >> n_sample >> n_dim;
	cv::Mat samples(n_sample, n_dim, CV_32F);
	cv::Mat responses(n_sample, 1, CV_32S);

	while (outf.is_open()) {
		outf >> line;
		std::stringstream str_strm(line);
		std::string tmp;
		vector<string> words;
		char split = ',';

		while (std::getline(str_strm, tmp, split)) {
			words.push_back(tmp);
		}

		string str;
		for (int k = 0; k < words.size(); k++) {
			str.append(words.at(k));
			if (!k) responses.at<int>(i, 0) = stoi(str);
			else samples.at<float>(i, d) = stof(str);
			str.clear();
			d++;
			d %= (n_dim + 1);
		}
		i++;
		if (i == n_sample) outf.close();
	}

	*data = cv::ml::TrainData::create(samples, 0, responses);
}