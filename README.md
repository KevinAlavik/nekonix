# Nekonix (Nnix.)

> ![NOTE]
> Nekonix (Nnix.) is a hobby project and not meant to run in production environments.

<img src="https://github.com/KevinAlavik/nekonix/blob/main/assets/nnix-dark.png?raw=true" width="150">

Nnix. is a hobby UNIX-like operating system for the x86_64 architecture.

## Building

1. Firstly create a kernel configuration file
    ```sh
    make menuconfig
    ```
    *Note: The default options should be good enough, so just exit to use the defaults.*

2. Then build the OS
    ```sh
    make
    ```
    *Note: This will create an ISO file by default called "Nekonix.iso".*


## Running

Nekonix aims to be a stable simple operating system. For now the only hardware supported by Nekonix is the QEMU virtual machine, we will of course support more hardware later on.

You can run directly using `qemu-system-x86_64` (expected to be in PATH):

1. Run Nekonix in QEMU with BIOS-ISO option (default):
    ```sh
    make run
    ```
    *Note: Aditional QEMU flags can be defined in the QEMUFLAGS variable, e.g. make run QEMUFLAGS="-serial file:com1.txt"*

2. Run Nekonix in QEMU with UEFI-ISO option:
    ```sh
    make run-uefi
    ```
    *Note: Additional QEMU flags works in the same way as in BIOS-ISO*

3. Run Nekonix in QEMU with BIOS-HDD option:
    ```sh
    make run-hdd
    ```
    *Note: Additional QEMU flags works in the same way as in BIOS-ISO*

4. Run Nekonix in QEMU with UEFI-HDD option:
    ```sh
    make run-hdd-uefi
    ```
    *Note: Additional QEMU flags works in the same way as in BIOS-ISO*

*Note: Read the Makefile for more information*

## License

Nnix. is licensed under the MIT license, see LICENSE file.