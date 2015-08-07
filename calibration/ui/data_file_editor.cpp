#include "data_file_editor.h"
#include "data_file_reader.h"
#include "../common/biotac_comm.h"
#include <fstream>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cDataFileEditor::cDataFileEditor(const std::string& inFile, const std::string& outFile)
{
	cDataFileReader reader(inFile);

	std::vector<biotac_tune_data> fileData(reader.GetData());
	std::vector<biotac_tune_data> outputData;
	for( std::size_t i = 0; i < fileData.size(); ++i )
	{
		if( i >= 40 )
		{
			outputData.push_back(fileData[i]);
		}
	}

	std::ofstream out(outFile.c_str(), std::ios::out | std::ios::binary);
	for( std::size_t i = 0; i < outputData.size(); ++i )
	{
		out.write(reinterpret_cast<const char*>(&(outputData[i])), sizeof(biotac_tune_data));
	}

	out.close();
}

////////////////////////////////////////////////////////////////////////////////

cDataFileEditor::~cDataFileEditor()
{

}

