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

struct dfs_file;


struct dfs_file_operations {

	//Open the file;
	void (*open)();

	//Close the file;
	void (*close)();

	//Get a copy of the file's internal structure. The passed pointer will be
	bool (*interface)(void *data, size_t size);

};



struct dfs_file {

	//files are linked;
	struct list_head head;

	//The name of the file;
	const char *const name;

	//The status of the file;
	bool file_open;

	//The type of resource;
	const enum dfs_file_type type;

	//The associated file operation struct;
	const struct dfs_file_operations *const operations;

	//The resource itself;
	void *const resource;

};


static inline void file_op_open(struct dfs_file *file) {

	//Cache the function;
	void (*open)() = file->operations->open;

	//If the function is not null, call it;
	if (open) { (*open)(); }

}


static inline void file_op_close(struct dfs_file *file) {

	//Cache the function;
	void (*close)() = file->operations->close;

	//If the function is not null, call it;
	if (close) { (*close)(); }

}


static inline bool file_op_interface(struct dfs_file *file, void *data, size_t size) {

	//Cache the function;
	bool (*interface)(void *, size_t) = file->operations->interface;

	//If the function is not null, call it;
	if (interface) {return (*interface)(data, size); }

	//If null pointer, fail;
	return false;

}



//Add a file in the file system;
void
dfs_create(const char *name, enum dfs_file_type type, const struct dfs_file_operations *operations, void *resource);

//Open a file;
struct dfs_file *dfs_open(const char *name);

//Close a file;
void dfs_close(struct dfs_file *file);

//Remove a file; Will be deleted is file closed;
bool dfs_remove(const char *name);


#endif //TRACER_DFS_H
