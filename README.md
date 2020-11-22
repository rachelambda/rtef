rtef
====

Relocatable to executable format - A (WIP) x86_64-ELF linker

Why?
----

Existing linkers are either huge and hard to port to other platforms or do not have the capability to move and align sections to the degree needed for kernel development

Goals
-----

* Keep code simple and portable, ideally without POSIX C extensions

* Give the user the ability to align and move sections in their code

Non-goals
---------

* Dynamic linking

Portability
-----------

rtef currently has the following depends

C library with the following symbols
```
malloc
exit
fopen
fread
fseek
memmove
strcmp
stdout
fwrite
puts
printf
strlen
strcpy
realloc
vprintf
```

An elf.h implementation for your platform (GNU elf.h for Linux used in development process)

A C compiler (tcc, gcc and clang used in development process)

gmake or bourne shell to automate compilation (not strictly needed)
