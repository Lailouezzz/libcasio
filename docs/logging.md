---
layout: page
title: logging
---
**This document only describes the library user control interface, which**
**doesn't cover the actual logging, aimed at library developers.**

For debugging purposes, libcasio has a debugging interface that allows
displaying messages in the debug stream (usually `stderr`) with four
different levels (plus a special one):

- `info` (debug information);
- `warn` (warnings);
- `error` (non-fatal errors);
- `fatal` (fatal errors);
- `none` (special log level to tell not to print messages).

Each level includes the message from the levels below in the list.

The log level used to be hardcoded into the library (with the configure
script), but it appeared that some users wanted to be able to control it
from the utilities using the library (with a `--log` option).

This interface is declared in `<libcasio/log.h>`. For forward compatibility
purposes, it works with strings.

To set the loglevel, use `casio_setlog()`, and to get a pointer to the current
loglevel, use `casio_getlog()`.

{% highlight c linenos %}
void casio_setlog(const char *level);
const char *casio_getlog(void);
{% endhighlight %}

Setting an unknown log level will simply result in setting the log level
to `none`.

Before setting the log level, you should list the recognized log levels.
For this, use the `casio_setlog()` function:

{% highlight c linenos %}
typedef void casio_log_list_t(void *cookie, const char *str);
void casio_listlog(casio_log_list_t *callback, void *cookie);
{% endhighlight %}

The callback will be called for every recognized log level. It is possible
for the function to call the callback only once (see the `--no-log` option
in [Getting started](getting-started.html)).

An example log level listing is the following:

{% highlight c linenos %}
#include <stdio.h>
#include <stdlib.h>
#include <libcasio.h>

void callback(void *cookie, const char *str)
{
	(void)cookie; /* no, cookie, we don't want to use you */
	printf("- %s\n", str);
}

int main(void)
{
	printf("Available log levels:\n");
	casio_listlog(&callback, NULL);
	return (0);
}
{% endhighlight %}
