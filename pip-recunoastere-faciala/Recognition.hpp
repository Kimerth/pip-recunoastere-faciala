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
};

/*
	\brief Loads train data and test data from disk.

	\param classes			Number of classes.
	\param nSample			Number of samples per class.

	\see struct FacialData
*/
FacialData readData(int nClasses, int nSamples);

/*
	\brief Computes the matrix used to represent the input data by finding a subspace which represents most of the data variance
	
	https://cseweb.ucsd.edu/classes/wi14/cse152-a/fisherface-pami97.pdf

	\param facialData		Input facial data to compute transformation
	\returns				Output transformation data

	\see FacialData
	\see TransformationData
*/
TransformationData computeTransformation(const FacialData& facialData);

/*
	\brief Utility to visualize the transformation matrix

	\param W				Input transformation matrix.

	\see FacialData
*/
void draw_faces(const cv::Mat& W);

/*
	\brief Utility to test the accuracy of this algorithm on the previously given dataset

	Will apply the transformation matrix onto the test matrix and then for each image vector it will find the closest train image vector and check if it corresponds to the actual train class.
	Prints the resulting accuracy in console.

	\param W				Input transformation matrix.
	\param classes			Input train classes.
	\param classes_test		Input test classes.
	\param X_test			Input test matrix.
*/
void test(const FacialData& facialData, const TransformationData& transformationData);

//cv::Mat softmax(cv::Mat in);
