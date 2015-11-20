//*********************************************************
//	SmoothData.cpp - moving average smoothing procedure
//*********************************************************

#include "SmoothData.hpp"

char * SmoothData::INPUT_DATA_FILE_x		= "INPUT_DATA_FILE_*";
char * SmoothData::INPUT_DATA_FORMAT_x		= "INPUT_DATA_FORMAT_*";
char * SmoothData::OUTPUT_DATA_FILE_x		= "OUTPUT_DATA_FILE_*";
char * SmoothData::OUTPUT_DATA_FORMAT_x		= "OUTPUT_DATA_FORMAT_*";
char * SmoothData::DISTRIBUTION_FILE_x		= "DISTRIBUTION_FILE_*";
char * SmoothData::DISTRIBUTION_FORMAT_x	= "DISTRIBUTION_FORMAT_*";
char * SmoothData::INPUT_TIME_FORMAT_x		= "INPUT_TIME_FORMAT_*";
char * SmoothData::OUTPUT_TIME_FORMAT_x		= "OUTPUT_TIME_FORMAT_*";
char * SmoothData::SMOOTH_FIELD_NUMBER_x	= "SMOOTH_FIELD_NUMBER_*";
char * SmoothData::SMOOTH_TIME_INCREMENT_x	= "SMOOTH_TIME_INCREMENT_*";
char * SmoothData::SMOOTH_GROUP_SIZE_x		= "SMOOTH_GROUP_SIZE_*";
char * SmoothData::PERCENT_MOVED_FORWARD_x	= "PERCENT_MOVED_FORWARD_*";
char * SmoothData::PERCENT_MOVED_BACKWARD_x	= "PERCENT_MOVED_BACKWARD_*";
char * SmoothData::NUMBER_OF_ITERATIONS_x	= "NUMBER_OF_ITERATIONS_*";
char * SmoothData::CIRCULAR_GROUP_FLAG_x	= "CIRCULAR_GROUP_FLAG_*";
char * SmoothData::REPLICATE_FIELDS_FLAG_x	= "REPLICATE_FIELDS_FLAG_*";
char * SmoothData::INPUT_DATA_FORMAT		= "INPUT_DATA_FORMAT";
char * SmoothData::OUTPUT_DATA_FORMAT		= "OUTPUT_DATA_FORMAT";
char * SmoothData::DISTRIBUTION_FILE		= "DISTRIBUTION_FILE";
char * SmoothData::DISTRIBUTION_FORMAT		= "DISTRIBUTION_FORMAT";
char * SmoothData::INPUT_TIME_FORMAT		= "INPUT_TIME_FORMAT";
char * SmoothData::OUTPUT_TIME_FORMAT		= "OUTPUT_TIME_FORMAT";
char * SmoothData::SMOOTH_FIELD_NUMBER		= "SMOOTH_FIELD_NUMBER";
char * SmoothData::SMOOTH_TIME_INCREMENT	= "SMOOTH_TIME_INCREMENT";
char * SmoothData::SMOOTH_GROUP_SIZE		= "SMOOTH_GROUP_SIZE";
char * SmoothData::PERCENT_MOVED_FORWARD	= "PERCENT_MOVED_FORWARD";
char * SmoothData::PERCENT_MOVED_BACKWARD	= "PERCENT_MOVED_BACKWARD";
char * SmoothData::NUMBER_OF_ITERATIONS		= "NUMBER_OF_ITERATIONS";
char * SmoothData::CIRCULAR_GROUP_FLAG		= "CIRCULAR_GROUP_FLAG";
char * SmoothData::REPLICATE_FIELDS_FLAG	= "REPLICATE_FIELDS_FLAG";

//---------------------------------------------------------
//	SmoothData constructor
//---------------------------------------------------------

SmoothData::SmoothData (void) : Execution_Service ()
{
	Program ("SmoothData");
	Version ("4.0.5");
	Title ("Moving Average Smoothing Procedure");

	char *keys [] = {
		INPUT_DATA_FILE_x,
		INPUT_DATA_FORMAT_x,
		OUTPUT_DATA_FILE_x,
		OUTPUT_DATA_FORMAT_x,
		DISTRIBUTION_FILE_x,
		DISTRIBUTION_FORMAT_x,
		INPUT_TIME_FORMAT_x,
		OUTPUT_TIME_FORMAT_x,
		SMOOTH_FIELD_NUMBER_x,
		SMOOTH_TIME_INCREMENT_x,
		SMOOTH_GROUP_SIZE_x,
		PERCENT_MOVED_FORWARD_x,
		PERCENT_MOVED_BACKWARD_x,
		NUMBER_OF_ITERATIONS_x,
		CIRCULAR_GROUP_FLAG_x,
		REPLICATE_FIELDS_FLAG_x,
		INPUT_DATA_FORMAT,
		OUTPUT_DATA_FORMAT,
		DISTRIBUTION_FILE,
		DISTRIBUTION_FORMAT,
		INPUT_TIME_FORMAT,
		OUTPUT_TIME_FORMAT,
		SMOOTH_FIELD_NUMBER,
		SMOOTH_TIME_INCREMENT,
		SMOOTH_GROUP_SIZE,
		PERCENT_MOVED_FORWARD,
		PERCENT_MOVED_BACKWARD,
		NUMBER_OF_ITERATIONS,
		CIRCULAR_GROUP_FLAG,
		REPLICATE_FIELDS_FLAG,
		NULL
	};

	Key_List (keys);

	num_group = num_ini = num_in = num_out = 0;
	group_ptr = NULL;

	diurnal_data.Max_Records (1500);
}

//---------------------------------------------------------
//	SmoothData destructor
//---------------------------------------------------------

SmoothData::~SmoothData (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SmoothData *exe = new SmoothData ();

	return (exe->Start_Execution (commands, control));
}
