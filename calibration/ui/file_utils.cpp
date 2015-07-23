#include "file_utils.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <dirent.h>
#include <algorithm>


////////////////////////////////////////////////////////////////////////////////

void nFileUtils::CreateDirectory(const std::string path)
{
	if( !boost::filesystem::exists(path) )
	{
		boost::filesystem::create_directory(path);
	}
}

////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> nFileUtils::GetFilesInDirectory(const std::string path, const std::string extension)
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

