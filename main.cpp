

#include <time.h>
#include "AOI.h"

int main()
{
	AOI* aoi=new AOI;

	//³¡¾°´óÐ¡30*30
	aoi->init(3, 3, 10);

	srand(time(NULL));
	printf("\nbegin add object=============\n");
	for (int i = 0; i < 10; ++i) {
		aoi->enter(i, rand() % 30, rand() % 30, 10);
	}

	printf("\nbegin move object=============\n");
	for (int i = 0; i < 100; ++i) {
		int x = rand() % 30;
		int y = rand() % 30;
		aoi->moveTo(i % 10, x, y);
	}

	printf("\nbegin leave object=============\n");
	for (int i = 0; i < 10; ++i) {
		aoi->leave(i);
	}

	return 0;
}