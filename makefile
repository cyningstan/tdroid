# ======================================================================
# Team Droid: Jam Edition
# A programming puzzle game with cute robots.
#
# Copyright (C) Damian Gareth Walker 2022.
# Makefile for building using OpenWatcom C.
#

# Don't prompt for deleting after errors.
.ERASE

# Paths (this project)
SRCDIR = src
INCDIR = inc
ASSDIR = assets
DOCDIR = doc
OBJDIR = obj
LIBDIR = lib
BINDIR = bin
LEVDIR = levelpak
TGTDIR = tdroid

# Paths (CGALIB)
CGADIR = cgalib
CGALIB = $(CGADIR)\cgalib
CGAINC = $(CGADIR)\inc

# Paths (KEYLIB)
KEYDIR = keylib
KEYLIB = keylib\keylib
KEYINC = keylib\inc

# Paths (SPKLIB)
SPKDIR = spklib
SPKLIB = spklib\spklib
SPKINC = spklib\inc

# Tools
CC = wcc
LD = wcl
AR = wlib
CP = copy
TOUCH = $(BINDIR)\touch.exe
MKASSETS = $(BINDIR)\mkassets
MKCAMP = $(CAMDIR)\mkcamp $(CAMDIR)\barren
RM = del

# Compiler flags
CCOPTS = -q -0 -w4 -ml -e1 -we &
	-i=$(INCDIR) -i=$(CGAINC) -i=$(KEYINC) -i=$(SPKINC)
LDOPTS = -q

#
# Main Targets
#

# All the things
ALL : &
	$(TGTDIR)\tdroid.exe &
	$(TGTDIR)\tdroid.dat &
	$(TGTDIR)\tdroid.lev

# Main asset file
$(TGTDIR)\tdroid.dat : &
	$(ASSDIR)\tdroid0.pic &
	$(ASSDIR)\tdroid1.pic &
	$(ASSDIR)\tdroid2.pic &
	$(BINDIR)\mkassets.exe
	$(MKASSETS)

# Default level pack
$(TGTDIR)\tdroid.lev : &
	$(LEVDIR)\tdroid.lvi &
	$(LEVDIR)\mklevels.exe
	cd $(LEVDIR)
	mklevels tdroid
	cd ..
	$(CP) $(LEVDIR)\tdroid.lev $(TGTDIR)\tdroid.lev

# Main program binary
$(TGTDIR)\tdroid.exe : &
	$(OBJDIR)\tdroid.obj &
	$(OBJDIR)\controls.obj &
	$(OBJDIR)\display.obj &
	$(OBJDIR)\fatal.obj &
	$(OBJDIR)\config.obj &
	$(OBJDIR)\game.obj &
	$(OBJDIR)\robot.obj &
	$(OBJDIR)\levelpak.obj &
	$(OBJDIR)\level.obj &
	$(OBJDIR)\cell.obj &
	$(OBJDIR)\item.obj &
	$(OBJDIR)\action.obj &
	$(OBJDIR)\scoretbl.obj &
	$(OBJDIR)\score.obj &
	$(OBJDIR)\utils.obj &
	$(OBJDIR)\timer.obj &
	$(OBJDIR)\uiscreen.obj &
	$(OBJDIR)\uinewgam.obj &
	$(OBJDIR)\uiscore.obj &
	$(OBJDIR)\uideploy.obj &
	$(OBJDIR)\uiprog.obj &
	$(OBJDIR)\uiaction.obj &
	$(OBJDIR)\uicomplt.obj &
	$(OBJDIR)\uivictor.obj &
	$(CGALIB)\cga-ml.lib &
	$(SPKLIB)\spk-ml.lib &
	$(KEYLIB)\key-ml.lib
	*$(LD) $(LDOPTS) -fe=$@ $<

# Asset generation binary
$(BINDIR)\mkassets.exe : &
	$(OBJDIR)\mkassets.obj &
	$(OBJDIR)\fatal.obj &
	$(OBJDIR)\robot.obj &
	$(OBJDIR)\utils.obj &
	$(CGALIB)\cga-ml.lib &
	$(SPKLIB)\spk-ml.lib
	*$(LD) $(LDOPTS) -fe=$@ $<

# Level pack generation binary
$(LEVDIR)\mklevels.exe : &
	$(OBJDIR)\mklevels.obj &
	$(OBJDIR)\levelpak.obj &
	$(OBJDIR)\level.obj &
	$(OBJDIR)\cell.obj &
	$(OBJDIR)\item.obj &
	$(OBJDIR)\robot.obj &
	$(OBJDIR)\utils.obj &
	$(OBJDIR)\scoretbl.obj &
	$(OBJDIR)\score.obj &
	$(OBJDIR)\fatal.obj &
	$(CGALIB)\cga-ml.lib
	*$(LD) $(LDOPTS) -fe=$@ $<

# Touch utility
$(BINDIR)\touch.exe : &
	$(OBJDIR)\touch.obj
	*$(LD) $(LDOPTS) -fe=$@ $<

# Required libraries: CGALIB
$(CGALIB)\cga-ml.lib :
	cd cgalib
	wmake
	cd ..

# Required libraries: KEYLIB
$(KEYLIB)\key-ml.lib :
	cd keylib
	wmake
	cd ..

# Required libraries: SPKLIB
$(SPKLIB)\spk-ml.lib :
	cd spklib
	wmake KEYLIB=..\keylib
	cd ..

#
# Object Files
#

# Main program module
$(OBJDIR)\tdroid.obj : &
	$(SRCDIR)\tdroid.c &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\display.h &
	$(INCDIR)\fatal.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\beta.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Asset generator
$(OBJDIR)\mkassets.obj : &
	$(SRCDIR)\mkassets.c &
	$(CGAINC)\cgalib.h &
	$(SPKLIB)\speaker.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\action.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Level pack generator
$(OBJDIR)\mklevels.obj : &
	$(SRCDIR)\mklevels.c &
	$(INCDIR)\levelpak.h &
	$(INCDIR)\level.h &
	$(INCDIR)\item.h &
	$(INCDIR)\action.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Controls module
$(OBJDIR)\controls.obj : &
	$(SRCDIR)\controls.c &
	$(INCDIR)\controls.h &
	$(INCDIR)\fatal.h &
	$(KEYINC)\keylib.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Configuration module
$(OBJDIR)\config.obj : &
	$(SRCDIR)\config.c &
	$(INCDIR)\fatal.h &
	$(INCDIR)\config.h &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Display module
$(OBJDIR)\display.obj : &
	$(SRCDIR)\display.c &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\fatal.h &
	$(INCDIR)\game.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\timer.h &
	$(CGAINC)\cgalib.h &
	$(SPKINC)\speaker.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Fatal error module
$(OBJDIR)\fatal.obj : &
	$(SRCDIR)\fatal.c &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Game module
$(OBJDIR)\game.obj : &
	$(SRCDIR)\game.c &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\game.h &
	$(INCDIR)\config.h &
	$(INCDIR)\levelpak.h &
	$(INCDIR)\level.h &
	$(INCDIR)\item.h &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Robot module
$(OBJDIR)\robot.obj : &
	$(SRCDIR)\robot.c &
	$(INCDIR)\robot.h &
	$(INCDIR)\fatal.h &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Level Pack module
$(OBJDIR)\levelpak.obj : &
	$(SRCDIR)\levelpak.c &
	$(INCDIR)\levelpak.h &
	$(INCDIR)\level.h &
	$(INCDIR)\scoretbl.h &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Level module
$(OBJDIR)\level.obj : &
	$(SRCDIR)\level.c &
	$(INCDIR)\level.h &
	$(INCDIR)\fatal.h &
	$(INCDIR)\cell.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\item.h &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Cell module
$(OBJDIR)\cell.obj : &
	$(SRCDIR)\cell.c &
	$(INCDIR)\cell.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\level.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Item module
$(OBJDIR)\item.obj : &
	$(SRCDIR)\item.c &
	$(INCDIR)\item.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Action module
$(OBJDIR)\action.obj : &
	$(SRCDIR)\action.c &
	$(INCDIR)\action.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\level.h &
	$(INCDIR)\item.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Score table module
$(OBJDIR)/scoretbl.obj : &
	$(SRCDIR)\scoretbl.c &
	$(INCDIR)\scoretbl.h &
	$(INCDIR)\fatal.h &
	$(INCDIR)\score.h &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Score table module
$(OBJDIR)/score.obj : &
	$(SRCDIR)\score.c &
	$(INCDIR)\score.h &
	$(INCDIR)\fatal.h &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# User Interface Module
$(OBJDIR)/uiscreen.obj : &
	$(SRCDIR)\uiscreen.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\fatal.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\timer.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# New Game User Interface Module
$(OBJDIR)/uinewgam.obj : &
	$(SRCDIR)\uinewgam.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\levelpak.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Score Screen User Interface Module
$(OBJDIR)/uiscore.obj : &
	$(SRCDIR)\uiscore.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\scoretbl.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Deployment Screen User Interface Module
$(OBJDIR)/uideploy.obj : &
	$(SRCDIR)\uideploy.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\level.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\item.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Programming Screen User Interface Module
$(OBJDIR)/uiprog.obj : &
	$(SRCDIR)\uiprog.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\level.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\item.h &
	$(INCDIR)\action.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Action Screen User Interface Module
$(OBJDIR)/uiaction.obj : &
	$(SRCDIR)\uiaction.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\level.h &
	$(INCDIR)\robot.h &
	$(INCDIR)\item.h &
	$(INCDIR)\action.h &
	$(INCDIR)\timer.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Level Complete User Interface Module
$(OBJDIR)/uicomplt.obj : &
	$(SRCDIR)\uicomplt.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\scoretbl.h &
	$(INCDIR)\score.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Level Complete User Interface Module
$(OBJDIR)/uivictor.obj : &
	$(SRCDIR)\uivictor.c &
	$(INCDIR)\uiscreen.h &
	$(INCDIR)\tdroid.h &
	$(INCDIR)\display.h &
	$(INCDIR)\controls.h &
	$(INCDIR)\config.h &
	$(INCDIR)\game.h &
	$(INCDIR)\scoretbl.h &
	$(INCDIR)\score.h &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Utils module
$(OBJDIR)\utils.obj : &
	$(SRCDIR)\utils.c &
	$(INCDIR)\utils.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Timer module
$(OBJDIR)\timer.obj : &
	$(SRCDIR)\timer.c &
	$(INCDIR)\timer.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

# Beta Test Hander Module
$(OBJDIR)\beta.obj: &
	$(SRCDIR)\beta.c &
	$(INCDIR)\fatal.h
	*$(CC) $(CCOPTS) -fo=$@ $[@

