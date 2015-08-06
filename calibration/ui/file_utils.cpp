#include "file_utils.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <dirent.h>
#include <algorithm>
#include <iostream>


namespace
{
	int getFileSelection(const std::vector<std::string>& files)
	{
		for( std::size_t i = 0; i < files.size(); ++i )
		{
			std::cout << i+1 << ": " << files[i] << std::endl;
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
					
			if( (selection < files.size()) && (selection >= 0) )
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
	std::cout << "Select the file using its list index." << std::endl;
	std::cout << std::endl;
	std::cout << "Files:" << std::endl;
	std::cout << "------" << std::endl;
	std::cout << std::endl;

	std::vector<std::string> files = nFileUtils::GetFilesInDirectory(path, extension);

	std::string fileSelection = "";
	if( !files.empty() )
	{
		int selection = getFileSelection(files);

		if( selection != -1 )
		{
			fileSelection = path;
			fileSelection += "/" + files[selection];
		}
		else
		{
			std::cout << "There was an error choosing the file!" << std::endl;
		}
	}
	else
	{
		std::cout << std::endl << "There are no files in the \'data\' directory." << std::endl << std::endl;
		std::cout << std::endl;
	}

	return fileSelection;
}
