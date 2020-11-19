#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include "MainWindow.h"
#include "Recognition.hpp"

MainWindow::MainWindow()
{
	setupUi(this);

	auto temp = QPixmap("Images/lena512.bmp");
	scene.addPixmap(temp);
	QPen pen = QPen(Qt::red, 5);
	scene.addRect(150, 150, 350, 350, pen);

	graphicsView->setScene(&scene);

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
	graphicsView->show();
}