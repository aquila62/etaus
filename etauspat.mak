#!/usr/bin/make

# etauspat.mak - Compile Program etauspat.c
# Version 0.1.0
# Copyright (C) 2016   aquila62 at github.com

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to:

# 	Free Software Foundation, Inc.
# 	59 Temple Place - Suite 330
# 	Boston, MA  02111-1307, USA.

# Commands when running etauspat:
# r = reset
# q = quit

OBJ=etauspat.o \
	etausinit.o \
	etaus.o \
	etausint.o

CC=gcc

CFLAGS=-c -Wall -O2

LDFLAGS=-lncurses -lm

etauspat:		$(OBJ)
	$(CC) -Wall -O2 $(OBJ) -o etauspat $(LDFLAGS)

etauspat.o:		etauspat.c
	$(CC) $(CFLAGS) etauspat.c

etausinit.o:		etausinit.c
	$(CC) $(CFLAGS) etausinit.c

etaus.o:		etaus.c
	$(CC) $(CFLAGS) etaus.c

etausint.o:		etausint.c
	$(CC) $(CFLAGS) etausint.c

clean:
	rm -f $(OBJ) etauspat
