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
# include <time.h>
CASIO_BEGIN_NAMESPACE

/* Forward structure declarations (don't mind). */

struct         casio_fs_s;
typedef struct casio_fs_s        casio_fs_t;
struct         casio_fsfuncs_s;
typedef struct casio_fsfuncs_s   casio_fsfuncs_t;
struct         casio_pathnode_s;
typedef struct casio_pathnode_s  casio_pathnode_t;
struct         casio_path_s;
typedef struct casio_path_s      casio_path_t;
struct         casio_stat_s;
typedef struct casio_stat_s      casio_stat_t;

/* ---
 * Filesystem file path.
 * --- */

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

/* Path nodes should be managed using the following functions: */

CASIO_EXTERN int  CASIO_EXPORT casio_make_pathnode
	OF((casio_pathnode_t **casio__node, size_t casio__size));
CASIO_EXTERN void CASIO_EXPORT casio_free_pathnode
	OF((casio_pathnode_t  *casio__node));
CASIO_EXTERN int  CASIO_EXPORT casio_duplicate_pathnode
	OF((casio_pathnode_t **casio__new_node,
		const casio_pathnode_t *casio__old_node));

/* And here is the main path structure. */

#define casio_pathflag_alloc 0x0001 /* path object is valid and allocated */
#define casio_pathflag_rel   0x0002 /* relative path */

struct casio_path_s {
	unsigned int      casio_path_flags;
	const char       *casio_path_device;
	casio_pathnode_t *casio_path_nodes;
};

/* ---
 * Filesystem file entry.
 * --- */

/* This structure defines file metadata.
 * Here are the flags:
 * `CASIO_STAT_FLAG_PERM`:  stat permissions are set;
 * `CASIO_STAT_FLAG_BTIME`: birth time is set;
 * `CASIO_STAT_FLAG_ATIME`: last accessed time is set;
 * `CASIO_STAT_FLAG_MTIME`: last modif. time is set. */

# define CASIO_STAT_FLAG_PERM  0x0001
# define CASIO_STAT_FLAG_BTIME 0x0002
# define CASIO_STAT_FLAG_ATIME 0x0004
# define CASIO_STAT_FLAG_MTIME 0x0008

/* Elements can have Unix-like permissions. Here they are:
 * `CASIO_STAT_PERM_IRUSR`: user   has  read  permission;
 * `CASIO_STAT_PERM_IWUSR`: user   has  write permission;
 * `CASIO_STAT_PERM_IXUSR`: user   has  exec  permission;
 * `CASIO_STAT_PERM_IRGRP`: group  has  read  permission;
 * `CASIO_STAT_PERM_IWGRP`: group  has  write permission;
 * `CASIO_STAT_PERM_IXGRP`: group  has  exec  permission;
 * `CASIO_STAT_PERM_IROTH`: others have read  permission;
 * `CASIO_STAT_PERM_IWOTH`: others have write permission;
 * `CASIO_STAT_PERM_IXOTH`: others have exec  permission. */

# define CASIO_STAT_PERM_IRUSR 0x0001
# define CASIO_STAT_PERM_IWUSR 0x0002
# define CASIO_STAT_PERM_IXUSR 0x0004
# define CASIO_STAT_PERM_IRGRP 0x0010
# define CASIO_STAT_PERM_IWGRP 0x0020
# define CASIO_STAT_PERM_IXGRP 0x0040
# define CASIO_STAT_PERM_IROTH 0x0100
# define CASIO_STAT_PERM_IWOTH 0x0200
# define CASIO_STAT_PERM_IXOTH 0x0400

/* And here are the "file" types ("file" is between quotes as on Windows,
 * directories are not files like on Unix) you can find:
 * `CASIO_STAT_TYPE_REG`:  regular file;
 * `CASIO_STAT_TYPE_DIR`:  directory;
 * `CASIO_STAT_TYPE_LNK`:  symbolic link;
 * `CASIO_STAT_TYPE_CHAR`: character device;
 * `CASIO_STAT_TYPE_BLK`:  block device;
 * `CASIO_STAT_TYPE_SOCK`: socket */

# define CASIO_STAT_TYPE_REG   0x0001
# define CASIO_STAT_TYPE_DIR   0x0002
# define CASIO_STAT_TYPE_LNK   0x0004
# define CASIO_STAT_TYPE_CHAR  0x0008
# define CASIO_STAT_TYPE_BLK   0x0010
# define CASIO_STAT_TYPE_SOCK  0x0020

/* And here is the stat structure. The elements it contains at runtime depend
 * on the flags (useful for binary compatibility and filesystem metadata type).
 * For example, if `~thestat.casio_stat_flags & CASIO_STAT_FLAG_PERM`,
 * then you shouldn't try to read the permissions, as it might contain crap.
 *
 * The elements are the following:
 * [ ] `casio_stat_type`:  the file type (see the `CASIO_STAT_TYPE_*` macros);
 * [X] `casio_stat_perm`:  Unix-like permissions (see `CASIO_STAT_PERM_*`);
 * [X] `casio_stat_size`:  the file size;
 * [X] `casio_stat_btime`: the file's birth time;
 * [X] `casio_stat_atime`: the file's last access time;
 * [X] `casio_stat_mtime`: the file's last modification time.
 *
 * Checked elements should not be read without checking that the corresponding
 * flag is set (compatibility and filesystem feature reasons).
 *
 * The file path/name is not in it, because it is:
 * - given by yourself when you are querying file information;
 * - given beside with listing callbacks. */

struct casio_stat_s {
	unsigned short casio_stat_flags;
	unsigned short casio_stat_type;
	unsigned short casio_stat_perm;
	unsigned short casio_stat__reserved;

	casio_off_t    casio_stat_size;
	time_t         casio_stat_btime;
	time_t         casio_stat_atime;
	time_t         casio_stat_mtime;
};

/* Here is the file listing callback type you will give to `casio_list`.
 *
 * If you want to copy the path node, do not copy the pointer as it will
 * probably disappear afterwards, duplicate the node using
 * `casio_duplicate_pathnode`! */

typedef void CASIO_EXPORT casio_fs_list_func_t OF((void *casio__cookie,
	const casio_pathnode_t *casio__node, const casio_stat_t *casio__stat));

/* ---
 * Filesystem description.
 * --- */

/* The filesystem is a private structure that represents a file system.
 *
 * Once all of the operations are done, the user shall close the filesystem.
 * This is the callback to free/close custom elements. */

typedef int CASIO_EXPORT casio_fs_close_t OF((void *casio__cookie));

/* From here, we will work with paths.
 * Instead of having to convert the path every time, we will use native paths
 * on the user side; they can be made how you want. */

typedef int  CASIO_EXPORT casio_fs_makepath_t
	OF((void *casio__cookie, void **casio__native_path,
		casio_path_t *casio__path));
typedef void CASIO_EXPORT casio_fs_freepath_t
	OF((void *casio__cookie, void *casio__native_path));

/* The `casio_fs_stat` callback is used to gather information about
 * any element in your filesystem: files, directories, ... */

typedef int CASIO_EXPORT casio_fs_stat_t
	OF((void *casio__cookie, void *casio__native_path,
		casio_stat_t *casio__stat));

/* The `casio_fs_make` callback is used to create an element.
 * The arguments depend on the file type:
 * - `CASIO_STAT_TYPE_REG`: casio_off_t size;
 * - `CASIO_STAT_TYPE_LNK`: void *casio__destination_path. */

# if defined(__STDC__) && __STDC__
typedef int CASIO_EXPORT casio_fs_make_t(void *casio__cookie,
	void *casio__native_path, const casio_stat_t *casio__stat, ...);
# else
typedef int CASIO_EXPORT casio_fs_make_t();
# endif

/* The `casio_fs_del` callback is used to delete an element of any type
 * from your filesystem. */

typedef int CASIO_EXPORT casio_fs_del_t
	OF((void *casio__cookie, void *casio__native_path));

/* The `casio_fs_move` callback is used to move an element of your filesystem
 * to an other location. */

typedef int CASIO_EXPORT casio_fs_move_t
	OF((void *casio__cookie, void *casio__native_orig,
		void *casio__native_dest));

/* The `casio_fs_open` callback is used to make a stream out of an element
 * of your filesystem. */

typedef int CASIO_EXPORT casio_fs_open_t
	OF((void *casio__cookie, void *casio__path, casio_off_t size,
		casio_openmode_t casio__mode, casio_stream_t **stream));

/* The `casio_fs_list` callback is used to list files in a directory. */

typedef int CASIO_EXPORT casio_fs_list_t
	OF((void *casio__cookie, casio_path_t *casio__path,
		casio_fs_list_func_t *casio__callback, void *casio__cbcookie));

/* The `casio_fs_getfreemem` callback is used to get the free space */
typedef int CASIO_EXPORT casio_fs_getfreemem_t
	OF((void *casio__cokie, casio_path_t *casio__path,
		size_t *casio__capacity));

/* The `casio_fs_optim` callback is used to optimize the filesystem
 * (run a defragmentation, …).
 *
 * FIXME: we need a `casio_path_t` equivalent to express devices.
 * On CASIOWIN, the device is expressed as a path node/string (e.g.
 * "fls0" in \fls0\file.txt), on WINDOWS, it can be expressed as a
 * letter (e.g. C:), a path from the Local Device \\.\ or
 * Root Local Device \\?\, or a mount point, and on Linux/Mac, as a
 * mount point. This is a mess we need to clean up sometime, for now
 * we're only considering this operation for CASIOWIN. */

typedef int CASIO_EXPORT casio_fs_optim_t
	OF((void *casio__cookie, const char *device));

/* And here is the structure with all of the functions.
 * It is the one used when you want to open a libcasio filesystem interface
 * abstraction. */

struct casio_fsfuncs_s {
	casio_fs_close_t    	*casio_fsfuncs_close;
	casio_fs_makepath_t 	*casio_fsfuncs_makepath;
	casio_fs_freepath_t 	*casio_fsfuncs_freepath;

	casio_fs_stat_t     	*casio_fsfuncs_stat;
	casio_fs_getfreemem_t	*casio_fsfuncs_getfreemem;
	casio_fs_make_t     	*casio_fsfuncs_make;
	casio_fs_del_t      	*casio_fsfuncs_del;
	casio_fs_move_t     	*casio_fsfuncs_move;

	casio_fs_list_t     	*casio_fsfuncs_list;
	casio_fs_open_t     	*casio_fsfuncs_open;

	casio_fs_optim_t    	*casio_fsfuncs_optim;
};

/* ---
 * Filesystem public functions.
 * --- */

CASIO_BEGIN_DECLS

/* Open a custom filesystem, and close any filesystem. */

CASIO_EXTERN int CASIO_EXPORT casio_open_fs
	OF((casio_fs_t **casio__filesystem,
		void *casio__cookie, const casio_fsfuncs_t *casio__funcs));
CASIO_EXTERN int CASIO_EXPORT casio_close_fs
	OF((casio_fs_t *casio__filesystem));

/* Manipulate native paths. */

CASIO_EXTERN int  CASIO_EXPORT casio_make_native_path
	OF((casio_fs_t *casio__filesystem,
		void **casio__native_path, casio_path_t *casio__path));
CASIO_EXTERN void CASIO_EXPORT casio_free_native_path
	OF((casio_fs_t *casio__filesystem,
		void  *casio__native_path));

/* Make a directory. */

CASIO_EXTERN int CASIO_EXPORT casio_makedir
	OF((casio_fs_t *casio__fs, casio_path_t *casio__path));
CASIO_EXTERN int CASIO_EXPORT casio_makedir_nat
	OF((casio_fs_t *casio__fs, void *casio__path));

/* Delete an element. */

CASIO_EXTERN int CASIO_EXPORT casio_delete
	OF((casio_fs_t *casio__fs, casio_path_t *casio__path));
CASIO_EXTERN int CASIO_EXPORT casio_delete_nat
	OF((casio_fs_t *casio__fs, void *casio__path));

/* Open a stream. */

CASIO_EXTERN int CASIO_EXPORT casio_open
	OF((casio_fs_t *casio__fs, casio_stream_t **casio__stream,
		casio_path_t *casio__path, casio_off_t casio__size,
		casio_openmode_t casio__mode));
CASIO_EXTERN int CASIO_EXPORT casio_open_nat
	OF((casio_fs_t *casio__fs, casio_stream_t **casio__stream,
		void *casio__path, casio_off_t casio__size,
		casio_openmode_t casio__mode));

/* Optimize the filesystem. */

CASIO_EXTERN int CASIO_EXPORT casio_optimize
	OF((casio_fs_t *casio__fs, const char *casio__device));

/* List all files/directories */

CASIO_EXTERN int CASIO_EXPORT casio_list
	OF((casio_fs_t *casio__fs, casio_path_t *casio__path,
		casio_fs_list_func_t *casio__callback, void *casio__cbcookie));

/* Get the free space */

CASIO_EXTERN int CASIO_EXPORT casio_getfreemem
	OF((casio_fs_t *casio__fs, casio_path_t *casio__path,
		size_t *casio__capacity));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_FS_H */
