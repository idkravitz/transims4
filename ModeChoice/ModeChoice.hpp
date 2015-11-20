//*********************************************************
//	ModeChoice.hpp - apply a mode choice model
//*********************************************************

#ifndef MODECHOICE_HPP
#define MODECHOICE_HPP

#include "Demand_Service.hpp"
#include "Integer_Array.hpp"
#include "Db_Array.hpp"
#include "Utility.hpp"
#include "Ext_File.hpp"
#include "Time_Step.hpp"

#include "Data/Model_Group.hpp"
#include "Data/Skim_Group.hpp"
#include "Data/Mode_Group.hpp"

#define MAX_PURPOSE		100

//---------------------------------------------------------
//	ModeChoice - execution class definition
//---------------------------------------------------------

class ModeChoice : public Demand_Service
{
public:
	ModeChoice (void);
	virtual ~ModeChoice (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

	virtual bool Zone_Processing (Db_File *fh);
	virtual bool Location_Processing (Db_File *fh);
	virtual bool Household_Processing (Db_File *fh);
	virtual bool Population_Processing (Db_File *fh);

protected:

	static char * HOUSEHOLD_LIST;
	static char * MODEL_PROCESSING_METHOD;
	static char * TOUR_PURPOSE_HIERARCHY;
	static char * TOUR_MODE_MODEL_x;
	static char * TOUR_PURPOSE_RANGE_x;
	static char * TRIP_MODE_MODEL_x;
	static char * TRIP_PURPOSE_RANGE_x;
	static char * MODE_DATA_MODE_x;
	static char * MODE_DATA_LABEL_x;
	static char * MODE_DATA_FIELD_x;
	static char * ZONE_SKIM_FILE_x;
	static char * ZONE_SKIM_FORMAT_x;
	static char * TIME_PERIOD_EQUIVALENCE_x;
	static char * SKIM_MEMORY_FACTOR_x;
	static char * DISTANCE_CALCULATION;
	static char * RANDOM_NUMBER_SEED;
	static char * MODE_SHARE_DETAILS;
	static char * MODE_SHARE_FORMAT;
	static char * TIME_OF_DAY_FORMAT;

	virtual void Program_Control (void);

private:
	enum ModeChoice_Reports { TRIP_SCRIPT = 1, TRIP_STACK, TOUR_SCRIPT, TOUR_STACK, TIME_EQUIV,
		SHARE_DETAILS };

	Distance_Type distance_method;

	int max_purpose, report_code, method;

	int trip_number, trip_mode, trip_purpose, trip_start, trip_end, trip_duration;
	int trip_distance, trip_ttime, tot_intra;

	int tour_first, tour_arrive, tour_return, tour_last;
	int tour_mode, tour_purpose, tour_start, tour_end, tour_duration;
	int tour_stops1, tour_stops2, tour_subtours;
	int tour_distance1, tour_distance2, tour_ttime1, tour_ttime2;

	int tot_act_in, tot_act_out, tot_act_mode, tot_trip_in, tot_trip_out, tot_trip_mode;

	bool activity_flag, trip_flag, new_act_flag, new_trip_flag;
	bool hhlist_flag, person_flag, skim_flag, zone_flag, mode_flag, share_flag, detail_flag;

	int purpose_order [MAX_PURPOSE];

	Db_Header share_file;

	Time_Step time_step;
	
	Db_Base tour_info, trip_info;

	Db_Index_Array household_db;
	Db_Index_Array person_db;

	Db_Data_Array location_db;
	Db_Data_Array zone_db;

	Db_Base org_zone, des_zone, org_loc, des_loc;

	Model_Group_Array tour_group, trip_group;
	Skim_Group_Array skim_group;
	Mode_Group_Array mode_group;

	Ext_File hhlist_file;
	Integer_Sort hhold_list;

	typedef struct {
		int hhold;
		int person;
		int trip;
		int origin;
		int destination;
		int start;
		int end;
		int mode;
		int purpose;
		int org_zone;
		int des_zone;
	} Travel_Leg;

	void Read_Scripts (void);
	void Household_List (void);
	bool Read_Trip (int file_num);
	bool Read_Activity (int file_num);
	int  Trip_Process (Travel_Leg &leg);

	void Write_Share_File (Travel_Leg &leg);

	void Share_Details_Report (Travel_Leg &leg);
	void Share_Details_Header (void);
};
#endif
