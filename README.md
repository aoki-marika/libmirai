# libmirai

A C library for reading files from the Hatsune Miku: Project Mirai series of 3DS games.

# Usage

As libmirai only uses the C standard library, there are many ways to include it.

If using Xcode then it is recommended to include the libmirai Xcode project in your workspace,
and then have the library be linked by adding libmirai to your target's Frameworks and Libraries.

In other cases either the source files can be added to the program, or a custom Makefile or similar
can be created to build libmirai into a static/dynamic library and then link that to the program.