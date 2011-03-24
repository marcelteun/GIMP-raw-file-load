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
