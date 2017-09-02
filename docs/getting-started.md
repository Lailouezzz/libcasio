---
layout: page
title: getting started
---
The libcasio development happens [on Github][gh].  
There is currently no mailing-list.  
The release source codes are available [here][pub] (all P7 projects are
in the same folder).

libcasio is a development library, either shared or static.
There are many configuration options for adapting the library to your
platform and needs.

To configure, use `./configure <options>`. Options are used as
`--<name>=<value>`. The following are some build options:

- `--target=<target>`: the target for which to produce the library (by default,
  native);
- `--static`: produce a static library (by default, shared);
- `--windows`: use the Windows versions of the final formats (dll instead of
  elf, lib instead of coff);
- `--no-libusb`: do not use (and link against) libusb;
- `--no-file`: do not use the standard I/O library (probably because there
  aren't any);
- `--no-log`, `--loglevel=<level>`: if we should include debugging content
  (but keep the interface), and if yes, what the default library log level.
  See [Logging](logging.html) for more information;
- `--maintainer`: alias for `--loglevel=info`, also introduces more
  compilation warnings.

Other options are packaging options (todo: add info about them here).

[gh]: https://github.com/PlaneteCasio/libcasio
[pub]: https://p7.planet-casio.com/pub/
