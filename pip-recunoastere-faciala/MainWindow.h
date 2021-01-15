#pragma once
#include "ui_MainWindow.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include "Recognition.hpp"

class MainWindow : public QMainWindow, private Ui_MainWindow
{
	Q_OBJECT
public:
	MainWindow();

	bool eventFilter(QObject* obj, QEvent* event);
	void paintEvent(QPaintEvent* ptr);

public slots:
	void detect();
	void select();
	void displayImage(const QString&);

signals:
	void selected(QString&);

private:
	
	FacialData facialData;
	TransformationData transformation;

	QPoint mousePos;
	bool mousePressed;

	QPoint lastRectPos = QPoint(0, 0);
	QPoint rectPos = QPoint(0, 0);
	double rectScale = 1;
	QImage image;

	void moveSquare(const QPoint& diff);
	void scaleSquare(double val);
};