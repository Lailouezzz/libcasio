---
layout: page
title: logging
---
For debugging purposes, libcasio has a debugging interface that allows
displaying messages in the debug stream (usually `stderr`) with four
different levels (plus a special one):

- `info` (debug information);
- `warn` (warnings);
- `error` (non-fatal errors);
- `fatal` (fatal errors);
- `none` (special log level to tell not to print messages).

Each level includes the message from the levels below in the list.

### Control interface for the library users
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
	printf("- %s\n", str);
}

int main(void)
{
	printf("Available log levels:\n");
	casio_listlog(&callback, NULL);
	return (0);
}
{% endhighlight %}

### Main interface for the library developers
To use it, just include `internals.h` at source root, or include something
that ends up including it, directly or indirectly. If the interface looks
weird to you, that may be because this has been an experiment to make
a logging system compatible with K&R and ANSI C (which explains the double
parenthesis).

There are two different types of logging in libcasio: message logging, and
memory logging (which end up being the same to the user).

First, message logging. The main macro for this is `msg((LEVEL, FORMAT, ...))`.
The level is of the form `ll_<level>`, so `ll_info`, `ll_warn`, `ll_error`
or `ll_fatal`. The format and arguments are for the printf-like function
behind, so you can use `msg((ll_info, "%d + %s", 5, "coucou"))` for example.

If you are doing a condition only for a logging instruction, with no `else if`
or `else` clause in the same state behind, you can use the `ifmsg`, `elifmsg`
and `elsemsg` macros. `ifmsg` and `elifmsg` take the condition and the
arguments for the `msg` function. For example:

{% highlight c linenos %}
if (something.cool > 0) {
	/* do something because it is cool */
}
elifmsg(something.cool == 0, (ll_warn, "The thing is not cool."))
elsemsg((ll_error, "The thing has NEGATIVE coolness, that's not cool."))
{% endhighlight %}

The memory logging family are organized the same, except the names are
`mem`, `ifmem`, `elifmem` and `elsemem`, and instead of the format and
format arguments, you have the pointer and the memory area size, e.g.:

{% highlight c linenos %}
char cooldude = "COOLDUD\x7F";
msg((ll_info, "Cool dude magic:"));
mem((ll_info, cooldude, 8));
{% endhighlight %}
