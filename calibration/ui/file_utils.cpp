#include "file_utils.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <dirent.h>


////////////////////////////////////////////////////////////////////////////////

void nFileUtils::CreateDirectory(const std::string path)
{
	if( !boost::filesystem::exists(path) )
	{
		boost::filesystem::create_directory(path);
	}
}

////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> nFileUtils::GetFilesInDirectory(const std::string path)
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
				retVal.push_back(ent->d_name);
			}
		}
		
		closedir(dir);
	}

	return retVal;
}

