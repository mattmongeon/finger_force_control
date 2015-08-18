#ifndef INCLUDED_DATA_FILE_READER_H
#define INCLUDED_DATA_FILE_READER_H


#include "../common/biotac_comm.h"
#include <vector>
#include <string>


// Parses a binary .dat file and makes the data available to be used.
class cDataFileReader
{
public:

	//--------------------------------------------------------------------------//
	//----------------------  CONSTRUCTION / DESTRUCTION  ----------------------//
	//--------------------------------------------------------------------------//

	explicit cDataFileReader(const std::string& filePath);

	~cDataFileReader();
	

	//--------------------------------------------------------------------------//
	//--------------------------  INTERFACE FUNCTIONS  -------------------------//
	//--------------------------------------------------------------------------//

	// Returns how many data points were contained in the file.
	//
	// Returns - The number of data points.
	std::size_t GetNumDataPoints() const;
	
	// Returns a vector of all of the data from the file.
	//
	// Returns - A vector of all data points.
	std::vector<biotac_tune_data> GetData() const;

	// Returns a vector of all of the timestamps associated with each data entry
	//
	// Returns - Vector of all timestamps.
	std::vector<uint32_t> GetAllTimestamps() const;

	std::vector<uint16_t> GetAllPACPoints() const;
	std::vector<uint16_t> GetAllPDCPoints() const;
	std::vector<uint16_t> GetAllTACPoints() const;
	std::vector<uint16_t> GetAllTDCPoints() const;

	std::vector<uint16_t> GetAllE1Points() const;
	std::vector<uint16_t> GetAllE2Points() const;
	std::vector<uint16_t> GetAllE3Points() const;
	std::vector<uint16_t> GetAllE4Points() const;
	std::vector<uint16_t> GetAllE5Points() const;
	std::vector<uint16_t> GetAllE6Points() const;
	std::vector<uint16_t> GetAllE7Points() const;
	std::vector<uint16_t> GetAllE8Points() const;
	std::vector<uint16_t> GetAllE9Points() const;
	std::vector<uint16_t> GetAllE10Points() const;
	std::vector<uint16_t> GetAllE11Points() const;
	std::vector<uint16_t> GetAllE12Points() const;
	std::vector<uint16_t> GetAllE13Points() const;
	std::vector<uint16_t> GetAllE14Points() const;
	std::vector<uint16_t> GetAllE15Points() const;
	std::vector<uint16_t> GetAllE16Points() const;
	std::vector<uint16_t> GetAllE17Points() const;
	std::vector<uint16_t> GetAllE18Points() const;
	std::vector<uint16_t> GetAllE19Points() const;

	std::vector<int32_t> GetAllLoadCellPoints() const;
	std::vector<int32_t> GetAllReferenceForcePoints() const;
	
private:

	//--------------------------------------------------------------------------//
	//-----------------------------  DATA MEMBERS  -----------------------------//
	//--------------------------------------------------------------------------//

	// The container of all of the data from the file.
	std::vector<biotac_tune_data> mData;
};


#endif  // INCLUDED_DATA_FILE_READER_H

