//
// Created by root on 9/3/18.
//

#ifndef TRACER_DFS_H
#define TRACER_DFS_H


#include <util/type.h>

/*
 * The file system, as its name doesn't state, references memory zones, that can have different types;
 */

enum dfs_file_type {

	//The memory zone is a driver struct;
		DFS_DRIVER,

	//The memory zone is an interface struct;
		DFS_INTERFACE,

};


struct dfs_file {

	//files are linked;
	struct list_head head;

	//The deletion status;
	bool to_delete;

	//The name of the file;
	const char *name;

	//The status of the file;
	bool file_open;

	//The type of resource;
	enum dfs_file_type type;

	//The resource itself;
	void *const resource;

};


//Add a file in the file system;
void dfs_create(const char *name, enum dfs_file_type type, void * resource);

//Open a file;
struct dfs_file *dfs_open(const char *name);

//Close a file;
void dfs_close(struct dfs_file *file);

//Remove a file; Will be deleted is file closed;
void dfs_remove(const char *name);


#endif //TRACER_DFS_H
