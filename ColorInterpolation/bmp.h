#ifndef BMP_H
#define BMP_H

// This structure is used for recording the imformation of the first header of BMP file.
struct BitMapFileHeader
{
	short	bfType;		// 2 bytes, save 'bm' these word ASCII code, bmp file format token
	int		bfSize;		// 4 bytes, bitmap file size, unit is byte
	short bfReserved1;	// 2 bytes, reservation, pad 0
	short bfReserved2;	// 2 bytes, reservation, pad 0
	int		bfOffBits;	// 4 bytes, Image data block begin postion
};// 14 bytes

// This structure is used for recording the imformation of the second header of BMP file.
struct BitMapInfoHeader
{
	int	biSize; // 4 bytes, Bit Map Info Header size
	int	biWidth;// 4 bytes, image width, present by pixel count
	int	biHeight;// 4 bytes, image height, present by pixel count
	short biPlanes;// 2 bytes, image count, thie field must be 1
	short biBitCount;// 2 bytes, bits count per pixel, now only apply for 24 bits( true color )
	int	biCompression;// 4 bytes, Image data compression, 0: no compression, 1: compress by 8 bits unit, 2: compress by 4 bits unit
	int	biSizeImage;// 4 bytes, Image Data Block size, unit is byte
	int	biXPelsPerMeter;// 4 bytes, pixel count per one horizontal
	int	biYpelsPerMeter;// 4 bytes, pixel count per one vertical
	int	biClrUsed;// 4 bytes, color count used by this image
	int	biClrImportant;// 4 bytes, important color count in this image
};// 40 bytes

#endif
