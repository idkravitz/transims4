//*********************************************************
//	Speed_Bin_Output.hpp - Output Interface Class
//*********************************************************

#ifndef SPEED_BIN_OUTPUT_HPP
#define SPEED_BIN_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Speed_Bin_File.hpp"
#include "Speed_Bin_Data.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Speed_Bin_Output Class definition
//---------------------------------------------------------

class Speed_Bin_Output : public Static_Service
{
public:
	Speed_Bin_Output (void);
	virtual ~Speed_Bin_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second = 1, double cell_size = 7.5);

	bool In_Range (int step, int dir, int veh_type);

	void Output_Check (int step);

	void Summarize (int step);

	void Close (void);

	void Dir (int value)				{ dir = value; }
	void Offset (int value)				{ offset = value; }
	void Speed (int value)				{ speed = value; }
	void Veh_Type (int value)			{ veh_type = value; }

	double Cell_Size (void)				{ return (cell_size); }
	void   Cell_Size (double size)		{ if (size >= 1.0) cell_size = size; }

private:
	static char * OUTPUT_SPEED_FILE_x;
	static char * OUTPUT_SPEED_FORMAT_x;
	static char * OUTPUT_SPEED_VEHICLE_TYPE_x;
	static char * OUTPUT_SPEED_FILTER_x;
	static char * OUTPUT_SPEED_TIME_FORMAT_x;
	static char * OUTPUT_SPEED_INCREMENT_x;
	static char * OUTPUT_SPEED_TIME_RANGE_x;
	static char * OUTPUT_SPEED_LINK_RANGE_x;
	static char * OUTPUT_SPEED_SAMPLE_TIME_x;
	static char * OUTPUT_SPEED_BOX_LENGTH_x;
	static char * OUTPUT_SPEED_NUM_BINS_x;

	typedef struct {
		Speed_Bin_File *file;
		int veh_type;
		int subtype;
		int veh_code;
		int filter;
		int step;
		int sample;
		int box_length;
		int num_bins;
		Time_Range *time_range;
		Data_Range *link_range;
		Speed_Bin_Array *speed_bin;
	} Output_Data;

	Data_Array output_data;

	bool output_flag;
	int dir, offset, speed, veh_type;
	double cell_size;

	void Write_Speed (int step, Output_Data *output);
};

#endif
