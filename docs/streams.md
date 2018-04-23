---
layout: page
title: streams
---
Streams are the abstractions for exchanging data with anything: a local or
distant file, a device through serial or USB communications, local memory
(usually for simulation), etc.

When it wasn't libcasio yet (at the time, communication-related objects were
in libp7), libusb usage used to be hardcoded in the Protocol 7.00 management
functions (sending, receiving); then, it used FILE streams from the GNU libc
implementation, but that still wasn't platform-agnostic enough, so what has
finally be decided is that the library would use its own stream object.

The stream object is usable both by the library and the user. The library
defines some useful platform-agnostic streams it uses itself, such as the
checksum stream (which calculates the checksum as the file content is read),
and some platform-specific streams for platforms defining a macro (for
example, CASIOWIN doesn't).

This document describes how to use and make a libcasio stream. Notice that
for the examples in this document, you shall include `<libcasio.h>`, or
the stream-specific header, `<libcasio/stream.h>`.

### Opening and closing a stream
There are three ways to open a stream:

- with a specialized function, such as `casio_open_stream_streams()` for
  POSIX or `casio_opencom_windows()` for the Windows API;
- with a multi-stream function, such as `casio_open_usb_stream()` for
  USB-connected calculators, or `casio_open_com_stream()` for
  serial-connected calculators;
- with the core function, `casio_open_stream()`, which you will use when
  you will want to define your own streams.

All of the stream opening functions shall take a reference to the stream
pointer you'll use as the stream handle as the first parameter, and return
the libcasio error code that occured (or 0 if no error has occured).

Once you are done with the stream, you shall close it, so that all of the
allocated resources can be free'd properly. In order to do this,
you can simply use `casio_close()`.

{% highlight c linenos %}
int casio_open_any_stream(casio_stream_t **stream, ...);
int casio_close(casio_stream_t *stream);
{% endhighlight %}

So here's a simple example using the `casio_open_memory()` function
(which makes a stream out of memory), for opening a stream and directly
closing it:

{% highlight c linenos %}
#include <libcasio.h>

void do_something()
{
	int err;
	casio_stream_t *stream = NULL;
	char zone[6];
	
	err = casio_open_memory(&stream, zone, 6);
	if (err) {
		/* an error has occured! */
		return ;
	}
	
	/* do something here, if an error occurs, make sure to
	   close the stream anyway, or use the 'fail' label here */

fail:
	err = casio_close(stream);
	/* you can check the error if you want, but the stream will always
	   be closed at this point, more or less properly */
}
{% endhighlight %}

### Opening modes
libcasio streams have different features that are activated by a flag in the
open mode. Here are the existing ones:

- `CASIO_OPENMODE_READ`: reading bytes from the stream is allowed (the
  `casio_read()` and `casio_skip()` functions can be used);
- `CASIO_OPENMODE_WRITE`: writing bytes to the stream is allowed (the
  `casio_write()` and `casio_write_char()` can be used);
- `CASIO_OPENMODE_SEEK`: seeking in the stream is allowed (the
  `casio_seek()` can be used);
- `CASIO_OPENMODE_SERIAL`: setting the serial properties of the stream is
  allowed (the `casio_streamfuncs_setattrs` callback will be used);
- `CASIO_OPENMODE_SCSI`: SCSI operations on the stream are allowed (the
  `casio_streamfuncs_scsi` callback will be used);
- `CASIO_OPENMODE_USB`: USB operations on the stream are allowed (no
  callback corresponding to this is done yet).

Notice that even if `CASIO_OPENMODE_SEEK` is not allowed, `casio_tell()` will
work and tell the

There also are a few open modes that aren't used by the core functions
themselves but that above opening functions can use when appropriate:

- `CASIO_OPENMODE_TRUNC`: the file will be truncated;
- `CASIO_OPENMODE_APPEND`: will append to the file.
