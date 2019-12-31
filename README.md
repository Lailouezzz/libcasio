# libcasio - Library and utilities for using CASIO's protocols and file formats

This is the main repository for the P7 project, a project to free software
surrounding CASIO calculators. It aims at being able to manage CASIO's
proprietary protocols and file formats. It is basically a merge of three
subprojects of the P7 project: the libp7, which managed protocols, the
libg1m, which managed file formats, and the p7utils, which had the utilities
now put in this repository.

## Prerequisites

Side note : the library might work with older versions of these dependencies,
I took these as a reference because these are the ones I work with.

Because of libusb, the library requires C99. Without it, it should compile
using C89.

## Build-only dependencies

- `make <https://www.gnu.org/software/make/>`_ (>= 4.0).
- `gcc <https://gcc.gnu.org/>`_ (>= 4.9).
- `binutils <https://www.gnu.org/software/binutils/>`_ (>= 2.25).
- `asciidoc <http://asciidoc.org/>`_ (>= 8.6.9).
- `gzip <https://www.gnu.org/software/gzip/>`_ (1.6).
- `pkg-config <https://www.freedesktop.org/wiki/Software/pkg-config/>`_.

## Runtime dependencies

- `libusb <http://libusb.info/>`_ (>= 1.0).
- `zlib <http://zlib.net/>`_ (>= 1.2.8).
- `SDL <https://www.libsdl.org/>`_ (>= 2.0).

## Configuring

First of all, the configure script has a help message where all of the
options are listed. The defaults should be appropriate for a native build,
just ``./configure`` if that's what you want to do, but you should read at
least the next paragraph.

If you're installing on or packaging for a GNU/Linux distribution, you might
want to install the ``udev`` rule to let the access to the direct calculator
connexions to normal users. As calculators can be connected as serial devices,
which are attributed to the ``uucp`` group (on Manjaro GNU/Linux), I chose to
attribute the ``uucp`` group to calculators plugged directly as USB. If you
agree with that, just add the ``--udev`` option to the configuration command
line; otherwise, make your own, or make nothing, I don't really care.

Building for the MS-Windows platform is supported, just use a ``-mingw32``
BFD target for GCC and Binutils.

Also, building a static library, for both PC and embedded systems, is
supported : just use the ``--static`` option. If you want to build a static
library so that a cross-compiler that uses Microsoft Windows static library
formats, add the ``--windows`` option (notice that for a ``-mingw32`` target
and a dynamic library generation, this option is automatically added).

## Building and installing

Once configuring is done (necessary step), just ``make``.  
To install, use ``make install``.

To build and install only the lib, use ``all-lib`` then ``install-lib``.
To build and install only the docs, use ``all-doc`` and ``install-doc``.

If you ought to package this library, use the ``DESTDIR`` Makefile variable,
e.g. ``make install DESTDIR=./package-root``. Do **not** use the ``--root``
configure options for this, as configure tools (``libcasio-config`` and the
``pkg-config`` configuration file) will add what's in the `--root` option
value but not in the ``DESTDIR`` option.

Other useful targets:

- ``clean``: remove built files;
- ``re``: regenerate built files (clean and build) -- useful when
  configuration is changed.