#pragma once
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include "AbstractViewer.h"
#include <memory>
#include "Image.hpp"

class ImageWidget;

class ImageViewer : public AbstractViewer
{
	Image image;
	Vec2 t, imPos, imSize;
	int scrollBarThickness;
	int enableScrollBars;

public:
	ImageViewer(QWidget *parent);
	void setImage(const Image& image);
	void restoreOriginalSize();
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void resizeEvent(QResizeEvent* e);
	void wheelEvent(QWheelEvent *e);
	void keyPressEvent(QKeyEvent *e);
	QString getInfo();
	void paintEvent(QPaintEvent *e);
	//QSize sizeHint() const;
	void setAutoResize(bool enabled);
};