//*********************************************************
//	Turn_Output.hpp - Output Interface Class
//*********************************************************

#ifndef TURN_OUTPUT_HPP
#define TURN_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Movement_File.hpp"
#include "Movement_Data.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Turn_Output Class definition
//---------------------------------------------------------

class Turn_Output : public Static_Service
{
public:
	Turn_Output (void);
	virtual ~Turn_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second = 1);

	bool In_Range (int step, int dir);

	void Output_Check (int step);

	void Summarize (int step);

	void Close (void);

	void In_Link (int value)			{ in_link = value; }
	void Out_Link (int value)			{ out_link = value; }

private:
	static char * OUTPUT_TURN_FILE_x;
	static char * OUTPUT_TURN_FORMAT_x;
	static char * OUTPUT_TURN_FILTER_x;
	static char * OUTPUT_TURN_TIME_FORMAT_x;
	static char * OUTPUT_TURN_INCREMENT_x;
	static char * OUTPUT_TURN_TIME_RANGE_x;
	static char * OUTPUT_TURN_NODE_RANGE_x;

	typedef struct {
		Movement_File *file;
		int filter;
		int step;
		Time_Range *time_range;
		Data_Range *node_range;
		Movement_Array *movement;
	} Output_Data;

	Data_Array output_data;

	bool output_flag;
	int node, in_link, out_link;

	void Write_Turn (int step, Output_Data *output);
};

#endif
