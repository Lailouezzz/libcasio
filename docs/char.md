---
layout: page
title: character encoding and conversions
---
libcasio has its own portable character encoding conversion, resembling
iconv. It supports Unicode transformation formats (UTF-8, UTF-16, UTF-32)
and CASIO's proprietary character encoding named FONTCHARACTER according to
the SDK it published in 2006 for the fx-9860G.

The character conversion utilities are defined in `<libcasio/char.h>`,
but as usual, including `<libcasio.h>` for everything is recommended.

### Opening and closing a conversion descriptor
The conversion descriptor is the main object that will keep track of
the conversion steps and status. Here are the main functions to manage it:

{% highlight c linenos %}
int casio_open_conv(casio_conv_t *cd, const char *to, const char *from);
int casio_close_conv(casio_conv_t cd);
{% endhighlight %}

The `to` and `from` parameters of the `casio_open_conv()` function are the
string identifiers of the source and destination encodings for the conversion.
For example, use `casio_open_conv(&cd, "utf-8", "utf-32")` to open a descriptor
that will be able to convert UTF-32 encoded data into UTF-8 encoded data.

### Converting formats
Once the conversion descriptor is opened, you can use the `casio_conv()`
function, defined this way:

{% highlight c linenos %}
int casio_conv(casio_conv_t cd,
	const char **in_buffer, size_t *in_left,
	const char **out_buffer, size_t *out_left);
{% endhighlight %}

In the same fashion than iconv, this function is made for being called
several times, usually because you read from a stream and write to another
stream — although you can use it to convert data all at once.

Here is a simple example using two buffers:

{% highlight c linenos %}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libcasio.h>

int main(void)
{
	int err = 0; casio_conv_t cd = NULL;
	uint32_t buf0[] = {'H', 'e', 'l', 'l', 'o'};
	size_t buf0_size = 5 * sizeof(uint32_t);
	uint32_t buf1[] = {' ', 'w', 'o', 'r', 'l', 'd', '!'};
	size_t buf1_size = 7 * sizeof(uint32_t);
	char bufdata[30], *buf = &bufdata;
	size_t outsize = 29; /* let space for the end zero */

	/* Open the conversion descriptor. */
	err = casio_open_conv(&cd, "utf-8", "utf-32");
	if (err) {
		fprintf(stderr, "Could not open the conversion desc.: %s",
			casio_strerror(err));
		goto fail;
	}

	/* Convert the first buffer. */
	err = casio_conv(cd, &buf0, &buf0_size, &buf, &outsize);
	if (err) {
		fprintf(stderr, "Could not convert the first buffer: %s",
			casio_strerror(err));
		goto fail;
	}

	/* Convert the second buffer. */
	err = casio_conv(cd, &buf1, &buf1_size, &buf, &outsize);
	if (err) {
		fprintf(stderr, "Could not convert the second buffer: %s",
			casio_strerror(err));
		goto fail;
	}

	/* Print the buffer. */
	*buf = '\0';
	fputs(bufdata, stdout);

fail:
	/* Close the conversion descriptor. */
	casio_close_conv(cd);
	return (err != 0);
}
{% endhighlight %}
