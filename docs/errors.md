---
layout: page
title: error management
---
Almost all functions that can fail in libcasio return an `int`, even if
they open a handle or descriptor of some kind (which is usually passed by
using a pointer to it as a first argument). This integer corresponds to the
error that occured in the function, or zero, representing the "everything
went fine" error.

The errors that can happen in libcasio are defined in `<libcasio/error.h>`,
although as usual, you should include `<libcasio.h>` to access the code
and utilities.

Some errors are "relative", which means their significance depends on the
function that returned it, when others are not. For example, `casio_error_op`
means the function has received arguments that it doesn't manage (sometimes,
yet), and it should not be transmitted as is, while `casio_error_read` can
be transmitted without any risk.

To get the full list of errors, you should read the header directly.
If you simply want to get the error string, you can use the `casio_strerror()`
function, which has the following prototype:

{% highlight c linenos %}
const char *casio_strerror(int err);
{% endhighlight %}

This string should only be used for displaying the error, as it could be
translated in future versions of the library.
