#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include "MainWindow.h"
#include "Recognition.hpp"

MainWindow::MainWindow()
{
	setupUi(this);

	connect(detectButton, &QPushButton::clicked, this, &MainWindow::detect);
	connect(selectButton, &QPushButton::clicked, this, &MainWindow::select);
	connect(this, &MainWindow::selected, filePath, &QLineEdit::setText);
	connect(this, &MainWindow::selected, this, &MainWindow::displayImage);
}

void MainWindow::detect()
{
	srand(time(NULL));

	auto facialData = readData(40, 10);

	auto transformation = computeTransformation(facialData);
	//draw_faces(transformation.W);
	auto rez = test(facialData, transformation);

	char x[32];
	sprintf_s(x, "%f", rez);

	QMessageBox::information(this, "Result", x);
}

void MainWindow::select()
{
	QFileDialog dialog(nullptr, "Select image", ".", "Images (*.pgm)");
	dialog.setFileMode(QFileDialog::FileMode::ExistingFile);

	if (dialog.exec())
	{
		selectedFile = dialog.selectedFiles()[0];
		filePath->setText(selectedFile);
	}
}

void MainWindow::displayImage(const char* path)
{
	// TODO
}