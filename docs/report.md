# NOT FINAL. THIS VERSION IS UNFORMATED AND WILL BE REFORMATED ONCE LAREGLY FINISHED

# What is the minimum amount of work needed by a linker to produce a bootable kernel

NTI Gymnasiet Göteborg

David Samuelsson - 3C

TE HT 2020-VT 2021

Handledare: Sofie Kjellgren

## Abstract

## Table of Contents

[PLACEHOLDER]

## Introduction

### Query

What is the thinnest linker feature set required in order to produce a bootable kernel.

### Background

An operating systems core, the kernel is made up of many different object files which must be linked by a linker. However existing linkers are large, arguably over engineered, programs which are hard to port to new operating systems. This portability issue makes it much harder to achieve a self hosting operating system.

The purpose of this project is to design a program capable of ELF object file linking which retains high portability and uses no POSIX or other non-standard C extensions.

In order to limit the scope of this project I've chosen to only target the x86\_64 processor architecture as well as only supporting the ELF format for object and executable files. The linker will also not be capable of dynamic linking.

### Theory

When a computer runs a program the operating system must load an executable file into memory and tell the CPU to start running it. Many modern programing languages such as ruby for example are interpreted, which means that rather than executing the program itself the OS executes ruby which then runs the file.[^EXECWIKI] However, this process is quite slow and as such there is still a need to write compiled programs.

During the compilation of a program from source code to machine code there are generally two steps.[^LINKERWIKI]

First the source files containing the code are compiled into object files by a compiler. These objects contain machine code and metadata.[^LINKERWIKI]

These object files are then processed by a linker who's job is to resolve missing symbols in object files with defined symbols in other object files, calculating absolute memory addresses, and then combining the code from these object into a file that the OS can load and the CPU can run.[^LINKERWIKI]

x86\_64 is the name of a processor architecture. A processor architecture is in essence a description of a CPU's available registers and instructions. Different architectures might differ in features and will therefore have different linking processes. At the time of writing x86\_64 has been the dominant CPU architecture for many years.[^X86WIKI]

ELF, executable and linkable format, is a file format which can be used for both object and executable files.[^ELFSPEC] It is the standard format for these files on nearly all all UNIX based operating systems as well as on the PS4 game's console.[^ELFWIKI]

ELF files are based on headers and sections.[^ELFSPEC]

At the beginning of an ELF file lies the ELF header, a header which describes the general contents of the file, if the file is an executable or object file, as well as the positions of the files section and program headers.[^ELFSPEC]

Program headers describe what parts of the file to load into memory upon execution of the file as well as at what address these parts should be loaded at. Since program headers only describe execution they need only be present in executable files.[^ELFSPEC]

Section headers describe the type of and position of sections in the file. The content of sections may include (but isn't limited to) machine code, string tables, symbol tables, relocation tables. String table entries contains the name of the files sections and symbols. Symbol table entries describe symbols names, which sections they are defined relative to, and their values. Relocation table entries describe missing values within the files machine code which need to be resolved by the linker, as well as information regarding how to calculate the correct value. Section headers are not needed at execution and need only be present in object files.[^ELFSPEC]

## Process

The first action of the process was planning the general flow of the program as well as defining the required data structures.

The following data structures were defined: elf\_file, a structure which contains all input data read from a file; sym\_def, a structure which contains all definitions of a single symbol; sec\_def, a structure which contains all definitions of a single section.

It was determined that the program flow would begin with an optional argument handler, which has the job of parsing the arguments given to the program on the command line, in order to define input and output files, as well as letting the user specify additional options. In the final product this wasn't present as no options other than input and output files were implemented.

Secondly the program runs a file validator, this ensures all input files are valid ELF files. In practice this was implemented by reading files ELF headers and checking their validity, as well as ensuring they targeted the correct platform (x86-64, in this case).

Thirdly the program collects all information from the input files, in essence filling out all fields of the elf\_file, sym\_def and sec\_def data structures.

Fourth the program calculates the memory address for each section in the output file by parsing the sec\_def data structures.

Fifth the program resolves missing symbols in all files by looking at each elf\_file's relocation entries and then finding the matching sym\_def entry, which after a short calculation dependent on the relocation info resolves the value of the defined symbol.

Sixth and ELF header and program headers are generated based on the values calculated in the fourth step.

Finally the program writes all data to a file and marks it as executable.

### Issues

Throughout the process there have been several issues. These were mostly caused by a lack of experience going into the project. For instance, it was initially planned to calculate section addresses after the symbol relocations. Though this is impossible due to some symbols being defined relative to the memory address of sections. Another example of this is the previously mentioned unimplemented first step. This was ultimately skipped as it turned out the program had no need for it.

There were also issues with the used data structures as it was found they didn't contain enough information several times. This lead to having to add additional fields of information to the data structures and having to rewrite old code in order to write information to them.

## Results

[successful linking]

## Discussion and Conclusions

[discussing feature sets and dynamic linking as well as what I've had to implement for my linker]

## References

[^ELFSPEC]: Tool Interface Standards commitee (TIS). (1995, May). *Executable and Linkable Format (ELF)*. In *TIS standard Portable Formats Specification V1.1*. Retrieved December 3.

[^ELFWIKI]: Wikipedia contributors. (2020, November 4). Executable and Linkable Format.
In *Wikipedia, The Free Encyclopedia*. Retrieved December 3, 2020, from
[https://en.wikipedia.org/w/index.php?title=Executable_and_Linkable_Format&oldid=987064905](https://en.wikipedia.org/w/index.php?title=Executable_and_Linkable_Format&oldid=987064905).

[^LINKERWIKI]: Wikipedia contributors. (2020, November 27). Linker (computing).
In *Wikipedia, The Free Encyclopedia*. Retrieved December 3, 2020, from
[https://en.wikipedia.org/w/index.php?title=Linker_(computing)&oldid=990946784](https://en.wikipedia.org/w/index.php?title=Linker_\(computing\)&oldid=990946784).

[^X86WIKI]: Wikipedia contributors. (2020, November 8). x86-64.
In *Wikipedia, The Free Encyclopedia*. Retrieved December 3, 2020, from
[https://en.wikipedia.org/w/index.php?title=X86-64&oldid=987698018](https://en.wikipedia.org/w/index.php?title=X86-64&oldid=98769801://en.wikipedia.org/w/index.php?title=X86-64&oldid=987698018).

[^EXECWIKI]: Wikipedia contributors. (2020, November 29). Executable.
In *Wikipedia, The Free Encyclopedia*. Retrieved December 3, 2020, from
[https://en.wikipedia.org/w/index.php?title=Executable&oldid=991287717](https://en.wikipedia.org/w/index.php?title=Executable&oldid=991287717).


## Attachments

[Will probably be removed]
