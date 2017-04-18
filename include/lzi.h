#ifndef LIBLZI_VERSION
#define LIBLZI_VERSION v1.0
#ifndef LIBLZI_BUFFER_SIZE
#define LIBLZI_BUFFER_SIZE 1000
#endif

#if (LIBLZI_BUFFER_SIZE < 100)
#error Buffer size nem megfelelo
#endif


int BMPtoLZI(const char* bmp, const char* lzi, int width, int height, int bmpoffset, unsigned char bmpbpp, unsigned char lzibpp);
int loadLZI(const char* lzi, char* ered, unsigned long eredSize, unsigned int& w, unsigned int& h, unsigned char& bpp);
#endif
