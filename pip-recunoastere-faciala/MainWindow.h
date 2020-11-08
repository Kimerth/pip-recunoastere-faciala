#pragma once
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, private Ui_MainWindow
{
public:
	MainWindow();

public slots:
	void detect();
	void select();

private:
	QString selectedFile;
};