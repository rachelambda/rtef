TARGET?=rtef
PREFIX?=/usr
INSTALL_PATH?=$(DESTDIR)$(PREFIX)/bin/$(TARGET)

OBJECTS=

CFLAGS?=-march=native -mtune=generic -O2 -pipe
# Strict warnings
CFLAGS+= \
    -Werror=pedantic \
    -Werror=return-local-addr \
    -Werror=missing-prototypes \
    -Werror=strict-prototypes \
    -Wall \
    -Wextra \
    -std=c99

all: $(TARGET)

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
