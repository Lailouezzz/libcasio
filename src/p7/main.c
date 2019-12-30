/* *****************************************************************************
 * p7/main.c -- p7 main source.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of p7utils.
 * p7utils is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * p7utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with p7utils; if not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************** */
#include "main.h"
#include <stdlib.h>
#include <string.h>

/* ---
 * Error messages.
 * --- */

/* Couldn't initialize connexion with the calculator. */

static const char error_noconnexion[] =
"Could not connect to the calculator.\n"
"- Is it plugged in and in receive mode?\n"
"- Have you tried changing the cable?\n";

/* Calculator was disconnected. */

static const char error_disconnected[] =
"Lost connexion to the calculator!\n"
"Please reconnect the calculator, rerun receive mode and try again.\n";

/* Calculator was found but program wasn't allowed to communicate with it. */

static const char error_noaccess[] =
"Could not get access to the calculator.\n"
"Install the appropriate udev rule, or run as root.\n";

/* Command was unsupported. */

static const char error_unsupported[] =
"The command is unsupported by the calculator.\n"
"- Does the calculator have mass storage?\n"
"- Does its OS allow the use of it?\n"
"- Is it in Receive Mode (and not in OS Update)?\n";

/* The device didn't exist. */

static const char error_unsupported_device[] =
"Device '%s' is not supported by the device.\n";

/* The calculator acted in an unplanned way. */

static const char error_unplanned[] =
"The calculator didn't act as planned.\n"
"Stop receive mode on calculator and start it again before re-running " \
	BIN ".\n"
"Error was: %s\n";

/* Requested file didn't exist. */

static const char error_noexists[] =
"Requested file didn't exist.\n";

/* Sent file cannot be empty. */

static const char error_empty[] =
"Can't send an empty file!\n";

/* Not enough space left on the calculator. */

static const char error_nospace[] =
"Not enough space on the calculator for the file you're trying to send.\n"
"If you believe there should be, try optimizing (OPT) on the calculator\n"
"(in MEMORY menu) and try again.\n";

/* ---
 * Auxiliary functions.
 * --- */

/**
 *	sendfile_confirm:
 *	Confirm file sending.
 *
 *	@return				if the file overwriting is confirmed
 */

static int sendfile_confirm(void)
{
	char line[10];

	/* Print stuff */
	printf("It looks like the file already exists on the calculator.\n");
	printf("Overwrite ? ([n]/y) ");

	/* Get the line */
	if (!fgets(line, 10, stdin))
		return (0);

	/* Check if should overwrite */
	return (*line == 'y');
}

/**
 *	sendfile_display:
 *	File sending nice display.
 *
 *	"Initialization" is when id > total (called in main).
 *
 *	@arg	id			data packet ID
 *	@arg	total		total number of data packets
 */

static int sendfile_display_initialized = 0;
static void sendfile_display(unsigned int id, unsigned int total)
{
	/* here's the buffer */
	static char buf[50] =
		"\r|---------------------------------------| 00.00%";
	static char *bar = &buf[2];

	/* initialize */
	static int pos;

	/* if is initialize, fill */
	if (id > total) {
		pos = 0;
		/* indicate that is has been initialized */
		sendfile_display_initialized = 1;
		/* put initial buffer */
		fputs(buf, stdout);
		/* we're done */
		return ;
	}

	/* modify buffer */
	/* - # - */
	int current = 38 * id / total;
	while (pos <= current) bar[pos++] = '#';
	/* - % - */
	unsigned int percent = 10000 * id / total;
	sprintf(&buf[43], "%02u.%02u", (percent / 100) % 100, percent % 100);

	/* put it */
	fputs(buf, stdout);
	fflush(stdout);
}

/**
 *	print_file_info:
 *	File listing callback.
 *
 *	@arg	cookie	(unused)
 *	@arg	dir		the directory in which the file is stored (NULL if root)
 *	@arg	name	the filename
 *	@arg	size	the filesize
 */

static void print_file_info(void *cookie,
	const casio_pathnode_t *node, const casio_stat_t *stat)
{
	(void)cookie;
	/* initialize buffer */
	static char buf[45];
	printf("callback called : %s\n", node->casio_pathnode_name);
}

/* ---
 * Main function.
 * --- */

/**
 *	main:
 *	User entry point of the program.
 *
 *	@arg	ac		arguments count
 *	@arg	av		arguments values
 *	@return			return code (0 if ok)
 */

int main(int ac, char **av)
{
	int err = 0; args_t args;
	casio_link_t *handle = NULL;
	casio_fs_t *fs = NULL;

	/* Decode the arguments. */

	if (!parse_args(ac, av, &args))
		return (0);

	/* Initialize the link. */

	if (args.com)
		err = casio_open_com(&handle, args.initflags, args.com, args.use);
	else
		err = casio_open_usb(&handle, args.initflags, -1, -1);
	if (err) {
		switch (err) {
		case casio_error_nocalc:
			fprintf(stderr, error_noconnexion);
			break;
		case casio_error_noaccess:
			fprintf(stderr, error_noaccess);
			break;
		default:
			fprintf(stderr, error_unplanned, casio_strerror(err));
			break;
		}

		/* Close and remove if necessary. */

		if (args.menu == mn_send)
			fclose(args.local);
		if (args.menu == mn_get && args.local != stdout) {
			fclose(args.local);
			remove(args.localpath);
		}
		return (1);
	}

	/* Change speed, and things. */

	if (args.do_the_set) {
		err = casio_setlink(handle, args.set);
		if (err)
			goto fail;
	}

	/* Check according to menu */

	casio_path_t path;
	switch (args.menu) {
#if 0
		case mn_send:
			/* get file size */
			fseek(args.local, 0, SEEK_END);
			casio_off_t filesize = (casio_off_t)ftell(args.local);
			rewind(args.local);

			/* get capacity */
			unsigned long capacity;
			err = casio_getfreemem(handle, args.storage, &capacity);
			if (err) break;

			/* optimize if required */
			if (filesize > (casio_off_t)capacity) {
				printf("Not enough space on the device. Let's optimize!\n");
				err = casio_optimize(handle, args.storage);
				if (err) break;
			}

			/* send the file */
			err = casio_sendfile(handle, args.local,
				args.dirname, args.filename,
				args.storage, 1, args.force ? NULL : &sendfile_confirm,
				args.nicedisp ? &sendfile_display : NULL);
			break;
		case mn_get:
			err = casio_reqfile(handle, args.local,
				args.dirname, args.filename,
				args.storage, args.nicedisp && args.local != stdout
				? &sendfile_display : NULL);
			break;
		case mn_copy:
			err = casio_copy(handle, args.dirname, args.filename,
				args.newdir, args.newname, args.storage);
			break;
		case mn_del:
			err = casio_delete(fs, args.dirname, args.filename,
				args.storage);
			break;
		case mn_reset:
			err = casio_reset(handle, args.storage);
			break;
#endif
		case mn_list:
			path.casio_path_device = args.storage;
			casio_make_pathnode(&path.casio_path_nodes, 1);
			if ((err = casio_open_seven_fs(&fs, handle))
			 || (err = casio_list(fs, &path, print_file_info, NULL)))
				break;
			
			break;
		case mn_optimize:
			if ((err = casio_open_seven_fs(&fs, handle))
			 || (err = casio_optimize(fs, args.storage)))
				break;

			break;

		case mn_info:
			err = dump(handle);
			break;

		case mn_idle:
			break; /* time! */

		case mn_unlock:
			break; /* dance! */

		default:
			fprintf(stderr, "Unhandled yet.\n");
			break;
	}

	/* Put the error. */

	if (err && err != casio_error_noow)
		goto fail;

	if (sendfile_display_initialized)
		puts("\b\b\b\b\b\bTransfer complete.");
	if (args.local && args.local != stdout) fclose(args.local);

	/* Terminate communication and de-initialize link handle. */

	casio_close_fs(fs);
	fs = NULL;
	casio_close_link(handle);
	handle = NULL;

	/* Then we're good */
	return (0);
fail:
	if (sendfile_display_initialized)
		puts("\b\b\b\b\b\bError !");

	/* close the file */
	if (args.local && args.local != stdout) fclose(args.local);
	if (args.menu == mn_get && args.local != stdout)
		remove(args.localpath);

	/* put the error string */
	switch (err) {
	case casio_error_fullmem:
		fprintf(stderr, error_nospace); break;
	case casio_error_empty:
		fprintf(stderr, error_empty); break;
	case casio_error_notfound:
		fprintf(stderr, error_noexists); break;
	case casio_error_nocalc:
		fprintf(stderr, error_disconnected); break;
#if 0
	case casio_error_unsupported:
		fprintf(stderr, error_unsupported); break;
	case casio_error_unsupported_device:
		fprintf(stderr, error_unsupported_device, args.storage); break;
#endif
	default: fprintf(stderr, error_unplanned, casio_strerror(err));
	}

	/* that doesn't mean you shouldn't exit, heh. */
	casio_close_fs(fs);
	fs = NULL;
	casio_close_link(handle);
	handle = NULL;

	/* then go away */
	return (1);
}
