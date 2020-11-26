# What is the minimum amount of work needed by a linker to produce a bootable kernel

NTI Gymnasiet Göteborg

David Samuelsson - 3C

TE HT 2020-VT 2021

Handledare: Sofie Kjellgren

## Abstract

## Table of Contents

TODO

## Introduction

[maybe remove the headers]

### Query

What is the thinnest linker feature set required in order to produce a bootable kernel.

### Background

An operating systems core, the kernel is made up of many different object files which must be linked by a linker. However existing linkers are large, arguably over engineered, programs which are hard to port to new operating systems. This portability issue makes it much harder to achieve a self hosting operating system.

### Limitations

In order to limit the scope of this project I've chosen to only target the x86\_64 processor architecture as well as only supporting the ELF format for object and executable files. The linker will also not be capable of dynamic linking.

### Purpose

The purpose of this project is to design a program capable of ELF object file linking which retains high portability and uses no POSIX or other non-standard C extensions.

### Theory

[TODO: find sources]

During the compilation of a program from source code to machine code there are generally two steps.

First the source files containing the code are compiled into object files by a compiler. These objects contain machine code and metadata.

These object files are then processed by a linker who's job is to resolve missing symbols in object files with defined symbols in other object files, calculating absolute memory addresses, and then combining the code from these object into a file that the OS can load and the CPU can run.

x86\_64 is the name of a processor architecture. A processor architecture is in essence a description of a CPU's available registers and instructions. Different architectures might differ in features and will therefore have different linking processes. At the time of writing x86\_64 has been the dominant CPU architecture for many years.

[NOTE: everything below (except for the PS4) is in the ELF spec]

ELF, executable and linkable format, is a file format which can be used for both object and executable files. It is the standard format for these files on nearly all all UNIX based operating systems as well as on the PS4 game's console.

ELF files are based on headers and sections.

At the beginning of an ELF file lies the ELF header, a header which describes the general contents of the file, if the file is an executable or object file, as well as the positions of the files section and program headers.

Program headers describe what parts of the file to load into memory upon execution of the file as well as at what address these parts should be loaded at. Since program headers only describe execution they need only be present in executable files.

Section headers describe the type of and position of sections in the file. The content of sections may include (but isn't limited to) machine code, string tables, symbol tables, relocation tables. String table entries contains the name of the files sections and symbols. Symbol table entries describe symbols names, which sections they are defined relative to, and their values. Relocation table entries describe missing values within the files machine code which need to be resolved by the linker. Section headers are not needed at execution and need only be present in object files.

## Process

[program structure]

[program operation and used data structures]

### Issues

[different types of relocation]

## Results

[successful linking]

## Discussion and Conclusions

[discussing feature sets and dynamic linking as well as what I've had to implement for my linker]

## Sources

[TODO]

## Attachments

[Will probably be removed]
