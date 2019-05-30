#****************************************************************************
#  Dot directive definition area (usually just suffixes)
#****************************************************************************

.SUFFIXES: .c .obj .dll .csc .sc .h .ih .ph .psc .rc .res

#****************************************************************************
#  Environment Setup for the component(s).
#****************************************************************************

SOMTEMP = .\somtemp
SCPATH  = D:\toolkt20\sc
HPATH   = D:\toolkt20\c\os2h
LIBPATH = D:\toolkt20\os2lib

!if [set SMINCLUDE=.;$(SCPATH);] || \
    [set SMTMP=$(SOMTEMP)] || \
    [set SMEMIT=ih;h;ph;psc;sc;c;def]
!endif

!if [cd $(SOMTEMP)]
!  if [md $(SOMTEMP)]
!    error Error creating $(SOMTEMP) directory
!  endif
!else
!  if [cd ..]
!    error Error could not cd .. from $(SOMTEMP) directory
!  endif
!endif

#
# Compiler/tools Macros
#

CC         = icc /c /Ge- /Gd- /Se /Re /ss /Ms /Gm+ /L+ /Li+ /Ls+ /Lx+ /La+ /Ti+
LINK       = link386
LDFLAGS    = /noi /map /nol /nod /exepack /packcode /packdata /align:16 /information /co
LIBS       = som.lib os2386.lib dde4mbs.lib

#****************************************************************************
# Set up Macros that will contain all the different dependencies for the
# executables and dlls etc. that are generated.
#****************************************************************************

OBJS       = pwFin.obj

#****************************************************************************
#   Setup the inference rules for compiling source code to
#   object code.
#****************************************************************************

.csc.c:
        sc -r $<

.c.obj:
        $(CC) -I$(HPATH) -c $<

all: pwFin.dll

pwFin.ih: $*.csc

pwFin.obj: $*.c $*.ih $*.h  $*.sc

pwFin.dll: $(OBJS) pwFin.res
         $(LINK) $(LDFLAGS) $(OBJS),$@,,$(LIBS),$*;
         rc $*.res $*.dll
         mapsym pwFin.map

pwFin.res: pwFin.rc
         rc -r $*.rc $*.res

