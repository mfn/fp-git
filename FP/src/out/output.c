/*  Princed V3 - Prince of Persia Level Editor for PC Version
    Copyright (C) 2003 Princed Development Team

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    The authors of this program may be contacted at http://forum.princed.com.ar
*/

/*
output.c: Free Prince : Output Devices Handler
��������

 Description: 
 	Platform and device independent functions for handling output
	(text, graphics, sound, music...).

 Copyright 2004 Princed Development Team
  Created: 23 Mar 2004

  Author: Ricardo Markiewicz <rmarkie (en) fi.uba.ar>

 Note:
  DO NOT remove this copyright notice
*/

#include <SDL/SDL.h>
#include <stdlib.h>    /* malloc */
#include <stdio.h>     /* fprintf NULL */
#include <string.h>    /* memset */
#include <stdarg.h>    /* var arg support. vsnprintf */
#include "resources.h" /* tMemory structure */
#include "output.h"
#include "kernel.h"    /* FP_VERSION */

#include "text_conf.h" /* TODO: use text.h */

#define DEF_SCREEN_WIDTH 320
#define DEF_SCREEN_HEIGHT 200

/* Main screen object */
SDL_Surface *screen;

/* Text functions */

#define CHAR_SIZE 12

typedef struct _valid_chars {
	char is_valid;      /* Is character valid ? */
	unsigned short x;   /* X pos in font image */
	unsigned short w;   /* Width of character */
} vChar;

vChar valid_chars[255];

static SDL_Surface *font = NULL;
int font_init = 0;

#define FONT_FILE "../fonts.bmp"

void initText ()
{
	int i;
	static unsigned char chars[] = TEXT_CHARS;
	static int pos_x[] = TEXT_POS;
	static unsigned char fonts[] = TEXT_IMG;
				
	memset(valid_chars, 0, sizeof(valid_chars));

	i = 0;
	while (chars[i] != '\0') {
		valid_chars[(unsigned char)chars[i]].is_valid = 1;
		valid_chars[(unsigned char)chars[i]].x = pos_x[i];
		valid_chars[(unsigned char)chars[i]].w = pos_x[i+1] - pos_x[i];
		i++;
	}

	/* Load Texture */
	font = SDL_LoadBMP (FONT_FILE);
	if (!font) {
		fprintf (stderr, "CAN'T LOAD " FONT_FILE "!\n");
		exit(1);
	}
	font_init = 1;
}

/* Use vararg's too?? */
unsigned int outputGetTextWidth (const char *txt)
{
	const unsigned char *s;
	unsigned int l = 0;

	s = (unsigned char*)txt;
	while ((*s) != '\0') {
		if (valid_chars[*s].is_valid) {
			l += valid_chars[*s].w;
		}
		s++;
	}

	return l;
}

unsigned int outputGetTextHeight (const char *txt)
{
	if (!font) return 0;
	
	return font->h;
}

static char messageBuffer[512]="";
static unsigned int messageFrames=0;
	
/* If vsnprintf is allowed in the standard use it */													
#if defined __USE_BSD || defined __USE_ISOC99 || defined __USE_UNIX98
/* ISO/IEC 9899:1999 */
#define fillBuffer(buffer,fmt,ap) va_start (ap, fmt);\
 vsnprintf (buffer, sizeof(buffer), fmt, ap);\
 va_end (ap)
#else
/* ANSI X3.159-1989 (`ANSI C') and ISO/IEC 9899:1999 (`ISO C99') */
#define fillBuffer(buffer,fmt,ap) va_start (ap, fmt);\
 vsprintf (buffer, fmt, ap);\
 va_end (ap)
#endif

/* Text Primitives*/
void outputsDrawText(int x, int y, const char *buffer)
{
	unsigned char *s;
	SDL_Rect from, to;
	int current_x;

	if (!font_init) {
		fprintf(stderr, "Font engine was not initialized!!. Maybe forgot call initText!\n");
		return;
	}

	s = (unsigned char*)buffer;
	current_x = x;
	while ((*s) != '\0') {
		if (valid_chars[*s].is_valid) {
			from.x = valid_chars[*s].x;
			from.y = 0;
			from.w = valid_chars[*s].w;
			from.h = CHAR_SIZE;

			to.x = current_x;
			to.y = y;
			to.w = from.w;
			to.h = from.h;
			SDL_BlitSurface (font, &from, screen, &to);

			current_x += valid_chars[*s].w;
		}
		s++;
	}
}

void outputDrawText(int x, int y, const char *fmt, ...)
{
	va_list ap;
	char buffer[512];
	fillBuffer(buffer,fmt,ap);
	outputsDrawText(x,y,buffer);
}

void outputDrawMessage(int frames, const char* fmt, ...)
{
	va_list ap;
	fillBuffer(messageBuffer,fmt,ap);
	if (frames) frames++; /* if not infinite, increase by one */
	messageFrames=frames;
}

void outputClearLastMessage()
{
	messageBuffer[0]=0;
}

/* End of text functions */

/* Sound */
void outputPlayWav(tMemory sound) {} /* Starts the reproduction of the sample and returns */
void outputPlayMid(tMemory music) {} /* Starts the music reproduction and returns */

/* Graphics */

/* Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!*/
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y*surface->pitch + x*bpp;

	*p = pixel;
}

typedef struct {
	int bottom;
	int left;
	SDL_Surface* surface;
} tSurface;


/* Graphics: Primitives for resources module */
void*
outputLoadBitmap(const unsigned char* data, int size, 
		const tPalette palette, int h, int w, int invert, 
		int firstColorTransparent, int bottom, int left)
{
 /* Returns an abstract object allocated in memory using the data 
  * information ti build it invert is 0 when no invertion is needed and 
  * non-zero when an inversion is performed	*/

	tSurface* loadedSurface;
	int i,j;
	SDL_Color* colors;

	loadedSurface=(tSurface*)malloc(sizeof(tSurface));
	colors=(SDL_Color*)malloc(sizeof(SDL_Color)*palette.colors);

	/* Fill colors with color information */
	for(i=0;i<palette.colors;i++) {
		colors[i].r=(palette.color[i].r<<2);
		colors[i].g=(palette.color[i].g<<2);
		colors[i].b=(palette.color[i].b<<2);
	}

	loadedSurface->left=left;
	loadedSurface->bottom=bottom;
	
	
	loadedSurface->surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_HWPALETTE, w, h, 8, 0, 0, 0, 0); /* TODO: bugfix: w is un bytes not in pixels */
	SDL_SetColorKey(loadedSurface->surface, SDL_SRCCOLORKEY, 0);
	if (!loadedSurface->surface) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		free(colors);
		return NULL;
	}
	SDL_SetPalette(loadedSurface->surface, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, palette.colors);

	/* Lock the surface for direct access to the pixels */
	if (SDL_MUSTLOCK(loadedSurface->surface)) {
		if (SDL_LockSurface(loadedSurface->surface) < 0) {
			fprintf(stderr, "Can't lock surface: %s\n", SDL_GetError());
			exit(1);
		}
	}
	
	/* Write pixels */
	if (palette.colors==16) {
		w = (w + 1) / 2;
		if (!invert) {
			for (i = 0; i < w; i++) {
				for (j = 0; j < loadedSurface->surface->h; j++) {
					putpixel(loadedSurface->surface, i<<1, j, (data[i+j*w])>>4);
					putpixel(loadedSurface->surface, (i<<1)+1, j, (data[i+j*w])&0x0f);
				}
			}
		} else {
			int serialized=loadedSurface->surface->w&1;
			for (i = 0; i < w; i++) {
				for (j = 0; j < loadedSurface->surface->h; j++) {
					if (i) putpixel(loadedSurface->surface, (i<<1)-serialized, j, (data[w-1-i+j*w])&0x0f);
					putpixel(loadedSurface->surface, (i<<1)+1-serialized, j, (data[w-1-i+j*w])>>4);
				}
			}
		}
	} else {
		w = (w + 7) / 8;
		if (!invert) {
			for (i = 0; i < w; i++) {
				for (j = 0; j < loadedSurface->surface->h; j++) {
					putpixel(loadedSurface->surface, (i<<3)  , j, (data[i+j*w]>>7)&1);
					putpixel(loadedSurface->surface, (i<<3)+1, j, (data[i+j*w]>>6)&1);
					putpixel(loadedSurface->surface, (i<<3)+2, j, (data[i+j*w]>>5)&1);
					putpixel(loadedSurface->surface, (i<<3)+3, j, (data[i+j*w]>>4)&1);
					putpixel(loadedSurface->surface, (i<<3)+4, j, (data[i+j*w]>>3)&1);
					putpixel(loadedSurface->surface, (i<<3)+5, j, (data[i+j*w]>>2)&1);
					putpixel(loadedSurface->surface, (i<<3)+6, j, (data[i+j*w]>>1)&1);
					putpixel(loadedSurface->surface, (i<<3)+7, j, (data[i+j*w]   )&1);
				}
			}
		} /* monochrome+inversible are not coded (for the moment there is no need) */
	}
	
	if (SDL_MUSTLOCK(loadedSurface->surface)) {
		SDL_UnlockSurface(loadedSurface->surface);
	}

	free(colors);
	return (void*)loadedSurface;
}

/* Frees the abstract object created by the loadBitmap function */
void outputFreeBitmap(void* image) {
	if (image) {
		SDL_FreeSurface(((tSurface*)image)->surface);
		free(image);
	}
}

/* Graphics: Primitives for the kernel */
void outputDrawBitmap(void* image, int x, int y) {
	/* Draws an abstract image */
	tSurface* img=(tSurface*)image;
	SDL_Surface *s = img->surface;
	SDL_Rect dest;
	dest.x = x+img->left;
	dest.y = y-s->h-img->bottom;
	dest.w = s->w;
	dest.h = s->h;
	if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	SDL_BlitSurface(s, NULL, screen, &dest);
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
}

void outputClearScreen()
{
	SDL_FillRect(screen, NULL, 0);
}

void outputUpdateScreen() 
{
	/* check out for messages */
	if (messageFrames==1) outputClearLastMessage();
	if (messageFrames) messageFrames--;
	outputsDrawText((DEF_SCREEN_WIDTH-outputGetTextWidth(messageBuffer))/2,DEF_SCREEN_HEIGHT-CHAR_SIZE,messageBuffer);

	SDL_Flip(screen);
}

/* Initialization */
int outputInit() 
{
	int i;
	SDL_Color colors[256];
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
	SDL_WM_SetCaption("FreePrince " FP_VERSION,NULL);
	atexit(outputStop);

	/* Fill colors with color information */
	for(i=0;i<256;i++) {
		colors[i].r=255-i;
		colors[i].g=255-i;
		colors[i].b=255-i;
	}
	screen = SDL_SetVideoMode(DEF_SCREEN_WIDTH, DEF_SCREEN_HEIGHT, 0, SDL_SWSURFACE|SDL_HWPALETTE);
	if (!screen) return -1;
	/*SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);*/
	initText();
	return 0;
}

int outputGetScreenHeight()
{
	return DEF_SCREEN_HEIGHT;
}

int outputGetScreenWidth()
{
	return DEF_SCREEN_WIDTH;
}

int outputGetHeight(void* image)
{
	register tSurface* img=(tSurface*)image;
	return img->surface->h+img->bottom;
}

int outputGetWidth(void* image)
{
	register tSurface* img=(tSurface*)image;
	return img->surface->w;
}

/* Finish all output modes, including the screen mode */
void outputStop()
{
	SDL_Quit();
}

