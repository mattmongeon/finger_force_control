#include "data_file_reader.h"
#include <fstream>
#include <iostream>
#include <stdexcept>


////////////////////////////////////////////////////////////////////////////////
//  Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

cDataFileReader::cDataFileReader(const std::string& filePath)
{
	// --- Read Data File --- //

	std::ifstream inFile(filePath.c_str());
	if( !inFile.is_open() )
	{
		std::cout << "Could not open file \'" << filePath << "\'!" << std::endl;
		throw std::invalid_argument("Could not open file!");
	}

	inFile.seekg(0, std::ios::end);
	std::streampos fileSize = inFile.tellg();
	inFile.seekg(0, std::ios::beg);
	
	mData = std::vector<biotac_tune_data>(fileSize/sizeof(biotac_tune_data));
	inFile.read( reinterpret_cast<char*>(&(mData[0])), fileSize);
	
	inFile.close();
}

////////////////////////////////////////////////////////////////////////////////

cDataFileReader::~cDataFileReader()
{
	
}

////////////////////////////////////////////////////////////////////////////////
//  Interface Functions
////////////////////////////////////////////////////////////////////////////////

std::size_t cDataFileReader::GetNumDataPoints() const
{
	return mData.size();
}

////////////////////////////////////////////////////////////////////////////////

std::vector<biotac_tune_data> cDataFileReader::GetData() const
{
	return mData;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint32_t> cDataFileReader::GetAllTimestamps() const
{
	std::vector<uint32_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mTimestamp);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllPACPoints() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.pac);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllPDCPoints() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.pdc);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllTACPoints() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.tac);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllTDCPoints() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.tdc);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE1Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e1);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE2Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e2);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE3Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e3);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE4Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e4);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE5Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e5);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE6Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e6);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE7Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e7);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE8Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e8);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE9Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e9);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE10Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e10);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE11Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e11);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE12Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e12);
	}

	return retVal;

}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE13Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e13);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE14Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e14);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE15Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e15);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE16Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e16);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE17Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e17);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE18Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e18);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<uint16_t> cDataFileReader::GetAllE19Points() const
{
	std::vector<uint16_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mData.e19);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<int32_t> cDataFileReader::GetAllLoadCellPoints() const
{
	std::vector<int32_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mLoadCell_g);
	}

	return retVal;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<int32_t> cDataFileReader::GetAllReferenceForcePoints() const
{
	std::vector<int32_t> retVal(mData.size());
	for( std::size_t i = 0; i < mData.size(); ++i )
	{
		retVal.push_back(mData[i].mReference_g);
	}

	return retVal;
}

