#include <QApplication>
#include "MainWindow.h"
#include <iostream>
#include <filesystem>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow mainWindow;

	mainWindow.show();
	return app.exec();
}