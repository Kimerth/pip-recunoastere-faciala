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

	auto facialData = readData(40, 10);

	auto transformation = computeTransformation(facialData);
	draw_faces(transformation.W);
	test(facialData, transformation);
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