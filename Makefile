.POSIX:
.SUFFIXES:
WFLAGS=-Werror=pedantic -D_POSIX_C_SOURCE=0 -DPROGNAME="\"$(PROGNAME)\"" -std=c99
OBJECTS=src/main.o src/file.o src/global.o

rtef: $(OBJECTS)
	$(CC) $(WFLAGS) $(CFLAGS) $(OBJECTS) -o $@

src/main.o: src/main.c src/global.h src/file.h
src/file.o: src/file.c src/global.h
src/global.o: src/global.c src/global.h

.SUFFIXES: .c .o
.c.o:
	$(CC) -c $(WFLAGS) $(CFLAGS) $< -o $@

install: rtef
	install -Dm755 $< $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/rtef

clean:
	rm -f src/*.o rtef.out

.PHONY: clean install
