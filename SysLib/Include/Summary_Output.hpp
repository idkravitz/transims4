//*********************************************************
//	Summary_Output.hpp - Output Interface Class
//*********************************************************

#ifndef SUMMARY_OUTPUT_HPP
#define SUMMARY_OUTPUT_HPP

#include "Static_Service.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Delay_File.hpp"
#include "Utility.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	Summary_Output Class definition
//---------------------------------------------------------

class Summary_Output : public Static_Service
{
public:
	Summary_Output (void);
	virtual ~Summary_Output (void);

	void Add_Keys (void);

	void Read_Control (int steps_per_second = 1);

	bool In_Range (int step, int dir, int veh_type = 0);

	bool Veh_Type_Flag (void)			{ return (veh_type_flag); }

	void Output_Check (int step);

	void Summarize (int step);

	void Close (void);

	void From_Dir (int value)			{ from_dir = value; }
	void From_Offset (int value)		{ from_offset = value; }
	void From_Speed (int value)			{ from_speed = value; }
	void From_Connect (int value)		{ from_connect = value; }

	void To_Dir (int value)				{ to_dir = value; }
	void To_Offset (int value)			{ to_offset = value; }
	void To_Speed (int value)			{ to_speed = value; }
	void To_Connect (int value)			{ to_connect = value; }

	void Veh_Type (int value)			{ veh_type = value; }
	void Vehicle_PCE (int value)		{ veh_pce = value; }
	void Occupancy (int value)          { veh_occ = value; }

	void Cycle_Failure (int step, int dir, int vehicles, int persons, int veh_type = 0);

private:
	static char * OUTPUT_SUMMARY_FILE_x;
	static char * OUTPUT_SUMMARY_FORMAT_x;
	static char * OUTPUT_SUMMARY_TIME_FORMAT_x;
	static char * OUTPUT_SUMMARY_INCREMENT_x;
	static char * OUTPUT_SUMMARY_TIME_RANGE_x;
	static char * OUTPUT_SUMMARY_LINK_RANGE_x;
	static char * OUTPUT_SUMMARY_VEH_TYPES_x;
	static char * OUTPUT_SUMMARY_TURN_FLAG_x;
	static char * OUTPUT_SUMMARY_PCE_FLAG_x;
	static char * OUTPUT_SUMMARY_PERSON_FLAG_x;
	static char * OUTPUT_SUMMARY_COORDINATES_x;

	typedef struct {
		int veh_enter;
		int veh_exit;
		int veh_step;
		int distance;
		int density;
		int time;
		int max_density;
		int num_stop;
		int queue;
		int max_queue;
		int num_fail;
	} Summary_Data;

	typedef struct {
		int volume;
		int distance;
		int time;
	} Connect_Sum;

	typedef struct {
		Delay_File *file;
		int step;
		bool turn_flag;
		Time_Range *time_range;
		Data_Range *link_range;
		Data_Range *veh_types;
		Data_Array *summary_data;
		Data_Array *connect_sum;
		int x1, y1, x2, y2;		//---- rounded ----
		bool coord_flag;
		bool pce_flag;
		bool person_flag;
	} Output_Data;

	Data_Array output_data;

	bool output_flag, veh_type_flag;
	int step_time, veh_type, veh_pce, veh_occ;
	int from_dir, to_dir, from_speed, to_speed, from_offset, to_offset, from_connect, to_connect;

	void Write_Summary (int step, Output_Data *output);
};
#pragma pack (pop)
#endif
