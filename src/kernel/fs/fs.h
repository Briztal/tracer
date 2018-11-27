//
// Created by root on 11/27/18.
//

#ifndef TRACER_FS_H
#define TRACER_FS_H


#include "../../dmz/list.h"

struct inode;
struct dentry;
struct superblock;
struct fs_type;



struct inode {
	
	//The mode of the inode;
	uint32_t i_mode;

	//The dentry related to the inode;
	struct dentry *i_dentry;
	
	//The superblock the inode relates to;
	struct superblock *i_sb;

};


/*
 * A dentry (for directory cache entry) contains the filesystem's structure elements.
 *
 * 	It associates a name with an inode, and contains links to :
 * 	- its unique parent dentry, via d_parent, direct reference;
 * 	- the first (historically) of its children, via d_child, direct reference;
 * 	- its siblings (dentries that share the same parent), via d_siblings, a doubly linked list head;
 */

struct dentry {
	
	//The name of the dentry;
	const char *d_name;
	
	//The inode associated with the dentry;
	struct inode *d_inode;
	
	//The parent of the dentry;
	struct dentry *d_parent;
	
	//The first child of the dentry;
	struct dentry *d_child;
	
	//The list of the dentry's siblings;
	struct list_head *d_siblings;
	
	//The superblock the dentry related to;
	struct superblock *d_sb;
	
};


/**
 * A fs mount structure describes a mounted file system;
 */

struct fs_mnt {
	
	//A list head, for fs_mnt storage;
	struct list_head mnt_head;
	
	//The mount point name;
	const char *mnt_name;
	
	//The mounted fs's superblock;
	struct superblock *mnt_sb;
	
};



struct superblock {
	
	//The first dentry of the superblock;
	struct dentry *sb_root;
	
	//The file system type the superblock relates to;
	struct fs_type *sb_type;
	
	//The mount struct related to the superblock;
	struct fs_mnt *sb_mnt;
	
	
};



/**
 * A file system type structure contains two function pointersone responsible for providing a superblock,
 * 	and another responsible for releasing the superblock;
 *
 * 	Those two functions are called by the vfs only;
 */

struct fs_type {
	
	//The name of the file system type;
	const char *name;
	
	//Provide a superblock from an existing file system; 0 is returned in case of failure;
	struct superblock *(*get_sb);//TODO ARGS
	
	//Properly cleanup a superblock; 0 is returned in case of failure;
	bool (*release_sb)(struct superblock *);//TODO ARGS
	
};





#endif //TRACER_FS_H
