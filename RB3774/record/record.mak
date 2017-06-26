#****************************************************************************
#  Dot directive definition area (usually just suffixes)
#****************************************************************************

.SUFFIXES: .c .obj .dll .csc .sc .h .ih .ph .psc .rc .res

#****************************************************************************
#  Environment Setup for the component(s).
#****************************************************************************

SOMTEMP = .\somtemp
SCPATH  = c:\toolkt20\sc
HPATH   = c:\toolkt20\c\os2h
LIBPATH = c:\toolkt20\os2lib

!if [set SMINCLUDE=.;$(SCPATH);] || \
    [set SMTMP=$(SOMTEMP)] || \
    [set SMEMIT=ih;h;ph;psc;sc;c;def]
!endif

!if [cd $(SOMTEMP)]
!  if [md $(SOMTEMP)]
!    error error creating $(SOMTEMP) directory
!  endif
!else
!  if [cd ..]
!    error - Couldn't cd .. from $(SOMTEMP) directory
!  endif
!endif

#
# Compiler/tools Macros
#

CC         = icc /c /Ge- /Gd- /Se /Re /ss /Mp /Gm+
LINK       = link386
LDFLAGS    = /noi /map /nol /nod  /align:16
#LDFLAGS    = /noi /map /nol /nod /exepack /packcode /packdata /align:16
LIBS       = som.lib DDE4MBS DDE4MBM  os2386

#****************************************************************************
# Set up Macros that will contain all the different dependencies for the
# executables and dlls etc. that are generated.
#****************************************************************************

OBJS       = record.obj

#****************************************************************************
#   Setup the inference rules for compiling source code to
#   object code.
#****************************************************************************

.c.obj:
        $(CC) -I$(HPATH) -c $<

.csc.ih:
.csc.c:
        sc -v -r $*.csc

all: record.dll

#
# Specific Process Tag
#

record.ih:  $*.csc $(HPATH)\wpdataf.h

record.obj: $*.c $*.ih $*.h  $*.sc $(HPATH)\wpdataf.h

record.dll: $(OBJS) record.res
         $(LINK) $(LDFLAGS) $(OBJS),$@,,$(LIBS),$*;
         rc $*.res $*.dll
         mapsym record.map

record.res: record.rc
         rc -r $*.rc $*.res

