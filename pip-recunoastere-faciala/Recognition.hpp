#include <opencv2/core.hpp>
#include <vector>
#include "Image.hpp"

/*!
	\brief Loads train data and test data from disk.

	\param C				Number of classes.
	\param nSample			Number of samples per class.
	\param X				Output train matrix. All images from the train dataset are flattened and copied into its columns.
	\param classes			Output train classes. Contains the respective class for each column in the train matrix.
	\param X_test			Output test matrix. All images from the test dataset are flattened and copied into its columns.
	\param classes_test		Output test classes. Contains the respective class for each column in the test matrix.
*/
void readData(int C, int nSamples, cv::Mat& X, std::vector<int>& classes, cv::Mat& X_test, std::vector<int>& classes_test);

/*!
	\brief Computes the matrix used to represent the input data by finding a subspace which represents most of the data variance
	
	https://cseweb.ucsd.edu/classes/wi14/cse152-a/fisherface-pami97.pdf

	\param C				Number of classes.
	\param X				Input train matrix.
	\param classes			Input train classes.
	\param W				Output transformation matrix.
	\param Y				Output transformed space.

	\see readData(int C, int nSamples, cv::Mat& X, std::vector<int>& classes, cv::Mat& X_test, std::vector<int>& classes_test)
*/
void computeTransformation(const int C, const cv::Mat& X, const std::vector<int>& classes, cv::Mat& W, cv::Mat& Y);

/*!
	\brief Utility to visualize the transformation matrix

	\param W				Input transformation matrix.

	\see computeTransformation(const int C, const cv::Mat& X, const std::vector<int>& classes, cv::Mat& W, cv::Mat& Y)
*/
void draw_faces(cv::Mat& W);

/*!
	\brief Utility to test the accuracy of this algorithm on the previously given dataset

	Will apply the transformation matrix onto the test matrix and then for each image vector it will find the closest train image vector and check if it corresponds to the actual train class.
	Prints the resulting accuracy in console.

	\param W				Input transformation matrix.
	\param classes			Input train classes.
	\param classes_test		Input test classes.
	\param X_test			Input test matrix.
*/
void test(const cv::Mat& W, const cv::Mat& Y, const std::vector<int>& classes, const std::vector<int>& classes_test, const cv::Mat& X_test);

//cv::Mat softmax(cv::Mat in);
