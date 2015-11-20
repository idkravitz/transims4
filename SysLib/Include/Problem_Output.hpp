//*********************************************************
//	Problem_Output.hpp - Output Interface Class
//*********************************************************

#ifndef PROBLEM_OUTPUT_HPP
#define PROBLEM_OUTPUT_HPP

#include "Problem_Service.hpp"
#include "Problem_Link_File.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Problem_Output Class definition
//---------------------------------------------------------

class Problem_Output : public Problem_Service
{
public:
	Problem_Output (void);
	virtual ~Problem_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second = 1);

	bool In_Range (int step, int dir);

	void Output_Check (int step);

	void Summarize (int step);

	void Close (void);

	void Problem (int value)			{ problem = value; }
	void Link_Dir (int value)			{ link_dir = value; }

private:
	static char * OUTPUT_PROBLEM_TYPE_x;
	static char * OUTPUT_PROBLEM_FILE_x;
	static char * OUTPUT_PROBLEM_FORMAT_x;
	static char * OUTPUT_PROBLEM_FILTER_x;
	static char * OUTPUT_PROBLEM_TIME_FORMAT_x;
	static char * OUTPUT_PROBLEM_INCREMENT_x;
	static char * OUTPUT_PROBLEM_TIME_RANGE_x;
	static char * OUTPUT_PROBLEM_LINK_RANGE_x;

	typedef struct {
		int ntype;
		int type [MAX_PROBLEM];
		Problem_Link_File *file;
		int filter;
		int step;
		Time_Range *time_range;
		Data_Range *link_range;
		Data_Array *problem_data;
	} Output_Data;

	Data_Array output_data;

	bool output_flag;
	int problem, link_dir;

	void Write_Problem (int step, Output_Data *output);
};

#endif
