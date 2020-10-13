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

Run time:

* None

Make time:

* C compiler and linker (tcc used in the development process)

* gmake or bourne shell

* An elf.h implementation for your platform (GNU Linux elf.h and OpenBSD elf.h used in development process)

* C library (glibc and OpenBSD libc used in development process), made build time depend with CFLAGS=-static
