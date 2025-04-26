# The Step by Step Guide to Building an Operating System

1. Create basic files - `kernel.cpp`, `linker.ld`, `loader.s`, `Makefile`.
2. Setup a way to create the iso file and run it in some hypervisor.
3. Build memory segments by creating a global descriptor table.

> ## Global Descriptor table
>
> It is a slightly complex data structure containing essentially info on what memory segment is
> present where, and with what access rights and other stuff.
>
> Each entry of the table is 8 bytes long, with a perfect backwards compatibility available, all the
> way up to 80386 systems.

4.
