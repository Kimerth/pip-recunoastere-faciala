#pragma once
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QLabel>
#include "AbstractViewer.h"
#include <memory>
#include "Image.hpp"

class ImageGridCell : public QWidget
{
	QLabel *titleLabel;
	AbstractViewer *viewer;
	QLabel *bottomLabel;
	QVBoxLayout *layout;
	bool showInfo;

public:
	ImageGridCell(QWidget *parent = nullptr);
	void setImage(const Image&, QString);
	void setHistogram(int *values, int size, QString title = "");
	void updateInfo();
	void setImage(QString fileName, QString title = "");
	void setBottomText(QString text = "");
	void setAutoResize(bool enable);
	void restoreOriginalSize();
	QSize sizeHint() const;
}; 