/*
  dfs.h Part of TRACER

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

#ifndef TRACER_DFS_H
#define TRACER_DFS_H


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


//Declare the inode ops structure;
struct inode_ops;

//The inode struct contains a status and a set of ops;
struct inode {

	//The status of the node;
	bool open;
	
	//TODO THE MODULE THAT CREATES THE FILE;

	//The associated node operation struct;
	const struct inode_ops *const ops;

};

//Initializer;
#define INODE(inode_ops) {.open = false, .ops = (inode_ops)}


/*
 * The inode ops structure contains a inode's accessible functions;
 */

struct inode_ops {

	//General operations;
	
	//Open the inode;
	void (*const open)(struct inode *node);
	
	//Close the inode;
	void (*const close)(struct inode *node);
	
	
	//Device operations;
	
	//Initialise the resource.
	bool (*const init)(struct inode *node, const void *config, size_t config_size);
	
	//Interface with the resource;
	bool (*const interface)(struct inode *node, void *iface_struct, size_t size);
	
	//Reset the resource.
	void (*const reset)(struct inode *node);
	
	
	//The deletion function;
	void (*const deleter)(struct inode *);
	
};


//A file descriptor is an inode pointer;
typedef size_t file_descriptor;


//A device file implements device operations, and can be in one of the following states;
enum dev_status {
	
	DEV_RESET,
	
	DEV_INITIALISED,
	
	DEV_INTERFACED,
	
};


//-------------------------------------------------- Inodes operations -------------------------------------------------

bool iop_init(file_descriptor fd, const void *cfg, size_t size);

bool iop_interface(file_descriptor fd, void *data, size_t size);

void iop_reset(file_descriptor fd);



//----------------------------------------------- File system operations -----------------------------------------------

//Add a node in the file system;
void fs_create(const char *name, struct inode *node);

//Remove a node; Will be deleted if node closed;
bool fs_remove(const char *name);

//Open a node;
file_descriptor fs_open(const char *name);

//Close a node;
void fs_close(file_descriptor fd);



//List all files;
void fs_list();


#endif //TRACER_DFS_H
