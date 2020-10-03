TARGET?=rtef
PREFIX?=/usr
PROGNAME?=rtef
INSTALL_PATH?=$(DESTDIR)$(PREFIX)/bin/$(TARGET)

OBJECTS=src/global.o src/main.o src/file.o

debug: CFLAGS?=-O0 -g
all: CFLAGS?=-O2 -pipe
# Strict warnings
CFLAGS+= \
    -Werror=pedantic \
    -Werror=return-local-addr \
    -Werror=missing-prototypes \
    -Werror=strict-prototypes \
    -Wall \
    -Wextra \
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
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean install
