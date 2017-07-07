libcasio streams
================

When it wasn't libcasio yet (at the time, communication-related objects were
in libp7), libusb usage used to be hardcoded in the Protocol 7.00 management
functions (sending, receiving); then, it used FILE streams from the GNU libc
implementation, but that still wasn't platform-agnostic enough, so what has
finally be decided is that the library would use its own stream object.

This stream object is usable both by the library and by the user.
The library defines some useful platform-agnostic streams it uses itself,
such as the checksum stream (which calculates the checksum as the file content
is read), and some platform-specific streams, related to POSIX or
the Windows API for example. It should only implement platform-specific
streams for platforms defining a macro (for example, CASIOWIN doesn't!).

This document describes how to use and make a libcasio stream.

Opening and closing
-------------------

There are three ways to open a stream:

- with a specialized function, such as :code:`casio_open_stream_streams()`
  for POSIX or :code:`casio_opencom_windows()` for the Windows API;
- with a multi-stream function, such as :code:`casio_open_usb_stream()`
  for USB-connected calculators, or :code:`casio_open_com_stream()` for
  serial-connected calculators;
- with the core function, :code:`casio_open_stream()`, which you will use
  when you will want to define your own streams.

All of the stream opening function shall take a reference to the stream pointer
you'll use as the stream handle as the first parameter, and return the libcasio
error code that occured (or 0 if no error has occured).

Once you are done with the stream, you shall close it, so that all of the
allocated resources can be free'd properly. In order to do this, you can
simply use :code:`casio_close()`. So here's a simple example using
the :code:`casio_open_memory()` function (which makes a stream out of memory):

.. code-block:: c
	:linenos:
	
	int err;
	casio_stream_t *stream;
	
	err = casio_open_memory(&stream, my_memory_area, the_memory_area_size);
	if (err) { /* oh no, things went wrong... */ }
	
	/* make awesome things */
	
	err = casio_close(stream);
	if (err) { /* things went wrong, but that's alright */ }
