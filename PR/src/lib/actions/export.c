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
export.c: Princed Resources : DAT Extractor
��������
 Copyright 2003-2005 Princed Development Team
  Created: 24 Aug 2003

  Author: Enrique Calot <ecalot.cod@princed.com.ar>
  Version: 1.01 (2003-Oct-23)
  Version: 1.20 (2004-Mar-07)
  Version: 1.30 (2004-Mar-15)

 Note:
  DO NOT remove this copyright notice
*/

#include <stdio.h>
#include <string.h>
#include "common.h"

#include "export.h"

#include "autodetect.h"
#include "dat.h"
#include "disk.h"
#include "idlist.h"
#include "memory.h"
#include "pallist.h" 
#include "unknown.h"
#include "palette.h"

#include "bmp.h"
#include "mid.h"
#include "pal.h"
#include "plv.h"
#include "wav.h"

extern FILE* outputStream;

/***************************************************************\
|                  I M P L E M E N T A T I O N                  |
\***************************************************************/

/***************************************************************\
|                    M A I N   E X T R A C T                    |
\***************************************************************/

/*
	Extracts a DAT file
	For parameter documentation, see pr.c
*/

int extract(const char* vFiledat,const char* vDirExt, tResourceList* r, int optionflag, const char* vDatFileName, const char* vDatAuthor,const char* backupExtension,const char* format) {
	char               vFileext[MAX_FILENAME_SIZE];
	int                indexNumber;
	int                ok;
	tImage             image; /* this is used to make a persistent palette */
	unsigned short int numberOfItems;
	tPaletteList       paletteBuffer;
	tResourceId        bufferedPalette={0,"",0};
	tResource          res;
	int                count=0;

	/* Initialize abstract variables to read this new DAT file */
	if ((ok=mReadBeginDatFile(&numberOfItems,vFiledat))) return ok;
	ok=1;

	/* initialize palette buffer */
	paletteBuffer=paletteListCreate();
	/* initialize the default palette */
	image.pal=createPalette(); /* TODO: add the default palette ad 0,"",0 */

	/* main loop */
	for (indexNumber=0;ok&&(indexNumber<numberOfItems);indexNumber++) {
		int ok2=mReadFileInDatFile(&res,indexNumber);
		if (ok2==PR_RESULT_INDEX_NOT_FOUND) return PR_RESULT_ERR_INVALID_DAT; /* Read error */
		if (ok2==PR_RESULT_CHECKSUM_ERROR) fprintf(outputStream,"Warning: Checksum error\n"); /* Warning */
		if (res.id.value==0xFFFF) continue; /* Tammo Jan Bug fix */
		/* add to res more information from the resource list */
		resourceListAddInfo(r,&res);

		if (isInTheItemMatchingList(res.path,res.id)) { /* If the resource was specified do the tasks */
			if ((!res.type)&&(!hasFlag(raw_flag))) res.type=verifyHeader(res.data,res.size);
			if (!(hasFlag(unknown_flag))) { /* If unknown flag is set do nothing but generate the unknown.xml file */
				if (hasFlag(raw_flag)) res.type=0; /* If "extract as raw" is set, type is 0 */
				/*tObject o;*/

				/* get save file name (if unknown document is in the XML) */
				getFileName(vFileext,vDirExt,&res,vFiledat,vDatFileName,optionflag,backupExtension,format);

				/* handle palette linking */
				switch (res.type) {
					case eResTypePop1Palette4bits: { /* save and remember palette file */
						tPaletteListItem e;
						/* Remember the palette for the next images
						 * (because it's more probable to get all the images after its palette) */
						e.bits=readPalette(&e.pal,res.data,res.size);
						applyPalette(&e.pal,&image);
						bufferedPalette=res.id;
						e.id=res.id;
						list_insert(&paletteBuffer,(void*)&e);
					}	break;
					case eResTypeImage: /* save image */
						/* Palette handling */
						if (resourceListCompareId(res.palette,bufferedPalette)) { /* The palette isn't in the buffer */
							tResource otherPalette;
							otherPalette.id=res.palette;
							/* Read the palette and load it into memory */
							if (mReadFileInDatFileId(&otherPalette)==PR_RESULT_SUCCESS) {
								/* All right, it's not so bad, I can handle it! I'll buffer the new palette */
								tPaletteListItem e;
								e.bits=readPalette(&e.pal,otherPalette.data,otherPalette.size);
								applyPalette(&e.pal,&image);
								bufferedPalette=otherPalette.id;
								e.id=res.id;
								list_insert(&paletteBuffer,(void*)&e);
							} /* else, that's bad, I'll have to use the previous palette, even if it is the default */
						} /* else, good, the palette is buffered */
						break;
					default:
						break;
				}
				
				/*o=getObject(&res,&ok);*/
				
				switch (res.type) {
						case eResTypeLevel:
						ok=mFormatExportPlv(res.data,vFileext,res.size,res.number,vDatFileName,res.name,res.desc,vDatAuthor,optionflag,backupExtension);
						break;
					case eResTypeBinary: /* Binary files */
					case eResTypeText: /* Text files */
					case eResTypeRaw: /* Raw files */
						ok=writeData(res.data,1,vFileext,res.size,optionflag,backupExtension); /* Ignore checksum */
						break;
					case eResTypePop1Palette4bits: { /* save and remember palette file */
						tPaletteListItem e;
						/* Remember the palette for the next images
						 * (because it's more probable to get all the images after its palette) */
						e.bits=readPalette(&e.pal,res.data,res.size);
						applyPalette(&e.pal,&image);
						bufferedPalette=res.id;
						e.id=res.id;
						list_insert(&paletteBuffer,(void*)&e);
						/* Export the palette */
						ok=mFormatExportPal(&e.pal,e.bits,vFileext,optionflag,backupExtension);
					}	break;
					case eResTypePcspeaker: /* save pcs file */
					case eResTypeMidi:	/* save midi file */
						ok=mFormatExportMid(res.data,vFileext,res.size,optionflag,backupExtension);
						break;
					case eResTypeWave: /* save wav file */
						ok=mFormatExportWav(res.data,vFileext,res.size,optionflag,backupExtension);
						break;
					case eResTypeImage: /* save image */
						/* Palette handling */
						if (resourceListCompareId(res.palette,bufferedPalette)) { /* The palette isn't in the buffer */
							tResource otherPalette;
							otherPalette.id=res.palette;
							/* Read the palette and load it into memory */
							if (mReadFileInDatFileId(&otherPalette)==PR_RESULT_SUCCESS) {
								/* All right, it's not so bad, I can handle it! I'll buffer the new palette */
								tPaletteListItem e;
								e.bits=readPalette(&e.pal,otherPalette.data,otherPalette.size);
								applyPalette(&e.pal,&image);
								bufferedPalette=otherPalette.id;
								e.id=res.id;
								list_insert(&paletteBuffer,(void*)&e);
							} /* else, that's bad, I'll have to use the previous palette, even if it is the default */
						} /* else, good, the palette is buffered */
						/* Export bitmap */

						ok=mFormatExportBmp(res.data,vFileext,res.size,image,optionflag,backupExtension);

						break;
					default:
						break;
				}
				/* Verbose information */
				if (hasFlag(verbose_flag)) {
					if (ok) {
						fprintf(outputStream,PR_TEXT_EXPORT_WORKING,getFileNameFromPath(vFileext));
					} else {
						fprintf(outputStream,PR_TEXT_EXPORT_ERROR,getFileNameFromPath(vFileext));
					}
				}
				if (ok) count++;
			} else {
				/* If the DAT file is unknown, add it in the XML */
				getFileName(vFileext,vDirExt,&res,vFiledat,vDatFileName,optionflag,backupExtension,format);
			}
		}
	}

	/* Free allocated resources, dynamic strings and the index */
	resourceListDrop(r);
	mReadCloseDatFile();
	list_drop(&paletteBuffer);

	/* Close unknownXML */
	return ok?count:PR_RESULT_ERR_EXTRACTION;
}

