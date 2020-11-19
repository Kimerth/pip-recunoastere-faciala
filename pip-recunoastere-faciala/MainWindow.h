#pragma once
#include "ui_MainWindow.h"
#include <QMouseEvent>
#include <QWheelEvent>

class MainWindow : public QMainWindow, private Ui_MainWindow
{
	Q_OBJECT
public:
	MainWindow();

	void mouseMoveEvent(QMouseEvent* e) override;
	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void wheelEvent(QWheelEvent* e) override;

public slots:
	void detect();
	void select();
	void displayImage(const QString&);

signals:
	void selected(QString&);

private:
	QGraphicsScene scene;
	QGraphicsRectItem* item = nullptr;

	QPoint mousePos;
	bool mousePressed;

	void moveSquare(const QPoint& diff, double scale);
};