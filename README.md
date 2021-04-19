# IO_PROGRAM

## Introduction

This program is a simple program for IO development on the Raspberry Pi. At this point I don't know where this project is going. It will most likely dead-end but something useful may come of it.

## Commands for compilation and execution

As I often forget exact syntax, here are some of the commands I use in my workflow.

### Pushing files from workstation to target

`scp -r C:\Users\wforbes\Documents\SOFTWARE\RASPI_IO\IO_PROGRAM.d admin@UNIX001:PROGRAMS.d/`

### Compiling the program on target device

Navigate to `IO_PROGRAM.d`

Execute the command `sudo bash COMPILE_SCRIPT`

### Running the program without compiling

Navigate to `IO_PROGRAM.d`

Execute the command `sudo ./IO_PROGRAM`