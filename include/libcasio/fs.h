/* ****************************************************************************
 * libcasio/fs.h -- libcasio filesystems.
 * Copyright (C) 2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of libcasio.
 * libcasio is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License,
 * or (at your option) any later version.
 *
 * libcasio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcasio; if not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************* */
#ifndef  LIBCASIO_FS_H
# define LIBCASIO_FS_H
# include "cdefs.h"
# include "stream.h"
CASIO_BEGIN_NAMESPACE

/* forward structure declarations (don't mind) */
struct         casio_filesystem_s;
typedef struct casio_filesystem_s casio_filesystem_t;
struct         casio_fsfuncs_s;
typedef struct casio_fsfuncs_s    casio_fsfuncs_t;
struct         casio_stat_s;
typedef struct casio_stat_s       casio_stat_t;
/* ************************************************************************* */
/*  Filesystem file entry                                                    */
/* ************************************************************************* */
/* This structure defines file metadata. */

# define CASIO_MODE_IFMT  0xF000
# define CASIO_MODE_IFREG 0x1000
# define CASIO_MODE_IFDIR 0x2000

typedef unsigned long casio_mode_t;

struct casio_stat_s {
	casio_mode_t casio_stat_mode;
	casio_off_t  casio_stat_size;
};

typedef void casio_fs_list_func_t OF((void *casio__cookie,
	const void *casio__name, const casio_stat_t *casio__stat));
/* ************************************************************************* */
/*  Filesystem description                                                   */
/* ************************************************************************* */
/* The filesystem is a private structure that represents a file system. */

typedef int casio_fs_stat_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__path[], casio_stat_t *casio__stat));
typedef int casio_fs_makedir_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__path[]));
typedef int casio_fs_del_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__path[]));
typedef int casio_fs_move_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__orig[], const void *casio__new[]));

typedef int casio_fs_open_t
	OF((void *casio__cookie, const void *casio__path, casio_off_t size,
		casio_openmode_t casio__mode, casio_stream_t **stream));

typedef int casio_fs_list_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__dirpath[],
		casio_fs_list_func_t *casio__callback, void *casio__cbcookie));

typedef int casio_fs_close_t OF((void *casio__cookie));

struct casio_fsfuncs_s {
	casio_fs_stat_t     *casio_fsfuncs_stat;
	casio_fs_makedir_t  *casio_fsfuncs_makedir;
	casio_fs_del_t      *casio_fsfuncs_del;
	casio_fs_move_t     *casio_fsfuncs_move;

	casio_fs_list_t     *casio_fsfuncs_list;
	casio_fs_open_t     *casio_fsfuncs_open;

	casio_fs_close_t    *casio_fsfuncs_close;
};
/* ************************************************************************* */
/*  Filesystem public functions                                              */
/* ************************************************************************* */
CASIO_BEGIN_DECLS

CASIO_EXTERN int CASIO_EXPORT casio_open_fs
	OF((casio_filesystem_t **casio__fs,
		void *casio__cookie, const char *casio__dirsep,
		const casio_fsfuncs_t *casio__funcs));

CASIO_EXTERN int CASIO_EXPORT casio_makedir
	OF((casio_filesystem_t *casio__fs, const char *casio__path));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_FS_H */
