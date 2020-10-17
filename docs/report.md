# What is the minimum amount of work needed by a linker to produce a bootable kernel

NTI Gymnasiet Göteborg

David Samuelsson - 3C

TE HT 2020-VT 2021

Handledare: Sofie Kjellgren

## Abstract

## Table of Contents

## Introduction

### Background

An operating systems core, the kernel is made up of many different object files which must be linked by a linker. However existing linkers are large, arguably over engineered, programs which makes it hard to port them to new operating systems. This makes it much harder to achieve a self hosting operating system.

In order to limit the scope of this project I've chosen to only target the x86_64 processor architecture as well as only supporting the ELF format for relocatable and executable files.

### Purpose

### Query (frågeställning)
TODO, find better title

* What is the minimum amount of work needed by a linker to produce a bootable kernel

### Feature Specification (kravspec)
TODO, find better title

### Required knowledge

TODO, find sources to back up your knowledge, cause I'm not credible enough on my own...

During the compilation of a program from source code there are generally two steps. The source files are first compiled into object files by a compiler and these object files are then linked together into a single executable file which the operating system is able to parse and let the processor execute.

ELF, executable and linkable format, is a file format for both object and executable files which uses a variety of different tables in order to specify the contents of an object or executable file.

x86_64 is a short hand for i386/i686 64 bit, sometimes also reffered to as amd64. This is the processor architecture most commonplace in personal computers currently.

## Results

## Discussion and Conclusions

## Sources

## Attachments
