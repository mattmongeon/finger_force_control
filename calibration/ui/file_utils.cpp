#include "file_utils.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <dirent.h>
#include <algorithm>
#include <iostream>


namespace
{
	int getSelection(const std::vector<std::string>& items)
	{
		for( std::size_t i = 0; i < items.size(); ++i )
		{
			std::cout << i+1 << ": " << items[i] << std::endl;
		}
				
		bool goodSelection = false;
		while(!goodSelection)
		{
			std::cout << std::endl;
			std::cout << "Selection: " << std::flush;

			size_t selection = 0;
			std::cin >> selection;
			selection -= 1;
			std::cout << std::endl << std::endl;
					
			if( (selection < items.size()) && (selection >= 0) )
			{
				goodSelection = true;
				return selection;
			}
			else
			{
				std::cout << "The entry \'" << selection+1 << "\' is invalid." << std::endl;
			}
		}

		return -1;
	}
};


////////////////////////////////////////////////////////////////////////////////

void nFileUtils::CreateDirectory(const std::string& path)
{
	if( !boost::filesystem::exists(path) )
	{
		boost::filesystem::create_directory(path);
	}
}

////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> nFileUtils::GetFilesInDirectory(const std::string& path, const std::string& extension)
{
	std::vector<std::string> retVal;
	
	DIR* dir;
	struct dirent* ent;
	if( (dir = opendir(path.c_str())) != NULL )
	{
		while((ent = readdir(dir)) != NULL)
		{
			if( ent->d_type == DT_REG )
			{
				std::string name(ent->d_name);
				if( (extension == "") ||
					(name.compare(name.length() - extension.length(), extension.length(), extension) == 0) )
				{
					retVal.push_back(ent->d_name);
				}
			}
		}

		std::sort(retVal.begin(), retVal.end());
		
		closedir(dir);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::string nFileUtils::GetFileSelectionInDirectory(const std::string& path, const std::string& extension)
{
	// --- Directory Selection --- //

	// Get a list of the subdirectories.
	DIR* dir;
	struct dirent* ent;
	std::vector<std::string> dirs;
	if( (dir = opendir(path.c_str())) != NULL )
	{
		while((ent = readdir(dir)) != NULL)
		{
			if( ent->d_type == DT_DIR )
			{
				std::string name(ent->d_name);
				if( name != ".." )
					dirs.push_back(ent->d_name);
			}
		}

		std::sort(dirs.begin(), dirs.end());
		
		closedir(dir);
	}
	else
	{
		std::cout << "Unable to open directory!" << std::endl;
		return "";
	}

	// Select the directory.
	std::string selectedDir(path);
	if( dirs.size() > 1 )
	{
		std::cout << "Select the directory using its list index." << std::endl;
		std::cout << std::endl;
		std::cout << "Directories:" << std::endl;
		std::cout << "------------" << std::endl;
		std::cout << std::endl;
		
		int selection = getSelection(dirs);

		// Recursively go through the subdirectories if the selection is not the current directory.
		if( selection != 0 )
			return GetFileSelectionInDirectory(path + "/" + dirs[selection], extension);
	}


	// --- File Selection In Directory --- //

	// We have chosen the current directory, so let's choose a file.
	std::cout << "Select the file using its list index." << std::endl;
	std::cout << std::endl;
	std::cout << "Files:" << std::endl;
	std::cout << "------" << std::endl;
	std::cout << std::endl;

	std::vector<std::string> files = nFileUtils::GetFilesInDirectory(selectedDir, extension);

	std::string fileSelection = "";
	if( !files.empty() )
	{
		int selection = getSelection(files);

		if( selection != -1 )
		{
			fileSelection = selectedDir;
			fileSelection += "/" + files[selection];
		}
		else
		{
			std::cout << "There was an error choosing the file!" << std::endl;
		}
	}
	else
	{
		std::cout << std::endl
				  << "There are no files in the directory \'"
				  << selectedDir
				  << "\'."
				  << std::endl
				  << std::endl
				  << std::endl;
	}

	return fileSelection;
}
