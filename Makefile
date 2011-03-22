TARGET := import-raw
NAME   := raw-file-rgb565-load

.PHONY: all
.PHONY: image
.PHONY: install
.PHONY: uninstall
all: install

image: $(TARGET)

$(TARGET): $(TARGET).c
	gimptool-2.0 --build $(TARGET).c

install:
	gimptool-2.0 --install $(TARGET).c

uninstall:
	gimptool-2.0 --uninstall-bin $(NAME)
