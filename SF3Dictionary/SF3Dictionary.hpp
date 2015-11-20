//*********************************************************
//	SF3Dictionary.hpp - SF3 data dictionary table index
//*********************************************************

#ifndef SF3DICTIONARY_HPP
#define SF3DICTIONARY_HPP

#include "Execution_Service.hpp"
#include "Db_File.hpp"
#include "Db_Header.hpp"
#include "Utility.hpp"

class SF3Dictionary : public Execution_Service, public Db_Code
{
public:
	SF3Dictionary (void);
	virtual ~SF3Dictionary (void);

	virtual void Execute (void);

protected:
	static char * SF3_DATA_DICTIONARY;
	static char * SF3_TABLE_INDEX;
	static char * NUM_INDEX_FIELDS;

	virtual void Program_Control (void);

private:
	int num_fields;
	Db_File input_file;
	Db_Header output_file;
};
#endif
