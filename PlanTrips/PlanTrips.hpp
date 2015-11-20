//*********************************************************
//	PlanTrips.hpp - create trips from plans
//*********************************************************

#ifndef PLANTRIPS_HPP
#define PLANTRIPS_HPP

#include "Demand_Service.hpp"
#include "Index_Array.hpp"
#include "Activity_File.hpp"
#include "Trip_File.hpp"
#include "Plan_File.hpp"
#include "Time_Step.hpp"
#include "Ext_File.hpp"
#include "Integer_Array.hpp"
#include "Vehicle_File.hpp"

#define MAX_TRAVEL	100

//---------------------------------------------------------
//	PlanTrips - execution class definition
//---------------------------------------------------------

class PlanTrips : public Demand_Service
{
public:

	PlanTrips (void);
	
	virtual void Execute (void);
	virtual bool Access_Processing (Db_File *fh);

protected:
	
	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * HOUSEHOLD_LIST;
	static char * TIME_OF_DAY_FORMAT;
	static char * NEW_PLAN_FILE;
	static char * NEW_PLAN_FORMAT;
	static char * NEW_HOUSEHOLD_LIST;
	static char * CREATE_INDEPENDENT_TRIPS;

	virtual void Program_Control (void);

private:
	enum PlanTrips_Reports { TIME_SHIFT = 1 };

	int num_trips, num_act, num_adjust, hhold_id, veh_id;
	bool trip_flag, new_trip_flag, act_flag, new_act_flag, plan_flag, hhold_flag, hhlist_flag;
	bool create_flag;
	double tot_adjust;

	Trip_File *trip_file;
	Activity_File *act_file;
	Vehicle_File *veh_file;

	typedef struct {
		int id;
		int from_loc;
		int to_loc;
		int from_time;
		int to_time;
	} Location_Access;

	typedef struct {
		int rec_id;
		int start;
		int end;
		int duration;
		int index1;
		int index2;
		int type;
		int constraint;
		int shift;
	} Travel_Activity;
		
	Travel_Activity travel [MAX_TRAVEL];

	Index_Array time_constraint;
	Index_Array parking_access, stop_access;

	Plan_File plan_file;
	Plan_File new_plan_file;
	Data_Array plan_list;

	Time_Step trip_time;
	Ext_File hhold_file, hhlist_file;

	Integer_Sort hhlist_data;

	int start_offset [101];
	int end_offset [101];
	int mid_offset [101];

	//---- methods ----

	void Read_Households (void);
	void Read_Plans (void);
	void Process_Plans (void);
	void New_Trips (void);
	void New_Activity (void);
	void Adjust_Trips (int traveler);
	void Adjust_Activity (int traveler);
	void Add_Trips (void);
	void Add_Activity (void);
	void Write_Trips (void);
	void Write_Activity (void);
};
#endif

