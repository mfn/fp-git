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

# room_conf.awk: FreePrince : room.conf parser
# �������������
# Copyright 2005 Princed Development Team
#  Created: 5 Mar 2005
#
#  Author: Enrique Calot <ecalot.cod@princed.com.ar>
#
# Note:
#  DO NOT remove this copyright notice
#


/[\t ]*XX .*/ {
	if (inIf) printf("}\n");
	printf("\n#define draw%s(x,y,left,tile,right,dleft) \\\n",$2)
	inIf=0
}

/[\t ]*if / {
	if (inIf) printf("}\\\n");
	printf("%s {\\\n",$0)
	inIf=1
}

/[\t ]*draw/ {
	printf("%s\\\n",$0);
}

END {
	if (inIf) printf("}\\\n");
	print
}
