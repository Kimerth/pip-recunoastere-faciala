#include "Recognition.hpp"
#include <limits>
#include <opencv2/imgcodecs.hpp>
#include <qdiriterator.h>
#include <qregularexpression.h>
#include <opencv2/highgui.hpp>


void readData(const int C, const int nSamples, cv::Mat& X, std::vector<int>& classes, cv::Mat& X_test, std::vector<int>& classes_test)
{
	std::vector<cv::Mat> images;
	std::vector<cv::Mat> images_test;
	classes.empty();
	classes_test.empty();

	for(int i = 1; i <= C; ++i)
	{
		char fname[128];

		for (int j = 1; j <= nSamples; ++j) 
		{
			//TODO: take path as parameter
			sprintf_s(fname, "Images/att/s%d/%d.pgm", i, j);
			//printf("%s\t", fname);

			cv::Mat img = cv::imread(fname, cv::IMREAD_GRAYSCALE);
			
			assert(img.rows == 92 && img.cols == 112);

			img = img.reshape(1);
			img.convertTo(img, CV_8U);
			images.push_back(img);

			classes.push_back(i - 1);

			/*char debug[128];
			sprintf_s(debug, "raw face class: %d img: %d", i, j);
			printf("%d\n", (i - 1) * nSamples + j - 1);
			cv::imshow(debug, images[(i - 1) * nSamples + j - 1]);*/
		}

		//! randomly picks an image from the train set and moves it to the test set
		int rnd = rand() % 10;
		int idx = (i - 1) * (nSamples - 1) + rnd;
		images_test.push_back(images[idx]);
		classes_test.push_back(classes[idx]);
		images.erase(images.begin() + idx);
		classes.erase(classes.begin() + idx);
		//cv::waitKey();
	}
	
	int n = images[0].total();

	//! flattens train images and copies them into the columns of X
	X = cv::Mat(n, images.size(), CV_32F);
	for (int i = 0; i < images.size(); ++i)
		images[i].reshape(1, n).copyTo(X.col(i));

	//! flattens test images and copies them into the columns of X
	X_test = cv::Mat(n, images_test.size(), CV_32F);
	for (int i = 0; i < images_test.size(); ++i)
		images_test[i].reshape(1, n).copyTo(X_test.col(i));
}

void computeTransformation(const int C, const cv::Mat& X, const std::vector<int>& classes, cv::Mat& W, cv::Mat& Y)
{
	//! calculates the class frequency
	std::vector<int> classFreq;
	classFreq.resize(C);
	std::fill(classFreq.begin(), classFreq.end(), 0);

	for (int i = 0; i < X.cols; ++i)
		classFreq[classes[i]]++;

#pragma region PCA

	cv::Mat scatter = X.t() * X;

	cv::Mat eivec;
	cv::Mat eival;

	cv::eigen(scatter, eival, eivec);

	//! the rank of S_w is at most N - C (where N is the total number of train samples and C is the number of classes
	cv::Mat bestKEivec(X.cols, X.cols - C, CV_32F);
	//! picks the k greatest eigenvalues and copies their respective eigenvectors into a matrix' columns
	for (int i = 0; i < X.cols - C; ++i) 
	{
		float max = -std::numeric_limits<float>::max();
		int idx = -1;
		for (int j = 0; j < eival.rows; j++) 
			if (eival.at<float>(j, 0) > max) 
			{
				max = eival.at<float>(j, 0);
				idx = j;
			}
		
		eival.at<float>(idx, 0) = -std::numeric_limits<float>::max();
		eivec.col(idx).copyTo(bestKEivec.col(i));
	}

	cv::Mat Wpca = X * bestKEivec;

	//! normalizes the eigenvectors
	for (int i = 0; i < Wpca.cols; ++i) 
		Wpca.col(i) /= cv::norm(Wpca.col(i));

	cv::Mat P = Wpca.t() * X;

#pragma endregion

	int n = P.rows;

#pragma region FLD

#pragma region perClassMean

	std::vector<cv::Mat> pc;

	cv::Mat means = cv::Mat::zeros(n, C, CV_32F);
	for (int i = 0; i < P.cols; ++i)
		means.col(classes[i]) += P.col(i);
	for (int i = 0; i < C; ++i)
		means.col(i) /= (double)classFreq[i];

	for (int i = 0; i < C; ++i)
		pc.push_back(cv::Mat::zeros(n, classFreq[i], CV_32F));

	std::vector<int> idx;
	idx.resize(C);
	std::fill(idx.begin(), idx.end(), 0);

	for (int i = 0; i < P.cols; ++i)
		P.col(i).copyTo(pc[classes[i]].col(idx[classes[i]]++));

	for (int i = 0; i < C; i++)
		for (int j = 0; j < classFreq[i]; ++j)
			pc[i].col(j) -= means.col(i);

	auto total_mean = cv::mean(P);

#pragma endregion

	//! between-class scatter matrix
	cv::Mat S_b = cv::Mat::zeros(n, n, CV_32F);
	//! within-class scatter matrix
	cv::Mat S_w = cv::Mat::zeros(n, n, CV_32F);

	for (int i = 0; i < C; ++i)
	{
		S_w += pc[i] * pc[i].t();
		S_b += classFreq[i] * (means.col(i) - total_mean) * (means.col(i) - total_mean).t();
	}
	cv::Mat out;
	cv::solve(S_w, S_b, out);

	cv::eigen(/*S_w.inv() * S_b*/ out, eival, eivec);

	//! there is a maximum of C - 1 Fisherfaces
	cv::Mat Wfld = cv::Mat(n, C - 1, CV_32F);
	//! picks the k greatest eigenvalues and copies their respective eigenvectors into a matrix' columns
	for (int i = 0; i < C - 1; ++i)
	{
		float max = -std::numeric_limits<float>::max();
		int idx = -1;

		for(int j = 0; j < eivec.rows; ++j)
			if (eival.at<float>(j, 0) > max)
			{
				max = eival.at<float>(j, 0);
				idx = j;
			}

		eival.at<float>(idx, 0) = -std::numeric_limits<float>::max();
		eivec.col(idx).copyTo(Wfld.col(i));
	}

	//! the input space was already transformed by Wpca
	Y = Wfld.t() * P;

#pragma endregion

	W = Wpca * Wfld;
}

void draw_faces(cv::Mat& W)
{
	for (int i = 0; i < W.cols; ++i)
	{
		cv::Mat aux;
		W.col(i).copyTo(aux);

		double min, max;
		cv::minMaxIdx(aux, &min);

		aux = aux - min;

		cv::minMaxIdx(aux, &min, &max);

		aux = aux * 255.0 / max;

		aux.convertTo(aux, CV_8U);

		aux = aux.reshape(0, 112);

		cv::imshow("test", aux);
		cv::waitKey();
	}
}

void test(const cv::Mat& W, const cv::Mat& Y, const std::vector<int>& classes, const std::vector<int>& classes_test, const cv::Mat& X_test)
{
	cv::Mat Y_test = W.t() * X_test;
	
	std::vector<int> closest;
	closest.resize(Y_test.cols);

	for (int i = 0; i < Y_test.cols; ++i) 
	{
		float min_dist = std::numeric_limits<float>::max();
		for (int j = 0; j < Y.cols; ++j) 
		{
			float dist = cv::norm(Y_test.col(i) - Y.col(j));
			if (dist < min_dist) {
				min_dist = dist;
				closest[i] = j;
			}
		}
	}

	int cnt = 0;
	for (int i = 0; i < Y_test.cols; ++i)
		if (classes[closest[i]] == classes_test[i])
			cnt++;

	printf("%lf", ((double)cnt / Y_test.cols) * 100);
}

//cv::Mat softmax(cv::Mat in)
//{
//	cv::Mat rez;
//	double min, max;
//	cv::minMaxIdx(in, &min, &max);
//	
//	double maxabs = std::max(std::abs(min), std::abs(max));
//
//	cv::exp(in / maxabs, rez);
//
//	double sum = *cv::sum(rez).val;
//
//	rez.mul (1 / sum);
//
//	return rez;
//}
