//*********************************************************
//	RunSetup.hpp - setup process control files
//*********************************************************

#ifndef RUNSETUP_HPP
#define RUNSETUP_HPP

#include "Execution_Service.hpp"
#include "Db_File.hpp"
#include "Data_Array.hpp"
#include "Utility.hpp"

class RunSetup : public Execution_Service
{
public:

	RunSetup (void);
	virtual ~RunSetup (void);
	
	virtual void Execute (void);

protected:

	static char * PROGRAM_DIRECTORY;
	static char * CONTROL_DIRECTORY;
	static char * NETWORK_DIRECTORY;
	static char * BATCH_DIRECTORY;
	static char * BATCH_NAME;
	static char * PROGRAM_FLAGS;
	static char * EXIT_CHECK;
	static char * FILE_FORMAT;
	static char * MODEL_NAME;
	static char * ALTERNATIVE_NAME;
	static char * ANALYSIS_YEAR;
	static char * MASTER_CONTROL_FILE;
	static char * DIRECTORY;
	static char * DESCRIPTION;
	static char * PROGRAMS;
	static char * CONTROL_NAME;
	static char * RUN_NAME;
	static char * PREVIOUS_RUN_NAME;
	static char * PARAMETER;
	static char * PARAMETER1;
	static char * PARAMETER2;
	static char * PARAMETER3;
	static char * APPEND_KEY;
	static char * APPEND1_KEY;
	static char * APPEND2_KEY;
	static char * APPEND3_KEY;
	static char * COMMENT_FLAG;
	static char * COMMENT1_FLAG;
	static char * COMMENT2_FLAG;
	static char * COMMENT3_FLAG;
	static char * MASTER_CONTROL_FILE_x;
	static char * DIRECTORY_x;
	static char * DESCRIPTION_x;
	static char * PROGRAMS_x;
	static char * CONTROL_NAME_x;
	static char * RUN_NAME_x;
	static char * PREVIOUS_RUN_NAME_x;
	static char * PARAMETER_x;
	static char * PARAMETER1_x;
	static char * PARAMETER2_x;
	static char * PARAMETER3_x;
	static char * APPEND_KEY_x;
	static char * APPEND1_KEY_x;
	static char * APPEND2_KEY_x;
	static char * APPEND3_KEY_x;
	static char * COMMENT_FLAG_x;
	static char * COMMENT1_FLAG_x;
	static char * COMMENT2_FLAG_x;
	static char * COMMENT3_FLAG_x;
	static char * NUM_SPLIT_FILES;
	static char * SETUP_CONTROL_FILE_x;
	static char * SETUP_PARAMETER_x;

	virtual void Program_Control (void);

private:

	char *param [4], *append [4], *comment [4], *paramx [4], *appendx [4], *commentx [4];
	char batch_name [FIELD_BUFFER], exit_check [FIELD_BUFFER], num_splits [FIELD_BUFFER];
	char bin_dir [FIELD_BUFFER], control_dir [FIELD_BUFFER], network_dir [FIELD_BUFFER];
	char alt_name [FIELD_BUFFER], model_name [FIELD_BUFFER], year_name [FIELD_BUFFER];
	char append_key [FIELD_BUFFER], file_prefix [FIELD_BUFFER], *record, message [8], exe [8];
	char batch_dir [FIELD_BUFFER], file_format [FIELD_BUFFER], program_flags [FIELD_BUFFER];
	int  first_run, last_run, size;
	bool series_flag, setup_flag;

	Db_File batch_file;

	typedef struct {
		int group_num;
		char setup_file [FIELD_BUFFER];
		char parameter [FIELD_BUFFER];
	} Setup_Group;

	typedef struct {
		int group_num;
		char directory [FIELD_BUFFER];
		char description [FIELD_BUFFER];
		char programs [FIELD_BUFFER];
		char control [FIELD_BUFFER];
		char run_name [FIELD_BUFFER];
		char old_name [FIELD_BUFFER];
		char parameter [4] [FIELD_BUFFER];
		char add_key [4] [FIELD_BUFFER];
		Db_File *master_file;
		bool comment_flag [4];
	} Control_Group;

	Data_Array setup_group;
	Data_Array control_group;

	void Key_Processing (bool batch_flag);
	void Master_Processing (void);
	void Convert_Codes (Control_Group *control);
};
#endif

