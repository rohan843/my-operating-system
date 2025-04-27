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

4. Setup the background for setting up the programmable interrupt controller to ensure messages from
   devices such as the keyboard are not blocked.
   As part of this, we'll build some classes that help us manipulate ports.

> ## PIC: Programmable Interrupt Controller
>
> It is a hardware chip that manages the devices attached to the computer, such as a keyboard, a
> mouse, etc. It handles the interrupts from these devices to ensure the CPU isn't overwhelmed with
> the requests.
>
> Before using the PIC, it is important to remap it to the appropriate ports, which is what we'll do
> in this step.

5. Setup the Interrupt Descriptor Table to tell the CPU how to handle the interrupts the PIC would
   send.

> ## Interrupt Handlers and the Interrupt Descriptor Table
>
> Interrupt handlers are short and quick functions that tell the CPU what to do in case of an
> interrupt. They are called during the execution of some other process, and therefore must ensure
> that the registers contain the data the other process expects, once the handler returns.
>
> The interrupt descriptor table contains addresses and other details about what handler handles
> what interrupt.
