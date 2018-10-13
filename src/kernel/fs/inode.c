/*
  dfs.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>

#include <string.h>

#include <kernel/panic.h>

#include <kernel/struct/nlist.h>

#include "inode.h"


//----------------------------------------------- Private file operations ----------------------------------------------

/**
 * inode_open : Calls the open file operation on the given file.
 *
 * 	Called automatically when a file is opened;
 *
 * @param fd : the file descriptor;
 */

static void inode_open(file_descriptor fd) {

	//Cache the function;
	void (*open)(struct inode *) = ((struct inode *) fd)->ops->open;

	//If the function is not null, call it;
	if (open) {
		(*open)(((struct inode *) fd));
	}

}


/**
 * inode_close : Calls the close file operation on the given file.
 *
 * 	Called automatically when a file is closed;
 *
 * @param fd : the file descriptor;
 */

static void inode_close(file_descriptor fd) {

	//Cache the function;
	void (*close)(struct inode *) = ((struct inode *) fd)->ops->close;

	//If the function is not null, call it;
	if (close) { (*close)(((struct inode *) fd)); }

}


/**
 * inode_delete : Calls the delete file operation on the given file.
 *
 * 	Called automatically when a file is removed;
 *
 * @param fd : the file descriptor;
 */

static void inode_delete(file_descriptor fd) {

	void (*del)(struct inode *) = ((struct inode *) fd)->ops->deleter;

	if (del) {
		(*del)(((struct inode *) fd));
	}

}


//----------------------------------------------- Public file operations -----------------------------------------------


bool inode_configure(file_descriptor fd, void *data, size_t size) {

	//Cache the function;
	bool (*configure)(struct inode *, const void *, size_t) = ((struct inode *) fd)->ops->configure;

	//If the function is not null, call it;
	if (configure) { return (*configure)(((struct inode *) fd), data, size); }

	//If null pointer, fail;
	return false;

}


void inode_execute(file_descriptor fd, size_t function_id, const void *args, size_t args_size) {

	//Cache the function;
	void (*execute)(struct inode *, size_t, const void *, size_t) = ((struct inode *) fd)->ops->execute;

	//If the function is not null, call it;
	if (execute) { return (*execute)(((struct inode *) fd), function_id, args, args_size); }

}


bool inode_interface(file_descriptor fd, void *data, size_t size) {

	//Cache the function;
	bool (*interface)(struct inode *, void *, size_t) = ((struct inode *) fd)->ops->interface;

	//If the function is not null, call it;
	if (interface) { return (*interface)(((struct inode *) fd), data, size); }

	//If null pointer, fail;
	return false;

}




static struct nlist files = {
	.elements = 0,
	.name_max_length = 32,
};


//---------------------------------------------------- File system -----------------------------------------------------


//Add a file in the file system;
void fs_create(const char *name, struct inode *const node) {

	//Add to the files list;
	nlist_add(&files, name, node);

}

//
static void fs_delete(struct inode *node, const char *name) {

	inode_delete((file_descriptor) node);

	/*
	 * Now delete the file;
	 */

	nlist_remove(&files, name);

}


//Delete a file from the file system;
bool fs_remove(const char *name) {

	//Search for the required file;
	struct inode *file = nlist_get(&files, name);

	//If the file doesn't exist, stop here;
	if (!file) return true;

	//If the inode is opened, fail;
	if (file->open) {

		return false;

	} else {

		//If not, delete it;
		fs_delete(file, name);
		return true;

	}

}


file_descriptor fs_open(const char *name) {

	struct inode *node = nlist_get(&files, name);

	if (!node) {
		return 0;
	}

	//If the file is opened :
	if (node->open) {

		//Fail, no concurrent access;
		return 0;

	}

	//Mark the file opened;
	node->open = true;

	//Call the open operation;
	inode_open((file_descriptor) node);

	//Return the file;
	return (file_descriptor) node;

}


void fs_close(file_descriptor fd) {

	struct inode *node = (struct inode *)fd;

	//If the file is null :
	if (!node) {

		kernel_panic("fs_close : null file provided;");

		return;

	}

	//Call the close operation;
	inode_close(fd);

	//If not, mark it closed;
	node->open = false;

}


//List all files;
void fs_list() {

	nlist_list(&files);

}

