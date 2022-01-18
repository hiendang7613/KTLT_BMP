#include"BMP.h"

int main() {
	FILE* f = fopen("SovietFlag_on_Reichstag.bmp", "rb");
	FILE* fout = fopen("out.bmp", "wb");
	BmpDib* dib = new BmpDib;
	BmpHeader header;
	PixelArray data;

	readBmpHeader(f, header);	
	writeBmpHeader(fout, header);	// output bmp
	printBmpHeader(header);			// output console 

	readBmpDib(f, dib);
	writeBmpDib(fout, dib);		// output bmp
	printBmpDib(*dib);			// output console

	readBmpPixelArray(f, header, *dib, data);
	BlackWhite(data);
	writeBmpPixelArray(fout, header, *dib, data);
	drawBmp(*dib, data,10);
	Sleep(10000);

	releaseBmpPixelArray(data);
	fclose(f);
	fclose(fout);
	return 0;
}