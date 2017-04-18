#include <fstream>
#include <stdio.h>
#include "lzi.h"

using namespace std;

unsigned char lziStatBuffer[LIBLZI_BUFFER_SIZE];

int de(istream& in, unsigned char bpp, unsigned char* buf,unsigned char* de){
  de[3]=0xFF;
  if(!in.read((char*)buf,bpp))
    return 0;
  if(bpp==1){
    de[0]=de[1]=de[2]=buf[0];
  }else if(bpp==3){
    de[0]=buf[0];
    de[1]=buf[1];
    de[2]=buf[2];
  }else if(bpp==4){
    de[0]=buf[0];
    de[1]=buf[1];
    de[2]=buf[2];
    de[3]=buf[3];
  }
  return 1;
}

int en(ostream& out, unsigned char bpp, unsigned char* buf, unsigned char* de){
  buf[3]=0xFF;
  if(bpp==1){
    buf[0]=(de[0]+de[1]+de[2])/3;
  }else if(bpp==3){
    buf[0]=de[0];
    buf[1]=de[1];
    buf[2]=de[2];
  }else if(bpp==4){
    buf[0]=de[0];
    buf[1]=de[1];
    buf[2]=de[2];
    buf[3]=de[3];
  }
  if(!out.write((char*)buf,bpp))
    return 0;
  else
    return 1;
}

int BMPtoLZI(const char* bmp, const char* lzi, int width, int height,int bmpoffset, unsigned char bmpbpp, unsigned char lzibpp){
/*
HEADER:
"LATZ" Width Height Bpp Reserved Data
//4      4      4    1     1     W*H*B

1 GrAY
3 TRUE
4 RGBA (alpha 0 -> nem latszik)
*/
  if(bmpbpp!=1&&bmpbpp!=3&&bmpbpp!=4){
    printf("ERROR: BMP Bpp not supported!\n");
    return 1;
  }
  if(lzibpp!=1&&lzibpp!=3&&lzibpp!=4){
    printf("ERROR: LZI Bpp not supported!\n");
    return 1;
  }
  bool amigood=1;
	ifstream in;
	in.open(bmp,istream::binary);
	if(!in){
	  printf("ERROR: Failed to load file %s\n",bmp);
	  in.close();
	  return 1;
	}
	in.seekg(bmpoffset,in.beg);
	ofstream out;
	out.open(lzi,ostream::binary);
	if(!out){
	  printf("ERROR: Failed to load file %s\n",lzi);
	  in.close();
	  out.close();
	  return 1;
	}
	lziStatBuffer[0]='L';
	lziStatBuffer[1]='A';
	lziStatBuffer[2]='T';
	lziStatBuffer[3]='Z';
	*((unsigned int*)(lziStatBuffer+4))=width;
	*((unsigned int*)(lziStatBuffer+8))=height;
	lziStatBuffer[12]=(char)lzibpp;
	lziStatBuffer[13]=0;
	if(!out.write((char*)lziStatBuffer,14)){
	  printf("ERROR: Failed to write to file %s\n",lzi);
	  in.close();
	  out.close();
	  return 1;
	}
  unsigned char buffer[4];
  unsigned long n=(unsigned long)width*height;
  for(unsigned long i=0;i<n;i++){
    if(!de(in,bmpbpp,lziStatBuffer,buffer)){
      printf("ERROR: Function 'de' failed\n");
      if(!in){
          printf("ANYAD!\n");
      }
      return 1;
    }
    if(!en(out,lzibpp,lziStatBuffer,buffer)){
      printf("ERROR: Function 'en' failed\n");
      return 1;
    }
  }
  in.close();
  out.close();
  if(!amigood){
    return 1;
  }
  return 0;
}

int loadLZI(const char* lzi, char* ered, unsigned long eredSize, unsigned int& w, unsigned int& h, unsigned char& bpp){
	ifstream in;
	in.open(lzi,istream::binary);
	if(!in.good()){
	  printf("ERROR: Failed to load file %s\n",lzi);
	  in.close();
	  return 1;
	}
	unsigned int wh[2];
	unsigned int ID=0;
  if(!in.read((char*)&ID,4)){
    printf("ERROR: Failed to read from file %s\n",lzi);
	  in.close();
	  return 1;
  }
  if(ID!=0x5A54414C){
    printf("ERROR: Wrong format (file: %s)\n",lzi);
	  in.close();
	  return 1;
  }
  if(!in.read((char*)wh,8)){
    printf("ERROR: Failed to read from file %s (2)\n",lzi);
	  in.close();
	  return 1;
  }
  w=wh[0];
  h=wh[1];
  if(!in.read((char*)&bpp,1)){
    printf("ERROR: Failed to read from file %s (3)\n",lzi);
	  in.close();
	  return 1;
  }
  in.seekg(1,in.cur);
  //printf("%ux%u_%u\n",wh[0],wh[1],bpp);
  unsigned long size=wh[0]*wh[1]*bpp;
  if(eredSize<size){
    printf("ERROR: Output buffer size not enough (file: %s) \n",lzi);
	  in.close();
	  return 1;
  }
  if(!in.read(ered,size)){
    printf("ERROR: Failed to load file %s to Output buffer\n",lzi);
	  in.close();
	  return 1;
  }
  in.close();
  return 0;
}
