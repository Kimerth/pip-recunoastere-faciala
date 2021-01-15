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

	if (image.isNull())
		return false;

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
	if (image.isNull())
		return;

	QMainWindow::paintEvent(ptr);

	QPainter painter(this);
	QPen pen = QPen(Qt::red, 5);
	painter.setPen(pen);

	painter.drawImage(QPoint{ 0, 0 }, image);
	painter.drawRect(rectPos.x(), rectPos.y(), 92 * rectScale, 112 * rectScale);
}

void MainWindow::detect()
{
	if (image.isNull())
	{
		QMessageBox::warning(this, "Error", "Please select an image first");
		return;
	}

	//srand(time(NULL));

	cv::Mat img = cv::imread(filePath->text().toStdString().data(), cv::IMREAD_GRAYSCALE);

	const auto& pos = rectPos;

	img = img(cv::Rect(pos.x(), pos.y(), rectScale * 92, rectScale * 112));
	cv::resize(img, img, { 92, 112 });
#ifndef NDEBUG
	cv::imshow("resized", img);
#endif

	auto facialData = readData();
	auto transformation = computeTransformation(facialData);
	//draw_faces(transformation.W);

	int rez = authenticate(facialData, transformation, img);

	char x[64];
	if (rez >= 0)
		sprintf_s(x, "Acceptat, Id: %s", facialData.class_names[rez].data());
	else
		sprintf_s(x, "Refuzat");

	QMessageBox::information(this, "Result", x);

#ifndef NDEBUG
	testRecognition(facialData, transformation);
#endif // !NDEBUG
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
	image = QImage(path);
	rectPos = { 0, 0 };
	rectScale = 1;
}

void MainWindow::moveSquare(const QPoint& diff)
{
	rectPos = lastRectPos + diff;

	rectPos.rx() = std::clamp<int>(rectPos.x(), 0, image.width() - rectScale * 92);
	rectPos.ry() = std::clamp<int>(rectPos.y(), 0, image.height() - rectScale * 112);

	update();
}

void MainWindow::scaleSquare(double val)
{
	rectScale += val;
	rectScale = std::clamp<double>(rectScale, 1, std::min(image.width() / 92.0, image.height() / 112.0));

	rectPos.rx() = std::clamp<int>(rectPos.x(), 0, image.width() - rectScale * 92);
	rectPos.ry() = std::clamp<int>(rectPos.y(), 0, image.height() - rectScale * 112);

	update();
}