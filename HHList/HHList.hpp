//*********************************************************
//	HHList.hpp - create a partitioned household list
//*********************************************************

#ifndef HHLIST_HPP
#define HHLIST_HPP

#include "Execution_Service.hpp"
#include "Db_File.hpp"
#include "Db_Header.hpp"
#include "Utility.hpp"
#include "Random.hpp"

class HHList : public Execution_Service
{
public:
	HHList (void);
	virtual ~HHList (void);

	virtual void Execute (void);

protected:

	static char * TRIP_FILE;
	static char * TRIP_FORMAT;
	static char * ACTIVITY_FILE;
	static char * ACTIVITY_FORMAT;
	static char * HOUSEHOLD_FILE;
	static char * HOUSEHOLD_FORMAT;
	static char * HOUSEHOLD_LIST;
	static char * NEW_HOUSEHOLD_LIST;
	static char * NUM_SPLIT_FILES;
	static char * RANDOM_NUMBER_SEED;

	virtual void Program_Control (void);

private:
	int num_files;
	bool list_flag, two_flag;

	Random random;

	Db_Header input_file, input2_file;
	Db_File list_file;
	Db_File **output_file;
};
#endif
