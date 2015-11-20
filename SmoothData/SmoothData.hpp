//*********************************************************
//	SmoothData.hpp - moving average smoothing procedure
//*********************************************************

#ifndef SMOOTHDATA_HPP
#define SMOOTHDATA_HPP

#include "Execution_Service.hpp"
#include "Diurnal_Data.hpp"
#include "Time_Step.hpp"
#include "Db_Header.hpp"
#include "Utility.hpp"

#include "Data/File_Group.hpp"

//---------------------------------------------------------
//	SmoothData - execution class definition
//---------------------------------------------------------

class SmoothData : public Execution_Service
{
public:
	SmoothData (void);
	virtual ~SmoothData (void);

	virtual void Execute (void);

protected:
	static char * INPUT_DATA_FILE_x;
	static char * INPUT_DATA_FORMAT_x;
	static char * OUTPUT_DATA_FILE_x;
	static char * OUTPUT_DATA_FORMAT_x;
	static char * DISTRIBUTION_FILE_x;
	static char * DISTRIBUTION_FORMAT_x;
	static char * INPUT_TIME_FORMAT_x;
	static char * OUTPUT_TIME_FORMAT_x;
	static char * SMOOTH_FIELD_NUMBER_x;
	static char * SMOOTH_TIME_INCREMENT_x;
	static char * SMOOTH_GROUP_SIZE_x;
	static char * PERCENT_MOVED_FORWARD_x;
	static char * PERCENT_MOVED_BACKWARD_x;
	static char * NUMBER_OF_ITERATIONS_x;
	static char * CIRCULAR_GROUP_FLAG_x;
	static char * REPLICATE_FIELDS_FLAG_x;
	static char * INPUT_DATA_FORMAT;
	static char * OUTPUT_DATA_FORMAT;
	static char * DISTRIBUTION_FILE;
	static char * DISTRIBUTION_FORMAT;
	static char * INPUT_TIME_FORMAT;
	static char * OUTPUT_TIME_FORMAT;
	static char * SMOOTH_FIELD_NUMBER;
	static char * SMOOTH_TIME_INCREMENT;
	static char * SMOOTH_GROUP_SIZE;
	static char * PERCENT_MOVED_FORWARD;
	static char * PERCENT_MOVED_BACKWARD;
	static char * NUMBER_OF_ITERATIONS;
	static char * CIRCULAR_GROUP_FLAG;
	static char * REPLICATE_FIELDS_FLAG;

	virtual void Program_Control (void);

private:

	File_Group_Array file_group;
	File_Group *group_ptr;

	Diurnal_Array diurnal_data;
	
	int num_group, num_ini, num_in, num_out;

	void Read_Input (void);
	void Write_Output (void);
};
#endif
