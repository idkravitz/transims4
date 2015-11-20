//*********************************************************
//	RunSetup.cpp - setup process control files
//*********************************************************

#include "RunSetup.hpp"

char * RunSetup::PROGRAM_DIRECTORY		= "PROGRAM_DIRECTORY";
char * RunSetup::CONTROL_DIRECTORY		= "CONTROL_DIRECTORY";
char * RunSetup::NETWORK_DIRECTORY		= "NETWORK_DIRECTORY";
char * RunSetup::BATCH_DIRECTORY		= "BATCH_DIRECTORY";
char * RunSetup::BATCH_NAME				= "BATCH_NAME";
char * RunSetup::PROGRAM_FLAGS			= "PROGRAM_FLAGS";
char * RunSetup::EXIT_CHECK				= "EXIT_CHECK";
char * RunSetup::FILE_FORMAT			= "FILE_FORMAT";
char * RunSetup::MODEL_NAME				= "MODEL_NAME";
char * RunSetup::ALTERNATIVE_NAME		= "ALTERNATIVE_NAME";
char * RunSetup::ANALYSIS_YEAR			= "ANALYSIS_YEAR";
char * RunSetup::MASTER_CONTROL_FILE	= "MASTER_CONTROL_FILE";
char * RunSetup::DIRECTORY				= "DIRECTORY";
char * RunSetup::DESCRIPTION			= "DESCRIPTION";
char * RunSetup::PROGRAMS				= "PROGRAMS";
char * RunSetup::CONTROL_NAME			= "CONTROL_NAME";
char * RunSetup::RUN_NAME				= "RUN_NAME";
char * RunSetup::PREVIOUS_RUN_NAME		= "PREVIOUS_RUN_NAME";
char * RunSetup::PARAMETER				= "PARAMETER";
char * RunSetup::PARAMETER1				= "PARAMETER1";
char * RunSetup::PARAMETER2				= "PARAMETER2";
char * RunSetup::PARAMETER3				= "PARAMETER3";
char * RunSetup::APPEND_KEY				= "APPEND_KEY";
char * RunSetup::APPEND1_KEY			= "APPEND1_KEY";
char * RunSetup::APPEND2_KEY			= "APPEND2_KEY";
char * RunSetup::APPEND3_KEY			= "APPEND3_KEY";
char * RunSetup::COMMENT_FLAG			= "COMMENT_FLAG";
char * RunSetup::COMMENT1_FLAG			= "COMMENT1_FLAG";
char * RunSetup::COMMENT2_FLAG			= "COMMENT2_FLAG";
char * RunSetup::COMMENT3_FLAG			= "COMMENT3_FLAG";
char * RunSetup::MASTER_CONTROL_FILE_x	= "MASTER_CONTROL_FILE_*";
char * RunSetup::DIRECTORY_x			= "DIRECTORY_*";
char * RunSetup::DESCRIPTION_x			= "DESCRIPTION_*";
char * RunSetup::PROGRAMS_x				= "PROGRAMS_*";
char * RunSetup::CONTROL_NAME_x			= "CONTROL_NAME_*";
char * RunSetup::RUN_NAME_x				= "RUN_NAME_*";
char * RunSetup::PREVIOUS_RUN_NAME_x	= "PREVIOUS_RUN_NAME_*";
char * RunSetup::PARAMETER_x			= "PARAMETER_*";
char * RunSetup::PARAMETER1_x			= "PARAMETER1_*";
char * RunSetup::PARAMETER2_x			= "PARAMETER2_*";
char * RunSetup::PARAMETER3_x			= "PARAMETER3_*";
char * RunSetup::APPEND_KEY_x			= "APPEND_KEY_*";
char * RunSetup::APPEND1_KEY_x			= "APPEND1_KEY_*";
char * RunSetup::APPEND2_KEY_x			= "APPEND2_KEY_*";
char * RunSetup::APPEND3_KEY_x			= "APPEND3_KEY_*";
char * RunSetup::COMMENT_FLAG_x			= "COMMENT_FLAG_*";
char * RunSetup::COMMENT1_FLAG_x		= "COMMENT1_FLAG_*";
char * RunSetup::COMMENT2_FLAG_x		= "COMMENT2_FLAG_*";
char * RunSetup::COMMENT3_FLAG_x		= "COMMENT3_FLAG_*";
char * RunSetup::NUM_SPLIT_FILES		= "NUM_SPLIT_FILES";
char * RunSetup::SETUP_CONTROL_FILE_x	= "SETUP_CONTROL_FILE_*";
char * RunSetup::SETUP_PARAMETER_x		= "SETUP_PARAMETER_*";

//---------------------------------------------------------
//	RunSetup constructor
//---------------------------------------------------------

RunSetup::RunSetup (void) : Execution_Service ()
{
	Program ("RunSetup");
	Version ("4.0.5");
	Title ("Setup Process Control Files");

	char *keys [] = {
		PROGRAM_DIRECTORY,
		CONTROL_DIRECTORY,
		NETWORK_DIRECTORY,
		BATCH_DIRECTORY,
		BATCH_NAME,
		PROGRAM_FLAGS,
		EXIT_CHECK,
		FILE_FORMAT,
		MODEL_NAME,
		ALTERNATIVE_NAME,
		ANALYSIS_YEAR,
		MASTER_CONTROL_FILE,
		DIRECTORY,
		DESCRIPTION,
		PROGRAMS,
		CONTROL_NAME,
		RUN_NAME,
		PREVIOUS_RUN_NAME,
		PARAMETER,
		PARAMETER1,
		PARAMETER2,
		PARAMETER3,
		APPEND_KEY,
		APPEND1_KEY,
		APPEND2_KEY,
		APPEND3_KEY,
		COMMENT_FLAG,
		COMMENT1_FLAG,
		COMMENT2_FLAG,
		COMMENT3_FLAG,
		MASTER_CONTROL_FILE_x,
		DIRECTORY_x,
		DESCRIPTION_x,
		PROGRAMS_x,
		CONTROL_NAME_x,
		RUN_NAME_x,
		PREVIOUS_RUN_NAME_x,
		PARAMETER_x,
		PARAMETER1_x,
		PARAMETER2_x,
		PARAMETER3_x,
		APPEND_KEY_x,
		APPEND1_KEY_x,
		APPEND2_KEY_x,
		APPEND3_KEY_x,
		COMMENT_FLAG_x,
		COMMENT1_FLAG_x,
		COMMENT2_FLAG_x,
		COMMENT3_FLAG_x,
		NUM_SPLIT_FILES,
		SETUP_CONTROL_FILE_x,
		SETUP_PARAMETER_x,
		NULL
	};

	Key_List (keys);
	Enable_Parameter (true);

	series_flag = setup_flag = false;
	first_run = last_run = 0;
	
	param [0] = PARAMETER;
	param [1] = PARAMETER1;
	param [2] = PARAMETER2;
	param [3] = PARAMETER3;
	paramx [0] = PARAMETER_x;
	paramx [1] = PARAMETER1_x;
	paramx [2] = PARAMETER2_x;
	paramx [3] = PARAMETER3_x;

	append [0] = APPEND_KEY;
	append [1] = APPEND1_KEY;
	append [2] = APPEND2_KEY;
	append [3] = APPEND3_KEY;
	appendx [0] = APPEND_KEY_x;
	appendx [1] = APPEND1_KEY_x;
	appendx [2] = APPEND2_KEY_x;
	appendx [3] = APPEND3_KEY_x;

	comment [0] = COMMENT_FLAG;
	comment [1] = COMMENT1_FLAG;
	comment [2] = COMMENT2_FLAG;
	comment [3] = COMMENT3_FLAG;
	commentx [0] = COMMENT_FLAG_x;
	commentx [1] = COMMENT1_FLAG_x;
	commentx [2] = COMMENT2_FLAG_x;
	commentx [3] = COMMENT3_FLAG_x;

	control_group.Initialize (sizeof (Control_Group), 5);
	setup_group.Initialize (sizeof (Setup_Group), 5);
}

//---------------------------------------------------------
//	RunSetup destructor
//---------------------------------------------------------

RunSetup::~RunSetup (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	RunSetup *exe = new RunSetup ();

	return (exe->Start_Execution (commands, control));
}

