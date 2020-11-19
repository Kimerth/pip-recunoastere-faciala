#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include "MainWindow.h"
#include "Recognition.hpp"

MainWindow::MainWindow()
{
	setupUi(this);

	graphicsView->setScene(&scene);

	connect(detectButton, &QPushButton::clicked, this, &MainWindow::detect);
	connect(selectButton, &QPushButton::clicked, this, &MainWindow::select);
	connect(this, &MainWindow::selected, filePath, &QLineEdit::setText);
	connect(this, &MainWindow::selected, this, &MainWindow::displayImage);
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton && mousePressed)
	{
		auto diff = e->pos() - mousePos;
		moveSquare(diff);
	}
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		mousePressed = true;
		mousePos = e->pos();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
	if(!(e->buttons() & Qt::LeftButton))
		mousePressed = false;
}

void MainWindow::wheelEvent(QWheelEvent* e)
{

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
	QFileDialog dialog(nullptr, "Select image", ".", "Images (*.pgm *.png *.jpg *.bmp)");
	dialog.setFileMode(QFileDialog::FileMode::ExistingFile);

	if (dialog.exec())
	{
		emit selected(dialog.selectedFiles()[0]);
	}
}

void MainWindow::displayImage(const QString& path)
{
	scene.clear();

	QPixmap img(path);
	scene.addPixmap(img);

	QPen pen = QPen(Qt::red, 5);
	item = scene.addRect(0, 0, 368, 448, pen);

	scene.update(scene.sceneRect());
}

void MainWindow::moveSquare(const QPoint& diff, double scale)
{
	if (item != nullptr)
	{
		item->setPos(item->pos() + diff);
		item->setScale(item->scale() + scale);
	}
}