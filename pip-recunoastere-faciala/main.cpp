#include <QApplication>
#include "ImageGrid.h"
#include "operatii.h"
#include "Image.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	/* generam o fereastra care va contine una sau mai multe imagini
	   plasate sub forma unei matrici 2D */
	ImageGrid grid("Prelucrarea imaginilor");

	Image img("Images/lena512.bmp");

	grid.addImage(img, { 0, 0 }, "imagine");

	// exemplu de operatie: negativarea unei imagini 

	/*parcurgem imaginea pixel cu pixel si determinam valoarea complementara
	pentru fiecare intensitate

	se recomanda ca acest gen de operatie sa se implementeze intr-o functie
	separata sau intr-o alta clasa
	*/

	Image negated = negateImage(img);
	grid.addImage(negated, { 0, 1 }, "negativ");

	grid.show();
	
	return a.exec();
}

