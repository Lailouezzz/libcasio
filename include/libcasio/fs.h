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
	size_t            casio_pathnode_size;
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
/* This structure defines file metadata.
 * Here are the flags: */

/* TODO */

/* And here are the "file" types ("file" is between quotes as on Windows,
 * directories are not files like on Unix) you can find: */

# define CASIO_STAT_TYPE_REG   0x0001 /* Regular file. */
# define CASIO_STAT_TYPE_DIR   0x0002 /* Directory. */
# define CASIO_STAT_TYPE_LNK   0x0004 /* Symbolic link. */
# define CASIO_STAT_TYPE_CHAR  0x0008 /* Character device. */
# define CASIO_STAT_TYPE_BLK   0x0010 /* Block device. */
# define CASIO_STAT_TYPE_SOCK  0x0020 /* Socket. */

/* And here is the stat structure.
 * The path is not in it. */

struct casio_stat_s {
	unsigned int casio_stat_flags;
	unsigned int casio_stat_type;
	casio_off_t  casio_stat_size;
};

typedef void casio_fs_list_func_t OF((void *casio__cookie,
	const casio_pathnode_t *casio__node, const casio_stat_t *casio__stat));
/* ************************************************************************* */
/*  Filesystem description                                                   */
/* ************************************************************************* */
/* The filesystem is a private structure that represents a file system.
 *
 * Once all of the operations are done, the user shall close the filesystem.
 * This is the callback to free/close custom elements. */

typedef int casio_fs_close_t OF((void *casio__cookie));

/* From here, we will work with paths.
 * Instead of having to convert the path every time, we will use native paths
 * on the user side; they can be made how you want. */

typedef int  casio_fs_makepath_t
	OF((void *casio__cookie, void **casio__native_path,
		casio_path_t *casio__path));
typedef void casio_fs_freepath_t
	OF((void *casio__cookie, void *casio__native_path));

/* The `casio_fs_stat` callback is used to gather information about
 * any element in your filesystem: files, directories, ... */

typedef int casio_fs_stat_t
	OF((void *casio__cookie, void *casio__native_path,
		casio_stat_t *casio__stat));

/* The `casio_fs_makedir` callback is used to create a directory. */

typedef int casio_fs_makedir_t
	OF((void *casio__cookie, void *casio__native_path));

/* The `casio_fs_del` callback is used to delete an element of any type
 * from your filesystem. */

typedef int casio_fs_del_t
	OF((void *casio__cookie, void *casio__native_path));

/* The `casio_fs_move` callback is used to move an element of your filesystem
 * to an other location. */

typedef int casio_fs_move_t
	OF((void *casio__cookie, void *casio__native_orig,
		void *casio__native_dest));

/* The `casio_fs_open` callback is used to make a stream out of an element
 * of your filesystem. */

typedef int casio_fs_open_t
	OF((void *casio__cookie, void *casio__path, casio_off_t size,
		casio_openmode_t casio__mode, casio_stream_t **stream));

/* The `casio_fs_list` callback is used to list files in a directory. */

typedef int casio_fs_list_t
	OF((void *casio__cookie, casio_path_t *casio__path,
		casio_fs_list_func_t *casio__callback, void *casio__cbcookie));

/* And here is the structure with all of the functions.
 * It is the one used when you want to open a libcasio filesystem interface
 * abstraction. */

struct casio_fsfuncs_s {
	casio_fs_close_t    *casio_fsfuncs_close;
	casio_fs_makepath_t *casio_fsfuncs_makepath;
	casio_fs_freepath_t *casio_fsfuncs_freepath;

	casio_fs_stat_t     *casio_fsfuncs_stat;
	casio_fs_makedir_t  *casio_fsfuncs_makedir;
	casio_fs_del_t      *casio_fsfuncs_del;
	casio_fs_move_t     *casio_fsfuncs_move;

	casio_fs_list_t     *casio_fsfuncs_list;
	casio_fs_open_t     *casio_fsfuncs_open;
};
/* ************************************************************************* */
/*  Filesystem public functions                                              */
/* ************************************************************************* */
CASIO_BEGIN_DECLS

/* Open a custom filesystem. */

CASIO_EXTERN int CASIO_EXPORT casio_open_fs
	OF((casio_filesystem_t **casio__filesystem,
		void *casio__cookie, const casio_fsfuncs_t *casio__funcs));

/* Manipulate native paths. */

CASIO_EXTERN int  CASIO_EXPORT casio_make_native_path
	OF((casio_filesystem_t *casio__filesystem,
		void **casio__native_path, casio_path_t *casio__path));
CASIO_EXTERN void CASIO_EXPORT casio_free_native_path
	OF((casio_filesystem_t *casio__filesystem,
		void  *casio__native_path));

/* Make a directory. */

CASIO_EXTERN int CASIO_EXPORT casio_makedir
	OF((casio_filesystem_t *casio__fs, casio_path_t *casio__path));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_FS_H */
