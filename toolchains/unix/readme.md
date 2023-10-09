# Unix toolchain instructions

## Important information

If you are:

* building SAMEthing for a Linux distribution (e.g., [Ubuntu](https://ubuntu.com/), [Arch Linux](https://archlinux.org/))
* building SAMEthing for [macOS](https://www.apple.com/macos/)
* building the toolchain from source

You **MUST** have the appropriate C and C++ development libraries and headers
installed on your system, for your system. These toolchains will not and cannot
provide them for you if any of the above is true. If you are unable to do this,
you cannot proceed.

The toolchain scripts are written in portable POSIX sh, but that does ***not***
mean it will work on every single Unix flavor. To prevent possible issues on
untested flavors, the scripts will exit fatally by design if they detect they
are executing on a host system not tested. The odds of an issue being
encountered with this approach are insignificant.

Refer to your host specific instructions to begin.

### Host-specific system instructions

* [Linux](#Linux)
* [macOS](#macOS)

### Linux 

#### Preamble

If you are building SAMEthing for a Linux distribution or building the toolchain
from source and do not have C/C++ development libraries and headers installed,
you must consult your distribution to install the appropriate software before
continuing. While we cannot provide instructions for every single distribution
or package manager, we can help with some common ones:

* [Ubuntu](https://ubuntu.com/): Run `sudo apt install libc6-dev` in a terminal.
* [Arch Linux](https://archlinux.org): Run `sudo pacman -S --needed glibc` in a terminal.

*Consult your distribution's package manager or support groups if in doubt.*

These instructions do not have to be followed if building for bare-metal
targets; the toolchain provides custom C/C++ development libraries and headers.

#### Download pre-built toolchain

Simply execute the `toolchain_fetch.sh` script and the appropriate toolchain
will be downloaded and immediately ready for use. This is the option most people
should use.

#### Build from source

TBD

### macOS

#### Preamble

If you are building SAMEthing for macOS distribution or building the toolchain
from source and do not have C/C++ development libraries and headers installed,
executing the `toolchain_build.sh` script with `--autoinstalldeps` will handle
this for you.

These instructions do not have to be followed if building for bare-metal
targets; the toolchain provides custom C/C++ development libraries and headers.
