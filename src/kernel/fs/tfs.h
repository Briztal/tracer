//
// Created by root on 9/2/18.
//

#ifndef TRACER_TSF_H
#define TRACER_TSF_H

#include <util/type.h>

struct inode;
struct dentry;


/*
 * A file system is an interface, offering file operation capabilities;
 */

struct file_system {

	//List children of an inode;
	void (*list)(struct file_system *, struct dentry *);

	//Create an inode's associated dentry, and add it to an existing dentry's children;
	void (*add)(struct file_system *, struct dentry *, struct inode *);

	//Delete a dentry, and its associated inode;
	void (*delete)(struct file_system *, struct dentry *);

};


/*
 * The file system, as its name doesn't state, references memory zones, that can have different types;
 */

enum memory_type {

	//The memory zone is a file; This is the only regular type, that has a meaning on its own;
		MEM_FILE,

	//The memory zone is a folder; No content;
		MEM_FOLDER,

	//The memory zone is a interface struct;
		MEM_DRIVER,

	//The memory zone is a
		MEM_INTERFACE,

};


/*
 * An inode provides information about a memory zone;
 */

struct inode {

	//The file system it belongs to;
	struct file_system *fs;

	//A dentry it is owned by;
	struct dentry *owner;

	//The type of the referenced memory zone;
	enum memory_type mem_type;

	//The size of the memory zone; Has a meaning only for a file;
	size_t mem_size;

	//The address of the memory zone;
	void *memory_start;

};


/*
 * A dentry stores information about the memory architecture;
 */

struct dentry {

	//The name of the dentry;
	const char *name;

	//The dentry's parent;
	struct dentry parent;

	//The list of dentries that have the same parent;
	struct list_head *pairs;

	//The inode we reference;
	struct inode *inode;

};


#endif //TRACER_TSF_H
