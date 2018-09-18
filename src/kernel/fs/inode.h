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


#include <util/type.h>


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

	//Open the resource;
	void (*const open)(struct inode *node);

	//Execute a function of the resource;
	void (*const execute)(struct inode *node, size_t function_id, const void *args, size_t args_size);

	//Configure the resource.
	bool (*const configure)(struct inode *node, const void *config, size_t config_size);

	//Get a copy of the resource's internal structure.
	bool (*const interface)(struct inode *node, void *iface_struct, size_t size);

	//Close the resource;
	void (*const close)(struct inode *node);

	//The deletion function;
	void (*const deleter)(struct inode *);
	
};


//A file descriptor is an inode pointer;
typedef size_t file_descriptor;


//-------------------------------------------------- Inodes operations -------------------------------------------------

bool inode_configure(file_descriptor fd, void *data, size_t size);

void inode_execute(file_descriptor fd, size_t function_id, const void *args, size_t args_size);

bool inode_interface(file_descriptor fd, void *data, size_t size);


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
