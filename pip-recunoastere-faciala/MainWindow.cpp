#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QPainter>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "MainWindow.h"
#include "Recognition.hpp"

MainWindow::MainWindow()
{
	setupUi(this);

	installEventFilter(this);

	connect(detectButton, &QPushButton::clicked, this, &MainWindow::detect);
	connect(selectButton, &QPushButton::clicked, this, &MainWindow::select);
	connect(this, &MainWindow::selected, filePath, &QLineEdit::setText);
	connect(this, &MainWindow::selected, this, &MainWindow::displayImage);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
	auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
	auto wheelEvent = dynamic_cast<QWheelEvent*>(event);

	switch (event->type())
	{
	case QEvent::MouseMove:
		if (mousePressed)
		{
			auto diff = mouseEvent->pos() - mousePos;
			moveSquare(diff);
		}
		break;
	case QEvent::MouseButtonPress:
		if (mouseEvent->buttons() & Qt::LeftButton)
		{
			mousePressed = true;
			mousePos = mouseEvent->pos();
			lastRectPos = rectPos;
		}
		break;
	case QEvent::MouseButtonRelease:
		if (!(mouseEvent->buttons() & Qt::LeftButton))
			mousePressed = false;
		break;
	case QEvent::Wheel:
		scaleSquare(wheelEvent->delta() / 1200.0);
		break;
	}
	return false;
}

void MainWindow::paintEvent(QPaintEvent* ptr)
{
	QMainWindow::paintEvent(ptr);

	QPainter painter(this);
	QPen pen = QPen(Qt::red, 5);
	painter.setPen(pen);

	if(!image.isNull())
		painter.drawImage(QPoint{0, 0}, image);

	painter.drawRect(rectPos.x(), rectPos.y(), 92 * scale, 112 * scale);
}

void MainWindow::detect()
{
	if (image.isNull())
	{
		QMessageBox::warning(this, "Error", "Please select an image first");
		return;
	}

	srand(time(NULL));

	cv::Mat img = cv::imread(filePath->text().toStdString().data(), cv::IMREAD_GRAYSCALE);

	img = img(cv::Rect(rectPos.x(), rectPos.y(), scale * 92, scale * 112));

	cv::resize(img, img, { 92, 112 });
	cv::imshow("resized", img);

	//auto facialData = readData(40, 10);

	//auto transformation = computeTransformation(facialData);
	////draw_faces(transformation.W);
	//auto rez = test(facialData, transformation);

	//char x[32];
	//sprintf_s(x, "%f", rez);

	//QMessageBox::information(this, "Result", x);
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
	image = QImage(path).scaledToHeight(500);
}

void MainWindow::moveSquare(const QPoint& diff)
{
	rectPos = lastRectPos + diff;

	rectPos.rx() = std::clamp<int>(rectPos.x(), 0, (image.isNull() ? 800 : image.width()) - scale * 90);
	rectPos.ry() = std::clamp<int>(rectPos.y(), 0, 500 - scale * 112);

	update();
}

void MainWindow::scaleSquare(double val)
{
	scale += val;

	rectPos.rx() = std::clamp<int>(rectPos.x(), 0, (image.isNull() ? 800 : image.width()) - scale * 90);
	rectPos.ry() = std::clamp<int>(rectPos.y(), 0, 500 - scale * 112);
	scale = std::clamp<double>(scale, 1, 4);

	update();
}