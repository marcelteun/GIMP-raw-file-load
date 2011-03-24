# Copyright (C) 2011 Marcel Tunnissen
#
# License: GNU Public License version 2
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not,
# check at http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
# or write to the Free Software Foundation,

GIMPTOOL ?= gimptool-2.0
CC ?= gcc
CFLAGS := $(shell $(GIMPTOOL) --cflags)
LDLIBS := $(shell $(GIMPTOOL) --libs)
INSTALL_DIR := $(shell $(GIMPTOOL)  --gimpplugindir)

# To make it easy, for such a small project I don't think it is needed to
# create include dependencies files...
# Though there is a risk that updating the Gimp installation will not trigger
# rebuilt.
INCLUDES := $(shell ls *.h)

# my extra flags
CFLAGS += -Wall
OBJECTS :=			\
	file-raw-load-gtk.o	\
	file-raw-load.o
PLUG_INS :=			\
	file-565		\
	file-rgb-888

.PHONY: all
.PHONY: install
.PHONY: uninstall
.PHONY: install-admin
.PHONY: uninstall-admin

all: $(PLUG_INS)

%.o: %.c $(INCLUDES)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(PLUG_INS): %: %.o $(OBJECTS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

install: $(PLUG_INS)
	for OBJ in $^; do $(GIMPTOOL) --install-bin $$OBJ; done

uninstall:
	for OBJ in $(PLUG_INS); do $(GIMPTOOL) --uninstall-bin $$OBJ; done

install-admin: $(PLUG_INS)
	for OBJ in $^; do $(GIMPTOOL) --install-admin-bin $$OBJ; done

uninstall-admin:
	for OBJ in $(PLUG_INS); do $(GIMPTOOL) --uninstall-admin-bin $$OBJ; done

clean:
	rm -f *.o $(PLUG_INS)
