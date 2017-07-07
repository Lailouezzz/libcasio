Welcome to libcasio's documentation!
====================================
.. rubric:: Everything you need to know about libcasio.

libcasio ought to become the *de facto* standard when it comes to manipulating
protocols and file formats used with CASIO calculators.
Most of it is platform-agnostic, although it contains stream and filesystem
interfaces with the most commons systems such as Microsoft Windows,
GNU/Linux distributions or Apple's OS X.

.. note::
	The C "namespace" the library reserves are `casio_`, `CASIO_`, `libcasio_`
	and `LIBCASIO_`; consider the usage of anything starting with one of those
	prefixes as having an undefined behaviour
	(so you really shoundn't do that).

There are many objects in libcasio. Choose what fits your need!

.. toctree::
	:maxdepth: 2
	:caption: Contents:

	stream.rst
