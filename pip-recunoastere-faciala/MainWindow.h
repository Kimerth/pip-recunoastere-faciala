#pragma once
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, private Ui_MainWindow
{
	Q_OBJECT
public:
	MainWindow();

public slots:
	void detect();
	void select();
	void displayImage(const char* path);

signals:
	void selected(const char* path);

private:
	QString selectedFile;
};