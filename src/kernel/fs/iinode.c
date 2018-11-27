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

#include <string.h>

#include <panic.h>

#include <struct/nlist.h>

#include "iinode.h"

#include "fs.h"


//--------------------------------------------------- Global variable --------------------------------------------------

static struct nlist files = {
	.elements = 0,
	.name_max_length = 32,
};


//----------------------------------------------- Private file operations ----------------------------------------------

/**
 * iop_open : Calls the open file operation on the given file.
 *
 * 	Called automatically when a file is opened;
 *
 * @param fd : the file descriptor;
 */

static void iop_open(file_descriptor fd) {

	//Cache the function;
	void (*open)(struct iinode *) = ((struct iinode *) fd)->ops->open;

	//If the function is not null, call it;
	if (open) {
		(*open)(((struct iinode *) fd));
	}

}


/**
 * iop_close : Calls the close file operation on the given file.
 *
 * 	Called automatically when a file is closed;
 *
 * @param fd : the file descriptor;
 */

static void iop_close(file_descriptor fd) {

	//Cache the function;
	void (*close)(struct iinode *) = ((struct iinode *) fd)->ops->close;

	//If the function is not null, call it;
	if (close) { (*close)(((struct iinode *) fd)); }

}


/**
 * iop_delete : Calls the delete file operation on the given file.
 *
 * 	Called automatically when a file is removed;
 *
 * @param fd : the file descriptor;
 */

static void iop_delete(file_descriptor fd) {

	void (*del)(struct iinode *) = ((struct iinode *) fd)->ops->deleter;

	if (del) {
		(*del)(((struct iinode *) fd));
	}

}


//----------------------------------------------- Public file operations -----------------------------------------------


bool iop_init(file_descriptor fd, const void *cfg, size_t size) {

	//Cache the function;
	bool (*configure)(struct iinode *, const void *, size_t) = ((struct iinode *) fd)->ops->init;

	//If the function is not null, call it;
	if (configure) { return (*configure)(((struct iinode *) fd), cfg, size); }

	//If null pointer, complete;
	return true;

}


bool iop_interface(file_descriptor fd, void *data, size_t size) {

	//Cache the function;
	bool (*interface)(struct iinode *, void *, size_t) = ((struct iinode *) fd)->ops->interface;

	//If the function is not null, call it;
	if (interface) { return (*interface)(((struct iinode *) fd), data, size); }

	//If null pointer, complete;
	return true;

}


void iop_reset(file_descriptor fd) {
	
	//Cache the function;
	void (*reset)(struct iinode *) = ((struct iinode *) fd)->ops->reset;
	
	//If the function is not null, call it;
	if (reset) {
		(*reset)(((struct iinode *) fd));
	}
	
}



//---------------------------------------------------- File system -----------------------------------------------------


//Add a file in the file system;
void fs_create(const char *name, struct iinode *const node) {

	//Add to the files list;
	nlist_add(&files, name, node);

}

//
static void fs_delete(struct iinode *node, const char *name) {
	
	iop_delete((file_descriptor) node);

	/*
	 * Now delete the file;
	 */

	nlist_remove(&files, name);

}


//Delete a file from the file system;
bool fs_remove(const char *name) {

	//Search for the required file;
	struct iinode *file = nlist_get(&files, name);

	//If the file doesn't exist, stop here;
	if (!file) return true;

	//If the iinode is opened, fail;
	if (file->open) {

		return false;

	} else {

		//If not, delete it;
		fs_delete(file, name);
		return true;

	}

}


file_descriptor fs_open(const char *name) {

	struct iinode *node = nlist_get(&files, name);

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
	iop_open((file_descriptor) node);

	//Return the file;
	return (file_descriptor) node;

}


void fs_close(file_descriptor fd) {

	struct iinode *node = (struct iinode *)fd;

	//If the file is null :
	if (!node) {

		kernel_panic("fs_close : null file provided;");

		return;

	}
	
	//Call the reset operation;
	iop_reset(fd);

	//Call the close operation;
	iop_close(fd);

	//If not, mark it closed;
	node->open = false;

}


//List all files;
void fs_list() {

	nlist_list(&files);

}

