/*
  vfs.h Part of TRACER

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

/*
 * This file contains the VFS API.
 *
 * 	All functions in this file can be used by the kernel to interact with the file system.
 */

#ifndef TRACER_VFS_H
#define TRACER_VFS_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


typedef void FILE;


/**
 * vfs_mount : mounts a file system.
 * @param path : the absolute path of the directory where to mount the file system;
 * @param fs_name : the name of the file system, used to determine its type (ex : ext2);
 * @param dev_name : the absolute path of the device where the file system is located; Can be null;
 */

void vfs_mount(const char *path, const char *fs_name, const char *dev_name);


/**
 * vfs_unmount : unmounts a file system;
 * @param path : the absolute path of the mount point directory.
 */

void vfs_unmount(const char *path);


/**
 * vfs_create : creates a generic resource by its path;
 * @param path : the absolute path where to create the resource. Must not be slash-ended;
 */

void vfs_create(const char *path);


/**
 * vfs_delete : searched for the given resource and attempts to delete it;
 * @param path : the absolute path of the resource;
 */

void vfs_delete(const char *path);


/**
 * vfs_open : opens a resource and returns its descriptor;
 * @param path : the absolute path of the resource;
 * @return the descriptor of the resource for future manipulations;
 */

FILE *vfs_open(const char *path);


/**
 * vfs_close : closes an opened resource;
 * @param rd : the resource descriptor of the opened resource;
 */

void vfs_close(FILE *rd);



#endif //TRACER_VFS_H
