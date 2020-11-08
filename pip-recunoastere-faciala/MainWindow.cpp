#include <QFileDialog>
#include <QMessageBox>
#include "MainWindow.h"
#include "Recognition.hpp"

MainWindow::MainWindow()
{
	setupUi(this);

	connect(detectButton, &QPushButton::clicked, this, &MainWindow::detect);
	connect(selectButton, &QPushButton::clicked, this, &MainWindow::select);
}

void MainWindow::detect()
{
	srand(time(NULL));

	cv::Mat X, X_test;
	std::vector<int> classes, classes_test;
	readData(40, 10, X, classes, X_test, classes_test);

	cv::Mat Y, W;
	computeTransformation(40, X, classes, W, Y);
	draw_faces(W);
	test(W, Y, classes, classes_test, X_test);
}

void MainWindow::select()
{
	QFileDialog dialog(nullptr, "Select image", ".", "Images (*.pgn)");
	dialog.setFileMode(QFileDialog::FileMode::ExistingFile);

	if (dialog.exec())
	{
		selectedFile = dialog.selectedFiles()[0];
		filePath->setText(selectedFile);
	}
}