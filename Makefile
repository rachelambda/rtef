TARGET?=rtef
PREFIX?=/usr
PROGNAME?=rtef
INSTALL_PATH?=$(DESTDIR)$(PREFIX)/bin/$(TARGET)

OBJECTS=src/global.o src/main.o src/file.o

debug: CFLAGS?=-O0 -g
all: CFLAGS?=-O2 -pipe
# Strict
WFLAGS= \
	-Werror=pedantic \
    -D_POSIX_C_SOURCE=0 \
	-DPROGNAME="\"$(PROGNAME)\"" \
    -std=c99

all: $(TARGET)

debug: $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(INSTALL_PATH)

uninstall: $(TARGET)
	rm -f $(INSTALL_PATH)

clean:
	rm -f $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(WFLAGS) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(WFLAGS) $(CFLAGS) $< -o $@

.PHONY: clean install
