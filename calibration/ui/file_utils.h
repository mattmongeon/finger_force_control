#ifndef INCLUDED_FILE_UTILS_H
#define INCLUDED_FILE_UTILS_H


#include <string>
#include <vector>


namespace nFileUtils
{
	// Creates the directory specified by the path.  If the directory already exists,
	// this function will effectively do nothing.
	//
	// Params:
	// path - The path to be created.
	void CreateDirectory(const std::string path);


	// Returns a vector containing the names of all of the files in the parameter directory.
	// Returns an empty vector if the directory does not exist.
	//
	// Params:
	// path - The path from which to get a list of files.
	//
	// Return - A vector of the names of all of the files.
	std::vector<std::string> GetFilesInDirectory(const std::string path);
}


#endif  // INCLUDED_FILE_UTILS_H


