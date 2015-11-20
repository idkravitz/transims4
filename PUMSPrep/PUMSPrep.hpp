//*********************************************************
//	PUMSPrep.hpp - PUMS data extraction
//*********************************************************

#ifndef PUMSPREP_HPP
#define PUMSPREP_HPP

#include "Execution_Service.hpp"
#include "Db_Header.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Utility.hpp"
#include "User_Program.hpp"

class PUMSPrep : public Execution_Service, public Db_Code
{
public:
	PUMSPrep (void);
	virtual ~PUMSPrep (void);

	virtual void Execute (void);

protected:

	static char * PUMS_DATA_FILE;
	static char * PUMS_DATA_FILE_x;
	static char * PUMS_HOUSEHOLD_FILE;
	static char * PUMS_HOUSEHOLD_FORMAT;
	static char * PUMS_POPULATION_FILE;
	static char * PUMS_POPULATION_FORMAT;
	static char * NEW_PUMS_HOUSEHOLD_FILE;
	static char * NEW_PUMS_HOUSEHOLD_FORMAT;
	static char * NEW_PUMS_POPULATION_FILE;
	static char * NEW_PUMS_POPULATION_FORMAT;
	static char * HOUSEHOLD_DATA_FIELD_x;
	static char * POPULATION_DATA_FIELD_x;
	static char * STATE_PUMA_LIST;
	static char * SELECT_HOUSEHOLD_TYPES;
 	static char * CONVERSION_SCRIPT;

	virtual void Program_Control (void);

private:
	enum PUMSPrep_Reports { PRINT_SCRIPT = 1, PRINT_STACK };

	bool input_flag, select_flag, type_flag, script_flag;
	int hhold_field, state_field, puma_field, persons_field, type_field, person_field;
	int num_pums, nrec, pums_hh, pums_pop, hh_in, pop_in, hh_out, pop_out;

	typedef struct {
		int field;
		int type;
		int start;
		int length;
		char name [24];
	} PUMS_FIELD;

	typedef struct {
		char state [4];
		int  puma;
	} STATE_PUMA;

	Data_Array hh_fields, pop_fields;
	Data_Array state_puma;
	Data_Range select_type;

	Db_File **pums_file;
	Db_Header hh_file, pop_file;
	Db_Header new_hh_file, new_pop_file;

	Db_File program_file;	
	User_Program program;

	void Copy_Data (void);
	void Read_PUMS (Db_File *pums_file);
};
#endif
