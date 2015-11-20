//*********************************************************
//	FileFormat.hpp - convert file format
//*********************************************************

#ifndef FILEFORMAT_HPP
#define FILEFORMAT_HPP

#include "Execution_Service.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	FileFormat - execution class definition
//---------------------------------------------------------

class FileFormat : public Execution_Service
{
public:
	FileFormat (void);
	virtual ~FileFormat (void);

	virtual void Execute (void);

protected:
	static char * INPUT_DIRECTORY;
	static char * INPUT_FILE;
	static char * INPUT_FORMAT;
	static char * INPUT_HEADER_LINES;
	static char * NESTED_COUNT_FIELD;
	static char * INPUT_FILE_x;
	static char * INPUT_FORMAT_x;
	static char * INPUT_HEADER_LINES_x;
	static char * NESTED_COUNT_FIELD_x;
	static char * OUTPUT_DIRECTORY;
	static char * OUTPUT_FILE;
	static char * OUTPUT_FORMAT;
	static char * OUTPUT_FILE_x;
	static char * OUTPUT_FORMAT_x;

	virtual void Program_Control (void);

private:
	int nfiles, *nest_field;
	Db_Header **input_file;
	Db_Header **output_file;
};
#endif
