//*********************************************************
//	Progression.hpp - Signal Progression Utility
//*********************************************************

#ifndef PROGRESSION_HPP
#define PROGRESSION_HPP

#include "Demand_Service.hpp"
#include "Link_Equiv.hpp"
#include "Db_Array.hpp"
#include "Time_Range.hpp"
#include "Integer_Array.hpp"
#include "Arcview_File.hpp"
#include "Projection_Service.hpp"

#include "Data/Group_Data.hpp"
#include "Data/Signal_Offset.hpp"
#include "Data/Thru_Connect.hpp"
#include "Data/Group_Weight.hpp"

//---------------------------------------------------------
//	Progression - execution class definition
//---------------------------------------------------------

class Progression : public Demand_Service
{
public:

	Progression (void);
	virtual ~Progression (void);

	virtual void Execute (void);

	virtual bool Connectivity_Processing (Db_File *file);
	virtual bool Signal_Processing (Db_File *file);
	virtual bool Phasing_Processing (Db_File *file);

protected:

	static char * LINK_EQUIVALENCE_FILE;
	static char * CLEAR_EXISTING_OFFSETS;
	static char * EVALUATE_EXISTING_OFFSETS;
	static char * PROGRESSION_TIME_PERIODS;
	static char * PROGRESSION_PERIOD_SPEED;
	static char * OPTIMIZATION_METHOD;
	static char * GROUP_PERIOD_WEIGHT_FILE;
	static char * KEEP_LINK_GROUP_ORDER;
	static char * ARCVIEW_PROGRESSION_FILE;
	static char * LINK_DIRECTION_OFFSET;
	
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Progression_Reports { LINK_EQUIV = 1, WEIGHT_REPORT };
	enum Optimization_Methods { PERCENT_THRU, MAX_SPEED, VEH_HOURS };

	int fixed, updated, progression_time, period, mid_period, method, tot_thru, max_period, old_thru;
	int period_fld, order_fld, group_fld, percent_fld, time_fld, length_fld, speed_fld, vht_fld;
	double progression_speed, link_offset;
	bool equiv_flag, clear_flag, speed_flag, period_flag, arcview_flag, delay_flag, weight_flag, order_flag;
	bool eval_flag;

	Db_Data_Array signal_db;

	Arcview_File arcview_file;
	Point_Data points;

	Projection_Service projection;	

	Db_File weight_file;
	Group_Weight_Array weight_data;

	Group_Array group_data;
	Signal_Offset_Array signal_offset;
	Thru_Connect_Array thru_connect;
	
	Time_Range progress_time;
	Integer_List progress_speed;
	Link_Equiv link_equiv;

	void Read_Weights (void);
	void Signal_Setup (void);
	void User_Groups (void);
	void Signal_Groups (void);
	void Write_Signals (void);
	int  Set_Offset (int group, bool report_flag = false, bool evaluate_flag = false);

	void Weight_Header (void);
};
#endif

