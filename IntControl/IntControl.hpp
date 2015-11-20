//*********************************************************
//	IntControl.hpp - Intersection Control Utility
//*********************************************************

#ifndef INTCONTROL_HPP
#define INTCONTROL_HPP

#include "Network_Service.hpp"
#include "Signal_File.hpp"
#include "Sign_File.hpp"
#include "Node_List.hpp"
#include "Dir_Connect.hpp"
#include "Time_Range.hpp"
#include "Movement_File.hpp"
#include "Connect_Time.hpp"
#include "Delay_File.hpp"
#include "Integer_Array.hpp"
#include "Db_Array.hpp"

#undef  node_data
#define node_data		(*((Node_List_Array *) node_array))

#undef  dir_data
#define dir_data		(*((Dir_Connect_Array *) dir_array))

#define connect_time	(*((Connect_Time_Array *) connect_array))

//---------------------------------------------------------
//	IntControl - execution class definition
//---------------------------------------------------------

class IntControl : public Network_Service
{
public:

	IntControl (void);
	virtual ~IntControl (void);

	virtual void Execute (void);

	virtual bool Sign_Processing (Db_File *file);
	virtual bool Signal_Processing (Db_File *file);
	virtual bool Timing_Processing (Db_File *file);
	virtual bool Phasing_Processing (Db_File *file);
	virtual bool Detector_Processing (Db_File *file);
	virtual bool Coordinator_Processing (Db_File *file);

protected:

	static char * INPUT_SIGN_FILE;
	static char * INPUT_SIGNAL_FILE;
	static char * DELETE_NODE_CONTROL_FILE;
	static char * ADD_NO_CONTROL_RECORDS;
	static char * PRINT_SIGN_WARNINGS;
	static char * PRINT_MERGE_WARNINGS;
	static char * SIGNAL_CYCLE_LENGTH;
	static char * MINIMUM_PHASE_TIME;
	static char * YELLOW_PHASE_TIME;
	static char * RED_CLEAR_PHASE_TIME;
	static char * SIGNAL_DETECTOR_LENGTH;
	static char * POCKET_LANE_FACTOR;
	static char * GENERAL_GREEN_FACTOR;
	static char * EXTENDED_GREEN_FACTOR;
	static char * SIGNAL_SPLIT_METHOD;
	static char * MINIMUM_LANE_CAPACITY;
	static char * MAXIMUM_LANE_CAPACITY;
	static char * TIME_OF_DAY_FORMAT;
	static char * TIME_PERIOD_BREAKS;
	static char * TIME_PERIOD_RANGE;
	static char * TURN_MOVEMENT_FILE;
	static char * LINK_DELAY_FILE;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum IntControl_Reports { TIMING_UPDATE = 1 };

	enum Method_Type { LANE_SPLITS, CAPACITY_SPLITS, VOLUME_SPLITS, UPDATE_SPLITS };

	int method, cycle_len, yellow, red_clear, min_phase, min_cap, max_cap, detector_length;
	double general_factor, pocket_factor, extend_factor;

	bool merge_flag, delay_flag, range_flag, signal_flag, sign_flag, delete_flag, update_flag;
	bool no_control_flag, warning_flag, input_sign_flag, input_signal_flag, time_flag, turn_flag;
	bool detector_flag, coord_flag, update_report, regen_flag;
	int nsign, nsignal, ntiming, nphasing, ndetector, ncoord;
	int max_timing, max_detector, max_coord;
	int sign_index, signal_index, phasing_index;

	Integer_List cycle_len_list, yellow_list, red_clear_list, min_phase_list, min_cap_list, max_cap_list;
	Integer_List general_factor_list, pocket_factor_list, extend_factor_list;

	Sign_File sign_file;
	Signal_File signal_file;
	Db_File delete_file;

	Sign_Array input_sign;
	Signal_Array input_signal;
	Integer_Sort delete_node;

	Time_Range time_range;
	Movement_File turn_file;
	Delay_File delay_file;

	Db_Data_Array sign_db, signal_db, timing_db, phasing_db, detector_db, coord_db;

	void Read_Deletes (void);
	void Read_Turn (void);
	void Read_Delay (void);
	void Data_Setup (void);
	void Pocket_Lanes (void);
	void Connection_List (void);
	void Read_Signs (void);
	void Write_Signs (void);
	void Read_Signals (void);
	void Copy_Signals (void);
	void Write_Signals (void);
	void Update_Signals (void);
	void Update_Header (void);
};
#endif

