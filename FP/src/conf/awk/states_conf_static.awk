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

# states_conf_static.awk: FreePrince : states.conf parser
# ŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻ
# Copyright 2004, 2003 Princed Development Team
#  Created: 15 Oct 2004
#
#  Author: Enrique Calot <ecalot.cod@princed.com.ar>
#
# Note:
#  DO NOT remove this copyright notice
#

BEGIN {
	currentCondition=-1
	currentAction=0
	printf("#define STATES_MOVETYPES_RELATIVE 0\n")
	printf("#define STATES_MOVETYPES_ABSOLUTEFORWARD 1\n\n")
	printf("#define STATES_MOVETYPES_RELATIVETURN 2\n\n")
	printf("#define STATES_CONDITIONS {\\\n")
	tmp="conf/statesproperties.conf"
	while ((getline line < tmp) > 0) {
		gsub(/[	 ]+/, "-",line)
		m=index(line,"-")
		if (m) {
			word=substr(line,0,m)
			number=substr(line,m+1)
			defines[word]=number
		}
	}
	close(tmp)
	currentAnimation=0
	greatestLevel=-1
	first=0
}

#3 tabs (options values)
/^\t\t\t[^\t# ].*$/ {
				if (listType == "next") {
					nextState=tolower($1)
				} else if (listType == "steps") {
					moveOffset=$2
					moveType=tolower($1)
				} else if (listType == "conditions") {
					if ($1=="none") next
					if (oldType != listType ) {
						oldType=listType
						currentCondition++
						printf("\t{esLast,0}, /* condition number %d */\\\n",currentCondition)
						conditions=currentCondition+1
					}
					currentCondition++
					if ($2!=sprintf("%d",$2)) {
						if (defines[$2]) {
							result=sprintf("STATES_COND_%s /* %d */",$2,defines[$2])
						} else {
							if ($2) {
								printf("Parsing error in states.conf: Condition modifier '%s' not recognized on uncommented line %d.\n",$2,NR)>"/dev/stderr"
								exit 22
							}
							result=0
						}
					} else {
						result=$2
					}
					printf("\t{es%s,%s}, /* condition number %d */\\\n",$1,result,currentCondition)
				} else if (listType == "animation") {
					if (match($1,/^[0-9]+-[0-9]*$/)) {
						split($1,a,"-")
						for (g=a[1];g<=a[2];g++) {
							arrayAnimation[currentAnimation,"frame"]=g
							arrayAnimation[currentAnimation,"flags"]=$2
							currentAnimation++
						}
					} else {
						arrayAnimation[currentAnimation,"frame"]=$1
						arrayAnimation[currentAnimation,"flags"]=$2
						currentAnimation++
					}
				} else if (listType == "level") {
					if (arrayLevel[$1]) {
						printf("Parsing error in states.conf: Redeclaration of level '%d' on line %d.\n",$1,NR)>"/dev/stderr"
						exit 23
					} else {
						arrayLevel[$1]=currentAction+1
						if ($1>greatestLevel) {
							greatestLevel=$1
						}
					}
				} else if (listType == "guardskill") {
					if (arrayGuard[$1]) {
						printf("Parsing error in states.conf: Redeclaration of guard skill '%d' on line %d.\n",$1,NR)>"/dev/stderr"
						exit 25
					} else {
						arrayGuard[$1]=currentAction+1
						if ($1>greatestSkill) {
							greatestSkill=$1
						}
					}
				}
}

#2 tabs (action options)
/^\t\t[^\t# ].*$/ {
	oldType=listType
	listType=tolower($1)
}
#1 tab (action)
/^\t[^\t# ].*$/ {
			if (first) {
				actionArray[currentAction,"description"]=rememberAction
				actionArray[currentAction,"isFirstInState"]=currentState
				actionArray[currentAction,"animationStart"]=startAnimation
				actionArray[currentAction,"animationSize"]=currentAnimation-startAnimation
				actionArray[currentAction,"conditionStart"]=conditions
				actionArray[currentAction,"nextState"]=nextState
				actionArray[currentAction,"moveType"]=moveType
				actionArray[currentAction,"moveOffset"]=moveOffset
				actionArray[currentAction,"lastComma"]=","
				currentAction++
				currentState=""
			} else {
				first=1
			}

			startAnimation=currentAnimation
			listType=""
			conditions=0
			if (tolower($1) != "action") {
				printf("Error! \"%s\" should be an action.\n",$0)>"/dev/stderr"
			}
			$1=""
			rememberAction=substr($0,2)
}

#no tabs (states)
/^[^\t# ].*$/ {
		listType=""
		if ($1 ~ /:$/ ) $1=substr($1,1,length($1)-1)
		priorState=currentState #TODO fix that!!!
		currentState=tolower($1)
		stateList[currentState]=currentAction+1
}

END {
	actionArray[currentAction,"description"]=rememberAction
	actionArray[currentAction,"isFirstInState"]=currentState
	actionArray[currentAction,"animationStart"]=startAnimation
	actionArray[currentAction,"animationSize"]=currentAnimation-startAnimation
	actionArray[currentAction,"conditionStart"]=conditions
	actionArray[currentAction,"nextState"]=nextState
	actionArray[currentAction,"moveType"]=moveType
	actionArray[currentAction,"moveOffset"]=moveOffset
	actionArray[currentAction,"lastComma"]=""
	printf("\t{esLast,0} /* the end */\\\n}\n\n#define STATES_ACTIONS {\\\n")

	for (i=0;i<=currentAction;i++) {
		nextStateId=stateList[actionArray[i,"nextState"]]
		#print comments
		printf("\t/* Action: %s (%d) \\\n",\
			actionArray[i,"description"],\
			i\
		)
		if (actionArray[i,"isFirstInState"]) {
			printf("\t * State: %s (%d) \\\n",\
				actionArray[i,"isFirstInState"],\
				i\
			)
		}
		printf("\t * Animations: animStart=%d, animSize=%d \\\n",\
			actionArray[i,"animationStart"],\
			actionArray[i,"animationSize"]\
		)
		printf("\t * Conditionals: conditionId=%d, nextStateId=%d (%s) \\\n",\
			actionArray[i,"conditionStart"],\
			nextStateId,\
			actionArray[i,"nextState"]\
		)
		printf("\t * Movements: moveType=%s, moveOffset=%d */ \\\n",\
			actionArray[i,"moveType"],\
			actionArray[i,"moveOffset"]\
		)
		#print array
		printf("\t\t{%d,STATES_MOVETYPES_%s,%d,%d,%d,%d}%s\\\n",\
			actionArray[i,"conditionStart"],\
			toupper(actionArray[i,"moveType"]),\
			actionArray[i,"moveOffset"],\
			nextStateId,\
			actionArray[i,"animationStart"],\
			actionArray[i,"animationSize"],\
			actionArray[i,"lastComma"]\
		)
	}

	printf("}\n\n#define STATES_ANIMATIONS {\\\n\t")
	coma=""
	for (i=0;i<currentAnimation;i++) {
		flags=arrayAnimation[i,"flags"]
		if (flags!="") {
			coma2=""
			flagmask=""
			for (j=1;j<=length(flags);j++) {
				c=substr(flags,j,1)
				flagmask=sprintf("%s%sSTATES_FLAG_%s",flagmask,coma2,toupper(c))
				coma2="|"
			}
		} else {
			flagmask="0"
		}
		printf "%s%d,%s",coma,arrayAnimation[i,"frame"],flagmask
		if (i%10==9) printf("\\\n\t")
		coma=","
	}

	printf("\\\n}\n\n#define STATES_LEVELS {")
	coma=""
	for (i=0;i<=greatestLevel;i++) {
		if (!arrayLevel[i]) {
			printf("Parsing error in states.conf: Level number %d is defined but level %d is not.\n",greatestLevel,i)>"/dev/stderr"
			exit 24
		}
		printf "%s%s",coma,arrayLevel[i]-1
		coma=","
	}
	printf("}\n\n#define STATES_GUARDS {")
	coma=""
	for (i=0;i<=greatestGuard;i++) {
		if (!arrayGuard[i]) {
			printf("Parsing error in states.conf: Guard skill %d is defined but skill %d is not.\n",greatestGuard,i)>"/dev/stderr"
			exit 26
		}
		printf "%s%s",coma,arrayGuard[i]-1
		coma=","
	}
printf("}\n")

}

