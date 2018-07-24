//
// Created by root on 7/18/18.
//

#include "file.h"

#include <string.h>

#include <kernel/kernel.h>
#include <data_structures/string/string.h>


#define FILENAME_SEPARATOR '/'


//--------------------------------------------------- Private headers --------------------------------------------------

//Search for a file matching the path in root's sub files;
file_descriptor_t file_search(const file_t *root, const char *path);


//----------------------------------------------------- Init - Exit ----------------------------------------------------

/*
 * file_init : initialises the file base that composes a file, providing all required data;
 */

void file_init(file_t *file, file_type_t type, file_destructor_t file_destructor) {

	//If a null pointer was provided :
	if (!file) {

		//Error, can't build the file;
		kernel_error("file.c : file_init : file pointer is null");

	}

	//Create the initializer;
	file_t init = {

		//Orphan for instance;
		.parent = 0,

		//File is closed at its creation;
		.status = FILE_CLOSED,

		//File of the required type;
		.type = type,

		//Save the provided destructor;
		.destructor = file_destructor,

	};

	//Initialise the file;
	memcpy(file, &init, sizeof(file_t));

}


/*
 * file_delete : Deletes the file data and the file structure. Error is raised if status is not closed;
 */

void file_delete(file_t *file) {

	//If a null pointer was provided :
	if (!file) {

		//Error, can't build the file;
		kernel_error("file.c : file_delete : file pointer is null");

		//Never reached;
		return;

	}

	//If the file is not closed :
	if (file->status == FILE_OPEN) {

		//Error, can't delete a non closed file;
		kernel_error("file.c : file_delete : file is opened;");

	}

	//The file can be deleted.

	//Execute the file's data destructor;
	(*(file->destructor))(file);

	//Delete the file structure;
	kernel_free(file);

}


//----------------------------------------------------- File search ----------------------------------------------------

/*
 * file_search : searches in root's sub files, for a file matching the path;
 */

file_t *file_search(const file_t *const root, const char *const path) {

	//If an empty path was provided :
	if (!(*path)) {

		//Error, return a null file descriptor;
		return 0;

	}

	//If the root pointer is null :
	if (!root) {

		//Error, root not existing;
		return 0;

	}

	//Cache the remaining path;
	const char *current_path = path;

	//Cache the current file;
	const file_t *file = root;

	//Cache the first word's address and size;
	size_t word_size = 0;
	const char *word = 0;

	while (true) {

		//Determine the first word and its size;
		strgw(current_path, FILENAME_SEPARATOR, &word, &word_size);


		//If the word is empty :
		if (!word_size) {

			//Return the last found file;
			return (file_descriptor_t) file;

		}

		//If the current file is not a directory :
		if (file->type != DIRECTORY_FILE) {

			//Error, tries to traverse a non directory file;
			return 0;

		}

		//Update the current path to the end of the word;
		current_path = word + word_size;

		//Call the directory search function, that will search among dir's subs;
		file = directory_search_child((directory_file_t *) file, word, word_size);

		//If no file was found :
		if (!file) {

			//Error, no file matched the required name;
			return 0;

		}

	}

}


//---------------------------------------------------- Open - close ----------------------------------------------------


/*
 * Attempt to open a file, given a root file, and a path through it;
 */

file_descriptor_t file_open(const file_t *const root, const char *path) {

	//Search for the required file;
	file_t *file = file_search(root, path);

	//If no file was found :
	if (!file) {

		//Fail, no file was found;
		return 0;

	}

	//If the file is not closed :
	if (file->status != FILE_CLOSED) {

		//Error, file could not be opened;
		return 0;

	}

	//Mark the file opened;
	file->status = FILE_OPEN;

	//Return the file descriptor;
	return file;


}

/*
 * file_close : attempts to close a file referenced from the file descriptor;
 */

bool file_close(file_t *const file) {

	if (!file) {

		//Error, provided a null file descriptor;
		return false;

	}

	//Cache the file status;
	file_status_t status = file->status;

	//If the file is already closed :
	if (status == FILE_CLOSED) {

		//Fail
		return false;

	}

	//Close the file;
	file->status = FILE_CLOSED;

	//If the file is removed from the file system :
	if (status == FILE_ORPHAN) {

		//Remove the file from root;
		file_delete(file);
	}

}


//---------------------------------------------------- Add - remove ----------------------------------------------------

//Add a file to, the  given a root file, a path through it;
bool file_add(directory_t *const root, file_t *f, const char *path, const bool path_creation) {

	//If the root pointer is null :
	if (!root) {

		//Error, root not existing;
		return 0;

	}

	//If the file pointer is null :
	if (!f) {

		//Error, file not existing;
		return 0;

	}

	//Declare the current file and initialise it to root;
	directory_t *current_dir = root;

	//Cache the first word's address and size;
	size_t prev_word_size = 0;
	const char *prev_word = 0;

	//Determine the first word and its size;
	strgw(path, FILENAME_SEPARATOR, &prev_word, &prev_word_size);

	//If the first word is empty :
	if (!prev_word_size) {

		//Error, cannot insert if no name was provided;
		return false;

	}

	//Update the current path to the end of the word;
	path = prev_word + prev_word_size;

	//Cache the second word's address and size;
	size_t current_word_size = 0;
	const char *current_word = 0;

	while (true) {

		//Determine the next word and its size;
		strgw(path, FILENAME_SEPARATOR, &current_word, &current_word_size);

		//If the word is empty, we reached the end of the path :
		if (!current_word_size) {

			//We can add the file to the current dir; Return the insertion status;
			return directory_add_file(current_dir, f, prev_word, prev_word_size);

		}

		//The current word is not null; Traversal can continue;

		//Call the directory search function, that will search among dir's subs for the previous word;
		file_t *file = directory_search_child(current_dir, prev_word, prev_word_size);

		//If no file was found :
		if (!file) {

			//If the path can be created :
			if (path_creation) {

				//Create a directory;
				directory_t new_dir = directory_create();//TODO FOR FS ABSTRACTION : THE CURRENT FIL SYSTEM MUST HANDLE THE CREATION;

				//Insert the new directory in the current directory;
				directory_add(current_dir, new_dir, prev_word, prev_word_size);

			} else {
				//If not, no file matches the required name;

				//Error, no file matched the required name;
				return 0;

			}

		} else {
			//If the file exists, we know that the required file must be in its children.

			//If it is not a dir :
			if (file->type != DIRECTORY_FILE) {

				//It has no children. We can stop here;
				return 0;

			} else {

				//If it is a dir, we can update data for another iteration.
				prev_word = current_word;
				prev_word_size = current_word_size;

				//Update the current path to the end of the previous word;
				path = prev_word + prev_word_size;

				//Update the current dir to match the new one;
				current_dir = (directory_t *) file;

			}

		}

	}

}

//Attempt to remove a file, given a root file and a path;
bool file_remove(file_t *root, const char *path) {

	//If the root pointer is null :
	if (!root) {

		//Error, root not existing;
		return 0;

	}

	//If the file pointer is null :
	if (!f) {

		//Error, file not existing;
		return 0;

	}

	//Declare the current file and initialise it to root;
	directory_t *current_dir = root;

	//Cache the first word's address and size;
	size_t prev_word_size = 0;
	const char *prev_word = 0;

	//Determine the first word and its size;
	strgw(path, FILENAME_SEPARATOR, &prev_word, &prev_word_size);

	//If the first word is empty :
	if (!prev_word_size) {

		//Error, cannot insert if no name was provided;
		return false;

	}

	//Update the current path to the end of the word;
	path = prev_word + prev_word_size;

	//Cache the second word's address and size;
	size_t current_word_size = 0;
	const char *current_word = 0;

	while (true) {

		//Determine the next word and its size;
		strgw(path, FILENAME_SEPARATOR, &current_word, &current_word_size);

		//If the word is empty, we reached the end of the path :
		if (!current_word_size) {

			//We can add the file to the current dir; Return the insertion status;
			return directory_add_file(current_dir, f, prev_word, prev_word_size);

		}

		//The current word is not null; Traversal can continue;

		//Call the directory search function, that will search among dir's subs for the previous word;
		file_t *file = directory_search_child(current_dir, prev_word, prev_word_size);

		//If no file was found :
		if (!file) {

			//If the path can be created :
			if (path_creation) {

				//Create a directory;
				directory_t new_dir = directory_create();//TODO FOR FS ABSTRACTION : THE CURRENT FIL SYSTEM MUST HANDLE THE CREATION;

				//Insert the new directory in the current directory;
				directory_add(current_dir, new_dir, prev_word, prev_word_size);

			} else {
				//If not, no file matches the required name;

				//Error, no file matched the required name;
				return 0;

			}

		} else {
			//If the file exists, we know that the required file must be in its children.

			//If it is not a dir :
			if (file->type != DIRECTORY_FILE) {

				//It has no children. We can stop here;
				return 0;

			} else {

				//If it is a dir, we can update data for another iteration.
				prev_word = current_word;
				prev_word_size = current_word_size;

				//Update the current path to the end of the previous word;
				path = prev_word + prev_word_size;

				//Update the current dir to match the new one;
				current_dir = (directory_t *) file;

			}

		}

	}

}


void file_remove(file_t *file) {


}