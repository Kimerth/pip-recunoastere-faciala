#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow()
{
	setupUi(this);

	connect(detectButton, &QPushButton::clicked, this, &MainWindow::detect);
	connect(selectButton, &QPushButton::clicked, this, &MainWindow::select);
}

void MainWindow::detect()
{
	QMessageBox::information(this, "hurr durr", "detecting");
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