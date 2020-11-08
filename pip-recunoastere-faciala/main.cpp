#include <QApplication>
#include "ImageGrid.h"
#include "operatii.h"
#include "Image.hpp"
#include "Recognition.hpp"

int main(int argc, char *argv[])
{
	srand(time(NULL));

	cv::Mat X, X_test;
	std::vector<int> classes, classes_test;
	readData(40, 10, X, classes, X_test, classes_test);

	cv::Mat Y, W;
	computeTransformation(40, X, classes, W, Y);
	//draw_faces(W);
	test(W, Y, classes, classes_test, X_test);
}

