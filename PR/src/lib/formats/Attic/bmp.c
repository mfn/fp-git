/*
#if DIR_SEPARATOR=='/'
#include "bmp.h"
#else
#include "formats/bmp.h"
#endif
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "memory.h"
#include "compile.h"

char mFormatExtractBmp(unsigned char* data, char *vFileext,unsigned long int size,tImage image) {
printf("hola, vengo a colgarme\n");
	if ((mExpandGraphic(data,&image,size))>0) {
printf("ya expandi\n");
		mWriteBitMap(image,vFileext);
printf("Escribi el bitmap\n");
		free(image.pix);
		return 1;
	} else {
		return 0;
	}
}


char mFormatCompileBmp(unsigned char* data, FILE* fp, tResource *res) {
	int size;
	tImage img;
	unsigned char aux[10000];
	if (!mReadBitMap(&img,data,(*res).size)) return 0;
	//free(data);
			//printf("llega 2\n");
	mCompressGraphic(aux,img,&size);
	printf("llega ssss2\n");
	free(img.pix);
	printf("llega ssss3\n");
	mAddFileToDatFile(fp,aux,size);
	printf("llega ssss4\n");
	(*res).size=size; //TODO: delete this bug (added to debug ;) ironic, don't you think
	return 1;
}

char mWriteBitMap(tImage img,char* vFile) {

	//declare variables
	unsigned char i=0;
	unsigned char j=0;
	char b;
	char c;
	char a;
	FILE* bitmap;
	unsigned long int filesize;
	unsigned long int width;
	unsigned long int height;
	char junk[3]={0,0,0};

	unsigned char header[]={
		'B','M',
		0x22,0x22,0x22,0x22,
		0,0,0,0,
		0x76,0,0,0,

		40,0,0,0,
		0x22,0x22,0,0,
		0x22,0x22,0,0,
		1,0,
		4,0,
		0,0,0,0,
		0,0,0,0,
		0xE8,0x03,0,0,
		0xE8,0x03,0,0,
		0x10,0,0,0,
		0,0,0,0,

		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0,
		0x22,0x22,0x22,0
	};


	if ((bitmap = fopen (vFile,"wb"))==NULL) return 0;

	filesize=((img.size+1)/2+118);
	width=img.width;
	height=img.height;


	//TODO: avoid using header array andwrite everithing in one run

	header[2]=filesize;
	header[3]=filesize>>8;
	header[4]=filesize>>16;
	header[5]=filesize>>24;

	header[18]=width;
	header[19]=width>>8;

	header[22]=height;
	header[23]=height>>8;

//BEGIN of format writing
	//Write ColorTable
	for (a=0;a<16;a++) {
		b=a*3;
		c=a<<2;
		header[54+c]=(img.pal[b+2])*4;  //Red
		header[55+c]=(img.pal[b+1])*4;  //Green
		header[56+c]=(img.pal[b])*4;    //Blue
	}
	//Write header
	fwrite(header,118,1,bitmap);
	//Write data
	img.width=(img.width+1)>>1;
	while (img.height--) {
		fwrite(img.pix+img.height*img.width,img.width,1,bitmap);
		fwrite(junk,(-img.width)&3,1,bitmap);
	}
//END of format writing

	fclose(bitmap);
	return 1;
}


char mReadBitMap(tImage* img,char* data, int size) {
	char ok;
	unsigned short int width;
	unsigned short int height;
	int width2;
	int x=0;

	//Validate if there is header and if it starts in BM
	ok    = size>118;
	ok=ok&& data[0]=='B' && data[1]=='M';

	//Read sizes from header
	width=data[18]+(data[19]<<8);
	height=data[22]+(data[23]<<8);

	//Save sizes into image
	(*img).width=width;            //width in pixels
	(*img).height=height;
	(*img).size=height*width;      //total of pixels

	//Calculate serialized widths
	width=(width+1)>>1;            //raw serialized width
	width2=width+((-width)&3); //bmp serialized width

	//Validate image and file size; get memory to allocate the image
	ok=ok&& ( ((*img).height*width2)                == (size-118));
	ok=ok&& (	((*img).pix=getMemory((*img).size/2)) != NULL	);

	//if validations==wrong
	if (!ok) {
		if ((*img).pix!=NULL) free((*img).pix);
		return 0; //this is not a valid bmp file format or memory too low
	}

	//Serialize bitmap-->raw array
	while (height--) memcpy((*img).pix+(x++)*width,data+118+height*width2,width);

	return 1;
}
