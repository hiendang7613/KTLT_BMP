#include "Bmp.h"

bool isBmpFile(FILE* f)
{
    if (f == NULL) return false;
    BmpSignature signature;
    fseek(f, 0, 0L);
    fread(&signature, sizeof(BmpSignature), 1, f);
    return signature.data[0] == 'B' && signature.data[1] == 'M';
}

void readBmpHeader(FILE* f, BmpHeader& header)
{
    if (f == NULL) return;
    fseek(f, 0, 0L);
    fread(&header, sizeof(BmpHeader), 1, f);
}

void printBmpHeader(BmpHeader header)
{
    cout << "* BMP Header ***\n";
    cout << "- File Size : " << header.fileSize << " byte(s)\n";
    cout << "- Reserved1 :" << header.reserved1 << endl;
    cout << "- Reserved2 :" << header.reserved2 << endl;
    cout << "- Data Offset: " << header.dataOffset << " byte(s)\n";
}

void writeBmpHeader(FILE* f, BmpHeader header)
{
    if (f == NULL) return;
    fseek(f, 0, 0L);
    fwrite(&header, sizeof(BmpHeader), 1, f);
}

void readBmpDib(FILE* f, BmpDib* dib)
{
    if (f == NULL) return;
    fseek(f, sizeof(BmpHeader), 0L);
    fread(dib, sizeof(BmpDib), 1, f);
}

void printBmpDib(BmpDib dib)
{
    cout << "* BMP Dib ***\n";
    cout << "- Dib Size               :" << dib.dibSize << " byte(s)\n";
    cout << "- Image Width            :" << dib.imageWidth << endl;
    cout << "- Image Height           :" << dib.imageHeight << endl;
    cout << "- Number of Color Planes :" << dib.colorPlaneCount << endl;
    cout << "- Pixel Size             :" << dib.pixelSize << " bit(s)\n";
    cout << "- Compress Method        :" << dib.compressMethod << endl;
    cout << "- Bitmap Size            :" << dib.bitmapByteCount << " byte(s)\n";
    cout << "- Horizontal Resolution  :" << dib.horizontalResolution << endl;
    cout << "- Vertical Resolution    :" << dib.verticalResolution << endl;
    cout << "- Number of Colors       :" << dib.colorCount << endl;
    cout << "- Number of Impt Colors  :" << dib.importantcolorCount << endl;
}

void writeBmpDib(FILE* f, BmpDib* dib)
{
    if (f == NULL) return;
    fseek(f, sizeof(BmpHeader), 0L);
    fwrite(dib, sizeof(BmpDib), 1, f);
}

void readBmpPixelArray(FILE* f, BmpHeader header, BmpDib dib, PixelArray& data)
{
    if (f == NULL) return;
    data.rowCount = dib.imageHeight;
    data.columnCount = dib.imageWidth;
    data.pixels = new Color * [data.rowCount];
    char paddingCount = (4 - (dib.imageWidth * (dib.pixelSize / 8) % 4)) % 4;
    fseek(f, header.dataOffset, 0L);
    for (int i = 0; i < data.rowCount; i++) {
        scanBmpPixelLine(f, data.pixels[data.rowCount - 1 - i], dib.imageWidth);
        skipBmpPadding(f, paddingCount);
    }
}

void scanBmpPixelLine(FILE* f, Color*& line, uint32_t length)
{
    if (f == NULL) return;
    line = new Color[length];
    fread(line, sizeof(Color), length, f);
}

void skipBmpPadding(FILE* f, char count)
{
    if (f == NULL) return;
    if (count == 0) return;
    char padding[3];
    fread(padding, count, 1, f);
}

void drawBmp(BmpDib dib, PixelArray data, int x)
{
    HWND console = GetConsoleWindow();
    HDC hdc = GetDC(console);
    for (int i = 0; i < dib.imageHeight; i++) {
        for (int j = 0; j < dib.imageWidth; j++) {
            Color pixel = data.pixels[i][j];
            SetPixel(hdc, j+x, i, RGB(pixel.red, pixel.green, pixel.blue));
        }
    }
    ReleaseDC(console, hdc);
}

void writeBmpPixelArray(FILE* f, BmpHeader header, BmpDib dib, PixelArray& data)
{
    if (f == NULL) return;
    char paddingCount = (4 - (data.columnCount * (dib.pixelSize / 8) % 4)) % 4;
    fseek(f, header.dataOffset, 0L);
    for (int i = 0; i < data.rowCount; i++) {
        fwrite(data.pixels[data.rowCount - 1 - i], sizeof(Color), data.columnCount, f);
        skipBmpPadding(f, paddingCount);
    }
}

void releaseBmpPixelArray(PixelArray data)
{
    for (int i = 0; i < data.rowCount; i++) {
        delete[]data.pixels[i];
    }
    delete[]data.pixels;
}

void fix(Color& c) {
    unsigned char v = (c.green+c.blue+c.red)/3;
    c.green = c.blue = c.red = v;
}

void BlackWhite(PixelArray& data)
{
    for (size_t j = 0; j < data.rowCount; j++)
    {
        for (size_t i = 0; i < data.columnCount; i++)
        {
            fix(data.pixels[j][i]);
        }
    }
}
