#include "Recognition.hpp"
#include <limits>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


FacialData readData(const int nClasses, const int nSamples, bool useTestData, int nIntruders)
{
	FacialData ret;
	auto& [_, X, classes, X_test, classes_test] = ret;
	ret.nClasses = (useTestData ? nClasses - nIntruders : nClasses);

	std::vector<cv::Mat> images;
	std::vector<cv::Mat> images_test;

	cv::Mat img;

	auto readImage = [&](int cls, int sample)
	{
		char fname[128];

		sprintf_s(fname, R"(Images\att\s%d\%d.pgm)", cls, sample);
		//printf("%s\t", fname);

		img = cv::imread(fname, cv::IMREAD_GRAYSCALE);

		assert(img.rows == 112 && img.cols == 92);

		img = img.reshape(1);
		img.convertTo(img, CV_8U);
	};

	for(int i = 1; i <= ret.nClasses; ++i)
	{
		for (int j = 1; j <= nSamples; ++j) 
		{
			readImage(i, j);
			
			images.push_back(img);
			classes.push_back(i - 1);

			/*char debug[128];
			sprintf_s(debug, "raw face class: %d img: %d", i, j);
			printf("%d\n", (i - 1) * nSamples + j - 1);
			cv::imshow(debug, images[(i - 1) * nSamples + j - 1]);*/
		}

		if (useTestData)
		{
			/// randomly picks an image from the train set and moves it to the test set
			int rnd = rand() % nSamples;
			int idx = (i - 1) * (nSamples - 1) + rnd;
			images_test.push_back(images[idx]);
			classes_test.push_back(classes[idx]);
			images.erase(images.begin() + idx);
			classes.erase(classes.begin() + idx);
		}
		//cv::waitKey();
	}
	
	int n = images[0].total();

	/// flattens train images and copies them into the columns of X
	X = cv::Mat(n, images.size(), CV_32F);
	for (int i = 0; i < images.size(); ++i)
		images[i].reshape(1, n).copyTo(X.col(i));

	if (useTestData)
	{
		for (int i = nClasses - nIntruders + 1; i <= nClasses; ++i)
			for (int j = 1; j <= nSamples; j++)
			{
				readImage(i, j);

				images_test.push_back(img);
				classes_test.push_back(i - 1);
			}

		/// flattens test images and copies them into the columns of X
		X_test = cv::Mat(n, images_test.size(), CV_32F);
		for (int i = 0; i < images_test.size(); ++i)
			images_test[i].reshape(1, n).copyTo(X_test.col(i));
	}

	return ret;
}

void computeDistance(const cv::Mat& Y_test, const cv::Mat& Y_train, const int idx, float& retDist, int& retClass)
{
	retDist = std::numeric_limits<float>::max();
	for (int j = 0; j < Y_train.cols; ++j)
	{
		//float cosineSimilarity = Y.col(j).dot(Y_test.col(i)) / (cv::norm(Y.col(j)) * cv::norm(Y_test.col(i)));
		float ab = Y_train.col(j).dot(Y_test.col(idx));
		float aa = Y_train.col(j).dot(Y_train.col(j));
		float bb = Y_test.col(idx).dot(Y_test.col(idx));
		//float dist = cv::norm(Y_test.col(idx) - Y_train.col(j));
		float dist = -ab / sqrt(aa * bb);
		if (/*cosineSimilarity*/ dist < retDist)
		{
			retDist = /*cosineSimilarity*/ dist;
			retClass = j;
		}
	}
}

TransformationData computeTransformation(const FacialData& facialData)
{
	TransformationData ret;
	const auto& [C, X, classes, X_test, classes_test] = facialData;
	auto& [W, Y, Y_test, threshold] = ret;

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

	const auto K = X.cols - C;

	/// the rank of S_w is at most N - C (where N is the total number of train samples and C is the number of classes)
	cv::Mat bestKEivec(X.cols, K, CV_32F);

	/// picks the k greatest eigenvalues and copies their respective eigenvectors into a matrix' columns
	for (int i = 0; i < K; ++i)
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

	/// normalizes the vectors
	for (int i = 0; i < Wpca.cols; ++i)
		Wpca.col(i) /= cv::norm(Wpca.col(i));
	
	/*{
		cv::Mat w, u, vt;
		cv::SVDecomp(Wpca, w, u, vt);
		Wpca = u;
	}*/
#ifndef NDEBUG
	/*
	/// assert Wpca has normalized columns
	for (int i = 0; i < Wpca.cols; ++i)
		assert(cv::norm(Wpca.col(i)) - 1 < 1e-4);

	/// assert Wpca has orthogonal columns
	for (int i = 0; i < Wpca.cols; ++i)
		for (int j = i + 1; j < Wpca.cols; ++j)
			assert(Wpca.col(i).dot(Wpca.col(j)) < 1e-4);
	*/
#endif

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

	/// the input space was already transformed by Wpca
	Y = Wfld.t() * P;

#pragma endregion

	W = Wpca * Wfld;
	Y_test = W.t() * X_test;

#pragma region computeThreshold

	std::vector<float> accDistances;
	accDistances.resize(C);
	std::vector<float> rejDistances;
	rejDistances.resize(Y_test.cols - C);

	float dist;
	int cls;
	for (int i = 0; i < C; ++i)
	{
		computeDistance(Y_test, Y, i, dist, cls);
		accDistances[i] = dist;
	}
	for (int i = C; i < Y_test.cols; ++i)
	{
		computeDistance(Y_test, Y, i, dist, cls);
		rejDistances[i - C] = dist;
	}

	std::stable_sort(accDistances.begin(), accDistances.end());
	std::stable_sort(rejDistances.begin(), rejDistances.end());

	float FAR, FRR;
	float minDist = std::numeric_limits<float>::max();
	int minIdx;
	while (accDistances[C - FAR - 1] > rejDistances[FRR])
	{
		dist = accDistances[C - FAR - 1] - rejDistances[FRR];
		if (abs(minDist) > abs(dist))
		{
			minDist = dist;
			minIdx = FRR;
		}

		FAR++;
		while (FRR / C < FAR / (Y_test.cols - C))
			FRR++;
	}

	threshold = rejDistances[minIdx] - minDist / 2;

	FRR = FRR / C;
	FAR = FAR / (Y_test.cols - C);

#ifndef NDEBUG
	printf("FRR: %.3f\tFAR: %.3f\n", FRR * 100, FAR * 100);
#endif // !NDEBUG

#pragma endregion

	return ret;
}

void draw_faces(const cv::Mat& W)
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

int authenticate(const FacialData& facialData, const TransformationData& transformationData, const cv::Mat& img)
{
	cv::Mat X_img = cv::Mat(img.rows * img.cols, 1, CV_32F);

	img.reshape(1, img.rows * img.cols).copyTo(X_img.col(0));

	cv::Mat Y_img = transformationData.W.t() * X_img;

	float dist;
	int cls;
	computeDistance(Y_img, transformationData.Y, 0, dist, cls);

	return (dist > transformationData.threshold ? -1 : cls);
}

void testRecognition(const FacialData& facialData, const TransformationData& transformationData)
{
	const auto& [nClasses, X, classes, X_test, classes_test] = facialData;
	const auto& [W, Y, Y_test, threshold] = transformationData;

	assert(classes_test.size() == Y_test.cols);

	std::vector<int> closest;
	closest.resize(Y_test.cols);
	std::vector<float> distances;
	distances.resize(Y_test.cols);


	for (int i = 0; i < Y_test.cols; ++i) 
	{
#ifndef NDEBUG
		float minClasa = std::numeric_limits<float>::max();
#endif // !NDEBUG
		float min_dist = std::numeric_limits<float>::max();
		for (int j = 0; j < Y.cols; ++j) 
		{
			float dist = cv::norm(Y_test.col(i) - Y.col(j));
#ifndef NDEBUG
			if (dist < minClasa)
				minClasa = dist;
#endif // !NDEBUG
			if (dist < min_dist) 
			{
				min_dist = dist;
				closest[i] = j;
			}

#ifndef NDEBUG
			//if (j % 9 == 0 && j > 0)
			//{
			//	printf("min pt clasa %d : %lf\n", j / 9, minClasa);
			//	minClasa = std::numeric_limits<float>::max();
			//}
#endif // !NDEBUG
		}
		distances[i] = min_dist;
	}

	int cnt = 0;
	for (int i = 0; i < Y_test.cols; ++i)
	{
#ifndef  NDEBUG
		printf("predicted: %d true: %d min distance: %f\n", classes[closest[i]], classes_test[i], distances[i]);
#endif // ! NDEBUG

		if (distances[i] < threshold && classes[closest[i]] == classes_test[i])
			cnt++;
	}

	printf("Recognition accuracy: %.3f\n", (static_cast<float>(cnt) / nClasses) * 100);
}

//void testAuthentication(const FacialData & facialData, const TransformationData & transformationData)
//{
//	const auto&[nClasses, X, classes, X_test, classes_test] = facialData;
//	const auto&[W, Y, Y_test, threshold] = transformationData;
//
//	assert(classes_test.size() == Y_test.cols);
//
//	float FAR = 0, FRR = 0;
//
//	for (int i = 0; i < Y_test.cols; ++i)
//	{
//		float min = std::numeric_limits<float>::max();
//		for (int j = 0; j < Y.cols; ++j)
//		{
//			float cosineSimilarity = Y.col(j).dot(Y_test.col(i)) / (cv::norm(Y.col(j)) * cv::norm(Y_test.col(i)));
//			if (cosineSimilarity < min)
//				min = cosineSimilarity;
//		}
//
//		printf("%d\t%f\n", i, min);
//
//		if (i < nClasses && min > threshold)
//			FRR++;
//		else if (i > nClasses && min < threshold)
//			FAR++;
//	}
//
//	FRR = FRR / nClasses * 100;
//	FAR = FAR / (Y_test.cols - nClasses) * 100;
//
//	printf("FRR: %f\tFAR: %f", FRR, FAR);
//}

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
