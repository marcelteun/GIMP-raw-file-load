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

# my extra flags
CFLAGS += -Wall
OBJECTS := file-565

.PHONY: all
.PHONY: install
.PHONY: uninstall
.PHONY: install-admin
.PHONY: uninstall-admin

all: $(OBJECTS)

$(OBJECTS): %: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

install: $(OBJECTS)
	for OBJ in $^; do $(GIMPTOOL) --install-bin $$OBJ; done

uninstall:
	for OBJ in $(OBJECTS); do $(GIMPTOOL) --uninstall-bin $$OBJ; done

install-admin: $(OBJECTS)
	for OBJ in $^; do $(GIMPTOOL) --install-admin-bin $$OBJ; done

uninstall-admin:
	for OBJ in $(OBJECTS); do $(GIMPTOOL) --uninstall-admin-bin $$OBJ; done
