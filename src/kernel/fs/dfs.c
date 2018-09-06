//
// Created by root on 9/3/18.
//

#include <kernel/driver/driver.h>
#include <kernel/krnl.h>
#include <string.h>
#include "dfs.h"

struct dfs_file *first_file;


//Add a file in the file system;
void dfs_create(const char *const name, const enum dfs_file_type type,
				const struct dfs_file_operations *const operations, void *const resource) {

	//Check that the file is not null;
	if (!resource) {

		kernel_panic("dfs_create : null resource ptr");

	}

	//Allocate the memory for the file;
	struct dfs_file *file = kmalloc(sizeof(struct dfs_file));

	//Create the file initializer;
	struct dfs_file init = {

		//Link to itself;
		.head = {
			.prev = file,
			.next = file,
		},


		//Duplicate the name;
		.name = kialloc(strlen(name), name),

		//File closed;
		.file_open = false,

		//Save the type;
		.type = type,

		//Reference the operations structure;
		.operations = operations,

		//Transfer the ownership of the resource;
		.resource = resource,

	};

	//Initialise;
	memcpy(file, &init, sizeof(struct dfs_file));

	/*
	 * Add to the files list;
	 */

	//If the first file is null :
	if (!first_file) {

		//Set to be the first file;
		first_file = file;

	} else {

		//If not, insert with no order;
		list_concat((struct list_head *) file, (struct list_head *) first_file);

	}

}


//Get a file from the file system;
void *dfs_search(const char *name) {

	//Cache the current file, as the first file;
	struct dfs_file *const first = first_file, *file = first;

	//If there are no files :
	if (!file) {
		return 0;
	}

	//For each file :
	do {

		//If names match :
		if (!strcmp(name, file->name)) {

			//Return the file;
			return file;

		}

		//If not, focus on the next file;
		file = file->head.next;

	} while (file != first);

}


void dfs_delete(struct dfs_file *file) {

	//Delete the resource;
	switch (file->type) {
		case DFS_DRIVER:

			//Delete the driver;
			driver_delete(file->resource);
			break;

		case DFS_INTERFACE:

			break;
		default:
			kernel_panic("dfs_delete : unknown file type;");
	}


	/*
	 * Now delete the file;
	 */

	//If the file is the first file :
	if (first_file == file) {

		//Cache the file's successor;
		struct dfs_file *next = file->head.next;

		//If the file has a successor :
		if (next != file) {

			first_file = next;

		} else {

			first_file = 0;
		}

	}

	//Remove the file from the list;
	list_remove((struct list_head *) file);

}


//Delete a file from the file system;
bool dfs_remove(const char *name) {

	//Search for the required file;
	struct dfs_file *file = dfs_search(name);

	//If the file doesn't exist, stop here;
	if (!file) return true;

	//If the file is close, mark it for deletion and fail; file will not be openable anymore;
	if (file->file_open) {

		return false;

	} else {

		//If not, delete it;
		dfs_delete(file);
		return true;

	}

}


struct dfs_file *dfs_open(const char *name) {

	struct dfs_file *file = dfs_search(name);

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

