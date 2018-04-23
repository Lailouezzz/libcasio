---
layout: page
title: file organization
---
There are many, many files in libcasio, and that's because I try to keep the
project clean and organized. This file is an attempt at describing how
the files are organized and where you can find or should put anything here.

There are a few main files and folders:

- `docs/`: a statically generated website using Jekyll, in which the page
  you're currently reading is written;
- `include/`: the public headers to use with the library (not including
  internal headers);
- `src/`: the sources and internal headers;
- `tools/`, `configure`, `Makefile`, `Makefile.vars`, `Makefile.msg`:
  build utilities;
- `README.md`, `CONTRIBUTING.md`, `LICENSE.md`: basic to-read files.

In the `include/` folder, there is the main header the user is supposed to
include, `libcasio.h`, and a subfolder, which contains all of the headers
specific to modules. It is organized as in `src/`, in modules which represent
the abstraction the module defines. For example, `src/stream/` and
`include/libcasio/stream.h` are related to the libcasio-specific stream which
defines platform-agnostic functions to the system-specific utilities.

In the source folder, the `internals.h` header is the general internal header
which is included in every source file in the project (sometimes using a
module or submodule specific header which defines some more specific things
afterwards). It contains general utilities such as reliable endian management
macros, reliable integer types, and so on.
