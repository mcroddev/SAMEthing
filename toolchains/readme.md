# Toolchain information

SAMEthing uses specialized toolchains for development.

Advantages:

* Exerts tight control over the environment, limiting the influence of the host
  system as much as possible. This allows developers/contributors to have a
  uniform environment, with a strong guarantee that the configuration of each
  dependency and piece of software is precisely adjusted for our use case. This
  makes debugging easier as there are numerous questions that we don't have to
  ask.

* Prevents dependency issues; once this repository is cloned, the only action
  the user must take is to execute the appropriate "toolchain fetch" script for
  their host system, and the ecosystem for this application is there, and just
  works. This reduces frustration and is very easy to get going.

Disadvantages:

* There are a lot of moving parts in each "toolchain build" script, with a
  decent potential for errors. Careful testing is required for each change,
  which can slow down development time for the actual application.

* Even when a toolchain is stripped, compressed, and using the minimum amount of
  functionality needed for the project, the toolchains are quite large. This is
  the least severe of the disadvantages, as disk space is generally large and
  cheap, on top of high-speed internet being generally available.

* They take a *very* long time to build on a decent system, even longer on
  GitHub's CI pipeline (upwards of 4+ hours), especially when LTO and PGO are
  requested. This project does not have the funding necessary to use anything
  but the free GitHub CI pipeline.

## Structure

Each toolchain is capable of targeting the x86-64 and ARM 32-bit/64-bit
architectures.

There are only 3 host systems where the toolchain can run on:

* [Windows 11 x86-64](https://www.microsoft.com/en-us/windows?r=1)
    - Supported targets: [Windows 11 x86-64](https://www.microsoft.com/en-us/windows?r=1)

* [Linux x86-64](https://www.linux.org/)
    - Supported targets: [Linux x86-64](https://www.linux.org/)

* [macOS Sonoma on Apple Silicon](https://www.apple.com/macos/)
    - Supported targets: [macOS Sonoma on Apple Silicon](https://www.apple.com/macos/sonoma)

**NOTE:** "Supported" does not mean "won't work on other targets"; it means we
do not officially support any target other than the aforementioned.

Each toolchain contains the following software:

* [LLVM 16.0.6](https://llvm.org/)
* [gcc 13.2.0](https://gcc.gnu.org/)
* [CMake 3.27.1](https://cmake.org/)
* [ninja v1.11.1](https://ninja-build.org/)
* [Qt 6.5.2](https://www.qt.io/)
* [SDL 2.28.3](https://www.libsdl.org/)

For host system specific instructions, navigate to the folder corresponding to
your system.
