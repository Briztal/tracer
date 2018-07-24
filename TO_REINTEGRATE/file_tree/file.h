//
// Created by root on 7/18/18.
//

#ifndef TRACER_FILE_H
#define TRACER_FILE_H


#include <stdbool.h>

#include <stdint.h>


/*
 * 	A file is a structure that offers access to a shared resource, depending on its type;
 */


/*
 * A file can have the following type :
 */

typedef enum {

	//An IO file, providing read-write functions;
		IO_FILE,

	//A stream file, providing send-receive functions;
		STREAM_FILE,

	//A queue file, providing push-pull functions;
		QUEUE_FILE,

	//A function file, providing call function;
		FUNCTION_FILE,

	//A directory file, providing sub_search function;
		DIRECTORY_FILE,

} file_type_t;


/*
 * Depending on if it is used or not, a file can have one of the following status :
 */

typedef enum {

	//The file is closed, not in use currently.
		FILE_CLOSED,

	//The file is open, used by a process;
		FILE_OPEN,

} file_status_t;


/*
 * The file struct defines the minimum of data that a file must comprise;
 * 	All files are composed of the following structure;
 */

typedef struct file_t {

	//The parent;
	struct file_t *parent;

	//The current status of the file, mutable;
	file_status_t status;

	//The type of the file, constant;
	const file_type_t type;

	//A function to correctly delete the file's private data, constant;
	void (*const destructor)(struct file_t *file);

} file_t;


//A file descriptor will be a pointer to a file;
typedef file_t *file_descriptor_t;

//A file destructor is a function that takes a file in argument and deletes it;
typedef void (*const file_destructor_t)(file_t *);


//----------------------------------------------------- Init - Exit ----------------------------------------------------

//Initialise the file base that composes a file, providing all required data;
void file_init(file_t *file, file_type_t type, file_destructor_t file_destructor);

//Deletes a file. Status must be closed;
void file_delete(file_t *file);

//---------------------------------------------------- Open - close ----------------------------------------------------

//Attempt to open a file, given a root file, and a path through it;
file_descriptor_t file_open(const file_t *root, const char *path);

//Attempt to close a file referenced from the file descriptor;
bool file_close(file_descriptor_t file_descriptor);


//---------------------------------------------------- Add - remove ----------------------------------------------------

//Add a file to, the  given a root file, a path through it, and a flag that allows the path creation if required;;
bool file_add(file_t *root, const char *path, bool path_creation);

//Attempt to remove a file, given a root file and a path;
bool file_remove(file_t *root, const char *path);

#endif //TRACER_FILE_H
