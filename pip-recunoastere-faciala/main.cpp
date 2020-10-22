#include <QApplication>
#include "ImageGrid.h"
#include "operatii.h"

/* Shortcuts:

Scroll wheel - zoom in/out

Left button drag - deplasare in imagine cand aceasta este mai mare decat fereastra

Ctrl+A - activeaza / dezactiveaza ajustarea automata a dimensiunii imaginii 
		 pe care se afla cursorul mouse-ului

Shift+A - activeaza / dezactiveaza ajustarea automata a dimensiunii 
		  tuturor imaginilor

Ctrl+R - reseteaza imaginea curenta la dimensiunile sale initiale

Shift+R - reseteaza toate imaginile la dimensiunile lor initiale

*/

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	/* generam o fereastra care va contine una sau mai multe imagini
	   plasate sub forma unei matrici 2D */
	ImageGrid *grid = new ImageGrid("Prelucrarea imaginilor");

	QString imageDir = "Images/";
	QString imageFile(imageDir + "lena512.bmp");

	/*adaugam prima imagine, cea initiala, citita din fisier,
	in pozitia stanga-sus (linia 0, coloana 0)*/
	grid->addImage(imageFile, 0, 0);

	/* extragem imformatiile necesare din imagine:
	dimensiunile ei si un sir de octeti care contine valorile
	intensitatilor pentru fiecare pixel */
	int w, h;
	unsigned char* img = Tools::readImageGray8(imageFile, w, h);

	// exemplu de operatie: negativarea unei imagini 

	/*parcurgem imaginea pixel cu pixel si determinam valoarea complementara
	pentru fiecare intensitate

	se recomanda ca acest gen de operatie sa se implementeze intr-o functie
	separata sau intr-o alta clasa
	*/

	unsigned char* negated = negateImage(img, w, h);

	/* afisam imaginea astfel obtinuta la dreapta celei initiale;
	parametrii cu valorile 0, 1 semnifica prima linie, respectiv
	a doua coloana a imageGrid-ului
	*/
	grid->addImage(negated, w, h, 0, 1, "negativ");

	grid->show();
	
	return a.exec();
}

