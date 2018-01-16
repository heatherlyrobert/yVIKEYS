#================================[[ beg-code ]]================================#



#===(manditory)============================================#
# basename of executable, header, and all code files
NAME_BASE  = yVIKEYS
# additional standard and outside libraries
LIB_STD    = 
# all heatherly libraries used, debug versions whenever available
LIB_MINE   = -lySTR_debug -lyFONT_debug -lGL
# directory for production code, no trailing slash
INST_DIR   = 



#===(optional)=============================================#
# extra include directories required
INC_MINE   = 
# utilities generated, separate from main program
NAME_UTIL  = 
# libraries only for the utilities
LIB_UTIL   = 



#===(post-processing)======================================#
# create a rule for...
#install_prep       :
#remove_prep        :



#===(master template)======================================#
include /usr/local/sbin/make_program



#===(post-processing)======================================#
# create a rule for...
#install_post       :
#remove_post        :



#================================[[ end-code ]]================================#
#
#
#
##*============================----beg-of-source---============================*#
#
#
#
##*---(current variables)--------------*#
#BASE    = yVIKEYS
#DEBUG   = ${BASE}_debug
#UNIT    = ${BASE}_unit
##*---(dirs and docs)------------------*#
#HDIR    = /home/system/yVIKEYS.vi_keys_library
#IDIR    = 
#MNUM    = 3
#MDIR    = /usr/share/man/man${MNUM}
#MBASE   = ${BASE}.${MNUM}
##*---(compilier variables)------------*#
## must have "-x c" on gcc line so stripped files work with alternate extensions
#COMP    = gcc -c -std=gnu89 -x c -g -pg -Wall -Wextra
#INCS    = -I/usr/local/include 
#LINK    = gcc
#LIBDIR  = -L/usr/local/lib
#LIBS    = ${LIBDIR}    
#LIBD    = ${LIBS}      -lyLOG
#LIBU    = ${LIBD}      -lyUNIT    -lyVAR_debug
##*---(file lists)---------------------*#
#HEADS   = ${BASE}.h        ${BASE}_priv.h
#OBJS    = ${BASE}_keys.os  ${BASE}_mode.o   ${BASE}_speed.os  ${BASE}_scale.os
#OBJD    = ${BASE}_keys.o   ${BASE}_mode.o   ${BASE}_speed.o   ${BASE}_scale.o
#OBJU    = ${BASE}_keys.o   ${BASE}_mode.o   ${BASE}_speed.o   ${BASE}_scale.o    ${UNIT}.o
##*---(make variables)-----------------*#
#COPY    = cp -f
#CLEAN   = rm -f
#PRINT   = @printf
#STRIP   = @grep -v -e " DEBUG_" -e " yLOG_" 
#
#
#
##*---(executables)--------------------*#
#all                : ${DEBUG} ${BASE} ${UNIT}
#
#${BASE}            : ${OBJS}
#	${LINK}  -shared -Wl,-soname,lib${BASE}.so.1   ${LIBS}  -o lib${BASE}.so.1.0   ${OBJS}
#	ar       rcs  lib${BASE}.a   ${OBJS}
#
#${DEBUG}           : ${OBJD}
#	${LINK}  -shared -Wl,-soname,lib${DEBUG}.so.1  ${LIBD}  -o lib${DEBUG}.so.1.0  ${OBJD}
#	ar       rcs  lib${DEBUG}.a  ${OBJD}
#
#${UNIT}            : ${OBJU}
#	${LINK}  -o ${UNIT}        ${OBJU}   ${LIBU}
#
#
#
##*---(components)---------------------*#
#${BASE}.o          : ${HEADS}       ${BASE}.c
#	${COMP}  -fPIC  ${BASE}.c                                ${INC}
#	${STRIP}        ${BASE}.c           > ${BASE}.cs
#	${COMP}  -fPIC  ${BASE}.cs         -o ${BASE}.os         ${INC}
#
#${BASE}_keys.o     : ${HEADS}       ${BASE}_keys.c
#	${COMP}  -fPIC  ${BASE}_keys.c                           ${INC}
#	${STRIP}        ${BASE}_keys.c      > ${BASE}_keys.cs
#	${COMP}  -fPIC  ${BASE}_keys.cs    -o ${BASE}_keys.os    ${INC}
#
#${BASE}_mode.o     : ${HEADS}       ${BASE}_mode.c
#	${COMP}  -fPIC  ${BASE}_mode.c                           ${INC}
#	${STRIP}        ${BASE}_mode.c      > ${BASE}_mode.cs
#	${COMP}  -fPIC  ${BASE}_mode.cs    -o ${BASE}_mode.os    ${INC}
#
#${BASE}_speed.o    : ${HEADS}       ${BASE}_speed.c
#	${COMP}  -fPIC  ${BASE}_speed.c                          ${INC}
#	${STRIP}        ${BASE}_speed.c     > ${BASE}_speed.cs
#	${COMP}  -fPIC  ${BASE}_speed.cs   -o ${BASE}_speed.os   ${INC}
#
#${BASE}_scale.o    : ${HEADS}       ${BASE}_scale.c
#	${COMP}  -fPIC  ${BASE}_scale.c                          ${INC}
#	${STRIP}        ${BASE}_scale.c     > ${BASE}_scale.cs
#	${COMP}  -fPIC  ${BASE}_scale.cs   -o ${BASE}_scale.os   ${INC}
#
#${UNIT}.o          : ${HEADS} ${BASE}.unit
#	koios    ${BASE}
#	${COMP}  ${UNIT}.c  ${INCS}
#
#
#
##*---(housecleaning)------------------*#
#clean              :
#	${PRINT}  "\n--------------------------------------\n"
#	${PRINT}  "cleaning out local object, backup, and temp files\n"
#	${CLEAN} lib${BASE}.so.1
#	${CLEAN} lib${BASE}.so.1.0
#	${CLEAN} lib${DEBUG}.so.1
#	${CLEAN} lib${DEBUG}.so.1.0
#	${CLEAN} *.o
#	${CLEAN} *.cs
#	${CLEAN} *.os
#	${CLEAN} *~
#	${CLEAN} temp*
#	${CLEAN} ${BASE}_unit.c
#
#bigclean           :
#	${PRINT}  "\n--------------------------------------\n"
#	${PRINT}  "clean out local swap files\n"
#	${CLEAN}  '.'*.sw?
#
#remove             :
#	${PRINT}  "\n--------------------------------------\n"
#	${PRINT}  "remove ${BASE} from production\n"
#	_lib      -d  ${BASE}
#	_lib      -d  ${DEBUG}
#	ldconfig
#
#install            :
#	#---(production version)--------------#
#	sha1sum   lib${BASE}.so.1.0
#	_lib      -s ${BASE}
#	ldconfig
#	sha1sum   lib${BASE}.a
#	_lib      -a ${BASE}
#	ldconfig
#	#---(debug version)-------------------#
#	sha1sum   lib${DEBUG}.so.1.0
#	_lib      -S ${DEBUG}
#	ldconfig
#	sha1sum   lib${DEBUG}.a
#	_lib      -A ${DEBUG}
#	ldconfig
#	#---(documentation)-------------------#
#	rm -f     ${MDIR}/${MBASE}.bz2
#	cp -f     ${BASE}.3    ${MDIR}
#	bzip2     ${MDIR}/${MBASE}
#	chmod     0644  ${MDIR}/${MBASE}.bz2
#
#
#
##*============================----end-of-source---============================*#
