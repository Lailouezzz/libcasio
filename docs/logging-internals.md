---
layout: page
title: logging internals
---
To use the libcasio logging interface, just include `internals.h` at source
root, or include something that ends up including it, directly or indirectly.
If the interface looks weird to you, that may be because this has been an
experiment to make a logging system compatible with K&R and ANSI C (which
explains the double parenthesis).

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
