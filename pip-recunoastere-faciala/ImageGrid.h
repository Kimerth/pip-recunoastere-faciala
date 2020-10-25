#pragma once
#include <QGridLayout>
#include <QKeyEvent>
#include "ImageGridCell.h"
#include <iostream>
#include <map>
#include "Image.hpp"
#include <memory>

class ImageGrid : public QWidget
{
	QGridLayout *layout;
	bool autoResizeCells;

public:
	ImageGrid(QString title = "ACPI", QWidget *parent = nullptr);
	void addImage(const Image& image, Vec2 pos, QString title = "");
	void addImage(QString fileName, int posX, int posY, QString title = "");
	void addHistogram(int *values, int nrValues, int posX, int posY, QString title = "histogram");
	QSize getCellSize();
	void keyPressEvent(QKeyEvent *e);
};