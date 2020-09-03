rtef
====

Relocatable to executable format - A (WIP) x86_64-ELF linker

Why?
----

Existing linkers are either huge and hard to port to other platforms or do not have the capability to move and align sections to the degree needed for kernel development

Goals
-----

* Keep code simple and portable, ideally without POSIX C extensions

* Do not rely on libelf

* Give the user the ability to align and move sections in their code

Non-goals
---------

* Dynamic linking
