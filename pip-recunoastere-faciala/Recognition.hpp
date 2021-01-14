#include <opencv2/core.hpp>
#include <vector>
#include "Image.hpp"

/*
	\brief Training and testing data
*/
struct FacialData
{
	/// number of classes
	int nClasses;
	/// train matrix - All images from the train dataset are flattened and copied into its columns.
	cv::Mat X;
	/// train classes - Contains the respective class for each column in the train matrix.
	std::vector<int> classes;
	/// test matrix - All images from the test dataset are flattened and copied into its columns.
	cv::Mat X_test;
	/// test classes - Contains the respective class for each column in the test matrix.
	std::vector<int> classes_test;
};

/*
	\brief Output space
*/
struct TransformationData
{
	/// Transformation matrix
	cv::Mat W;
	/// Transformed space
	cv::Mat Y;
	/// Transformed test space
	cv::Mat Y_test;
	/// Acceptance threshold
	float threshold;
};

/*
	\brief Loads train data and test data from disk.

	\returns				Facial data read from disk

	\see struct FacialData
*/
FacialData readData();

/*
	\brief Computes the matrix used to represent the input data by finding a subspace which represents most of the data variance
	
	https://cseweb.ucsd.edu/classes/wi14/cse152-a/fisherface-pami97.pdf

	Computes the reduced dimensionality matrices for train and test data

	Calculates the threshold so that FRR = FAR, where FRR is False Rejection Rate and FAR is False Acceptance Rate

	\param facialData		Input facial data to compute transformation (it should contain images in test data)
	\returns				Output transformation data

	\see FacialData
	\see TransformationData
*/
TransformationData computeTransformation(const FacialData& facialData);

/*
	\brief Utility to visualize the transformation matrix

	\param W				Input transformation matrix.
*/
void draw_faces(const cv::Mat& W);

int authenticate(const FacialData& facialData, const TransformationData& transformationData, const cv::Mat& img);

/*
	\brief Utility to test the accuracy of this algorithm on the previously given dataset

	Will apply the transformation matrix onto the test matrix and then for each image vector it will find the closest train image vector and check if it corresponds to the actual train class.
	Prints the resulting accuracy in console.

	It should be called in debug.

	\param facialData - Input facial data
	\param transformationData - Input transformation data

	\see FacialData
	\see TransformationData
*/
void testRecognition(const FacialData& facialData, const TransformationData& transformationData);

//void testAuthentication(const FacialData& facialData, const TransformationData& transformationData);

//cv::Mat softmax(cv::Mat in);
