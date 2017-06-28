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
struct         casio_pathnode_s;
typedef struct casio_pathnode_s   casio_pathnode_t;
struct         casio_path_s;
typedef struct casio_path_s       casio_path_t;
struct         casio_stat_s;
typedef struct casio_stat_s       casio_stat_t;
/* ************************************************************************* */
/*  Filesystem file path                                                     */
/* ************************************************************************* */
/* We ought to make a file path abstraction, that take into account all of the
 * existing file systems, with devices, namespaces, and their character
 * encodings.
 *
 * The path node is a path element, generally a file/folder/else name.
 * It is stored as a linked list. */

struct casio_pathnode_s {
	casio_pathnode_t *casio_pathnode_next;
	unsigned char     casio_pathnode_name[2048];
};

/* And here is the main path structure. */

#define casio_pathflag_alloc 0x0001 /* path object is valid and allocated */
#define casio_pathflag_rel   0x0002 /* relative path */

struct casio_path_s {
	unsigned int      casio_path_flags;
	const char       *casio_path_device;
	casio_pathnode_t *casio_path_nodes;
};
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
/* The filesystem is a private structure that represents a file system.
 *
 * The `casio_fs_stat` callback is to get information about a file or
 * directory. */

typedef int casio_fs_stat_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__path[], casio_stat_t *casio__stat));

/* The `casio_fs_makedir` callback creates a directory. */

typedef int casio_fs_makedir_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__path[]));

/* The `casio_fs_del` callback deletes a file or directory. */

typedef int casio_fs_del_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__path[]));

/* The `casio_fs_move` callback renames/moves a file into an
 * other directory, eventually with an other name. */

typedef int casio_fs_move_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__orig[], const void *casio__new[]));

/* The `casio_fs_open` callback makes a stream out of a file with a path. */

typedef int casio_fs_open_t
	OF((void *casio__cookie, const void *casio__path, casio_off_t size,
		casio_openmode_t casio__mode, casio_stream_t **stream));

/* The `casio_fs_list` callback lists files in a directory. */

typedef int casio_fs_list_t
	OF((void *casio__cookie, const char *casio__device,
		const void *casio__dirpath[],
		casio_fs_list_func_t *casio__callback, void *casio__cbcookie));

/* The `casio_fs_close` callback is called when the filesystem interface
 * is closed. */

typedef int casio_fs_close_t OF((void *casio__cookie));

/* And here is the structure with all of the functions.
 * It is the one used when you want to open a libcasio filesystem interface
 * abstraction. */

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
