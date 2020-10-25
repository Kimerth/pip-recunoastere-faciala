#include "ImageViewer.h"
#include <QApplication>
#include <QHBoxLayout>
#include "ImageGridCell.h"
#include <QPainter>

ImageViewer::ImageViewer(QWidget *parent)
	: AbstractViewer(parent),
	t({ 0, 0 }), imPos({ 0 , 0 }), imSize({ 0, 0 }), image(Image({ 0, 0 })),
	scrollBarThickness(4),
	enableScrollBars(false)
{
}

void ImageViewer::setImage(const Image& _image)
{
	image = _image;
	imSize = image.dims();
	update();
}

void ImageViewer::restoreOriginalSize()
{
	imPos = { 0, 0 };
	imSize = image.dims();
	scaleFactor = 1.0f;
	update();
	updateInfo();
}

void ImageViewer::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		mousePosX = e->pos().x();
		mousePosY = e->pos().y();
	}
}

void ImageViewer::mouseMoveEvent(QMouseEvent *e)
{
	if (!autoResize)
	{
		if (e->buttons() & Qt::LeftButton)
		{
			int mx = e->pos().x();
			int my = e->pos().y();
			t = { mx - mousePosX , my - mousePosY };
			mousePosX = mx;
			mousePosY = my;
			update();
		}
	}
}

void ImageViewer::wheelEvent(QWheelEvent *e)
{
	if (!autoResize)
	{
		float oldScaleFactor = scaleFactor;
		scaleFactor *= 1 + (float)e->delta() / 1200;
		imPos -= {(int)(e->pos().x() * (scaleFactor - oldScaleFactor)), (int)(e->pos().y() * (scaleFactor - oldScaleFactor))};
		imSize = image.dims() * scaleFactor;
		update();
		updateInfo();
	}
}

void ImageViewer::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_A &&
		(QApplication::keyboardModifiers() & Qt::ControlModifier))
	{
		autoResize ? setAutoResize(false) : setAutoResize(true);
	}
	else if (e->key() == Qt::Key_R && !autoResize &&
		(QApplication::keyboardModifiers() & Qt::ControlModifier))
	{
		restoreOriginalSize();
	}

	AbstractViewer::keyPressEvent(e);
}

void ImageViewer::resizeEvent(QResizeEvent* e)
{
	if (autoResize)
		updateInfo();
}

QString ImageViewer::getInfo()
{
	std::string info;
	info += "Size:" + std::to_string(imSize.x) + "x" + std::to_string(imSize.y) +
		"\nOriginal size: " + std::to_string(image.width()) + "x" +
		std::to_string(image.height()) +
		"\nAuto resize: " + (autoResize ? "on" : "off");
	return QString::fromStdString(info);
}

void ImageViewer::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	QPen pen(Qt::red);
	painter.setPen(pen);
	int viewW = width();
	int viewH = height();

	float imAspect = (float)image.width() / image.height();
	float viewAspect = (float)viewW / viewH;
	if (autoResize)
	{
		if (imAspect >= viewAspect)
		{
			imPos = { 0, (int)(viewH - viewW / imAspect) / 2 };
			imSize = {viewW, (int)(viewW / imAspect)};
		}
		else
		{
			imPos = { (int)(viewW - viewH * imAspect) / 2, 0 };
			imSize = { viewH, (int)(viewH * imAspect) };
		}
		t = { 0, 0 };
	}
	else
	{
		if (imSize.x <= viewW)
		{
			imPos.x = (viewW - imSize.x) / 2;
			t.x = 0;
		}
		else
		{
			imPos.x += t.x;
			if (imPos.x > 0) imPos.x = 0;
			if (imPos.x < viewW - imSize.x) imPos.x = viewW - imSize.x;
		}
		if (imSize.y <= viewH)
		{
			imPos.y = (viewH - imSize.y) / 2;
			t.y = 0;
		}
		else
		{
			imPos.y += t.y;
			if (imPos.y > 0) imPos.y = 0;
			if (imPos.y < viewH - imSize.y) imPos.y = viewH - imSize.y;
		}
	}
			
	painter.drawImage(QRect(imPos.x, imPos.y, imSize.x, imSize.y), *image.get_qimage());

	//pseudoscrollbars
	if (enableScrollBars && !autoResize)
	{
		if (imSize.x > viewW)
		{
			int sbarSizeX = viewW * viewW / imSize.x;
			int sbarSizeX2 = sbarSizeX / 2;
			int sbarMinPosX = sbarSizeX2;
			int sbarMaxPosX = viewW - sbarSizeX2;
			int sbarPosX = sbarMinPosX + (float)imPos.x / (viewW - imSize.x) * (sbarMaxPosX - sbarMinPosX);

			pen.setColor(Qt::darkRed);
			pen.setWidth(scrollBarThickness);
			painter.setPen(pen);
			painter.drawRect(sbarPosX - sbarSizeX2, viewH - scrollBarThickness,
				sbarSizeX, scrollBarThickness);
		}
		if (imSize.y > viewH)
		{
			int sbarSizeY = viewH * viewH / imSize.x;
			int sbarSizeY2 = sbarSizeY / 2;
			int sbarMinPosY = sbarSizeY2;
			int sbarMaxPosY = viewH - sbarSizeY2;
			int sbarPosY = sbarMinPosY + (float)imPos.y / (viewH - imSize.x) * (sbarMaxPosY - sbarMinPosY);

			pen.setColor(Qt::darkRed);
			pen.setWidth(scrollBarThickness);
			painter.setPen(pen);
			painter.drawRect(viewW - scrollBarThickness, sbarPosY - sbarSizeY2,
				scrollBarThickness, sbarSizeY);
		}
	}	
	//pen.setColor(Qt::red);
	//painter.setPen(pen);
	//painter.drawLine(0, 0, viewW, viewH);
	//painter.drawLine(viewW, 0, 0, viewH);
	//painter.drawRect(QRect(0, 0, viewW, viewH));
}

//QSize ImageViewer::sizeHint() const
//{
//	if (image) return image->size();
//	return QWidget::sizeHint();
//}

void ImageViewer::setAutoResize(bool enabled)
{
	autoResize = enabled;
	if (!autoResize)
	{
		restoreOriginalSize();
	}
	else
	{
		repaint();
		updateInfo();
	}
}
