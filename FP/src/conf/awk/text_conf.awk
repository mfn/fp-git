#   Princed V3 - Prince of Persia Level Editor for PC Version
#   Copyright (C) 2003 Princed Development Team
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#   The authors of this program may be contacted at http://forum.princed.com.ar

# text_conf.awk: FreePrince : alphabet.conf parser
# ����������������������
# Copyright 2005 Princed Development Team
#  Created: 25 Jan 2005
#
#  Author: Enrique Calot <ecalot.cod@princed.com.ar>
#
# Note:
#  DO NOT remove this copyright notice
#

BEGIN {
	#constants
	waitingForChar=-2
	waitingForTop=-1

	#initialize
	state=waitingForChar
	maxSize=0
	k=0
	for (i=0;i<256;i++) {
		chr[i]=sprintf("%c",i)
		ord[chr[i]]=i
	}
	offset=0

	#default
	space=3
	
}

##########################
# PART 1                 #
# Parse file into memory #
##########################

/^Space [0-9]+$/ {
	space=$2
}

/^Character '.':$/ {
	if (state==waitingForChar) {
		c=ord[substr($2,2,1)]
		state=waitingForTop
		k++
	} else {
		print "Not waiting for character"
		exit 1
	}
}

/^\/[-]*\\$/ { #beggining of an image
	if (state==waitingForTop) {
		len=length($1)
		state=0
	} else {
		print "Not waiting for a new image"
		exit 1
	}
}

/^\\[-]*\/$/ {
	if (state>0) {
		l=length($0)
		if (l!=len) {
			print "Error: the image len is not valid"
			exit 1
		}
		offset+=l-2;
		if (maxSize<state) maxSize=state
		state=waitingForChar
		list[k]=c
		offs[k]=offset
	} else {
		print "Not waiting for and end of image"
		exit 1
	}
}


/^\|[ O]*\|$/ {
	if (state>=0) {
		l=length($0)
		if (l!=len) {
			print "Error: the image len is not valid"
			exit 1
		}
		for (i=0;i<l-2;i++) {
			if (substr($0,2+i,1)=="O") {
				show=1
			} else {
				show=0
			}
			arr[state,offset+i]=show
		}
		state++
	} else {
		print "Not waiting for image"
		exit 1
	}
}

END {
	if (state!=waitingForChar) {
		print "End reched with an unfinished image"
		exit 1
	}
	
	printf("#define TEXT_CHARS {\\\n\t")
	for (i=1;i<=k;i++) {
		printf("%d,",list[i])
	}
	printf("32,0}\n")

	offset+=space
	printf("#define TEXT_POS {\\\n\t0,")
	for (i=1;i<=k;i++) {
		printf("%d,",offs[i])
	}
	printf("%d}\n",offset)

	img=0
	printf("#define TEXT_IMG {\\\n\t")
	coma=""
	for (j=0;j<maxSize;j++) {
		for (i=0;i<offset;i+=8) {
			value=(arr[j,i]*(2^7))+(arr[j,i+1]*(2^6))+(arr[j,i+2]*(2^5))+(arr[j,i+3]*(2^4))+(arr[j,i+4]*(2^3))+(arr[j,i+5]*(2^2))+(arr[j,i+6]*(2^1))+(arr[j,i+7]*(2^0))
			printf("%s%s",coma,value)
			coma=","
			img++
		}
	}
	printf("}\n")
	printf("#define TEXT_IMG_SIZE %d\n",img)
}
