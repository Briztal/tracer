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

#include <kernel/driver/driver.h>
#include <kernel/krnl.h>
#include <string.h>
#include <kernel/panic.h>
#include <kernel/debug/debug.h>
#include <kernel/struct/nlist.h>
#include "dfs.h"

static struct nlist files = {
	.elements = 0,
	.name_max_length = 32,
};


//Add a file in the file system;
void dfs_create(const char *const name, const enum dfs_file_type type,
				const struct dfs_file_operations *const operations, void *const resource) {

	//Create the file initializer;
	struct dfs_file init = {

		//File closed;
		.file_open = false,

		//Save the type;
		.type = type,

		//Reference the operations structure;
		.operations = operations,

		//Transfer the ownership of the resource;
		.resource = resource,

	};


	//Allocate the memory for the file;
	struct dfs_file *file = kialloc(sizeof(struct dfs_file), &init);

	//Add to the files list;
	nlist_add(&files, name, file);

}


static void dfs_delete(struct dfs_file *file, const char *name) {

	file_delete(file);

	/*
	 * Now delete the file;
	 */

	nlist_remove(&files, name);

}


//Delete a file from the file system;
bool dfs_remove(const char *name) {

	//Search for the required file;
	struct dfs_file *file = nlist_search(&files, name);

	//If the file doesn't exist, stop here;
	if (!file) return true;

	//If the file is close, mark it for deletion and fail; file will not be openable anymore;
	if (file->file_open) {

		return false;

	} else {

		//If not, delete it;
		dfs_delete(file, name);
		return true;

	}

}


struct dfs_file *dfs_open(const char *name) {

	struct dfs_file *file = nlist_search(&files, name);

	if (!file) {
		return 0;
	}

	//If the file is opened :
	if (file->file_open) {

		//Fail, no concurrent access;
		return 0;

	}

	//Mark the file opened;
	file->file_open = true;

	//Call the open operation;
	file_op_open(file);

	//Return the file;
	return file;

}


void dfs_close(struct dfs_file *file) {

	//If the file is null :
	if (!file) {

		kernel_panic("dfs_close : null file provided;");

		return;

	}

	//Call the close operation;
	file_op_close(file);

	//If not, mark it closed;
	file->file_open = false;

}


//List all files;
void dfs_list() {

	nlist_list(&files);

}

