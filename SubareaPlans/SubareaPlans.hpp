//*********************************************************
//	SubareaPlans.hpp - Create a Subarea Plan file
//*********************************************************

#ifndef SUBAREAPLANS_HPP
#define	SUBAREAPLANS_HPP

#include "Demand_Service.hpp"
#include "Integer_Array.hpp"
#include "Plan_File.hpp"
#include "Arcview_File.hpp"
#include "Access_File.hpp"
#include "Parking_File.hpp"
#include "Location_File.hpp"
#include "Stop_File.hpp"
#include "Line_File.hpp"
#include "Schedule_File.hpp"
#include "Driver_File.hpp"
#include "Zone_Equiv.hpp"
#include "Time_Equiv.hpp"
#include "Ext_File.hpp"
#include "Time_Range.hpp"

class SubareaPlans : public Demand_Service
{
public:
	SubareaPlans (void);
	virtual ~SubareaPlans (void);
	
	virtual void Execute (void);

	virtual bool Location_Processing (Db_File *file);
	virtual bool Parking_Processing (Db_File *file);
	virtual bool Access_Processing (Db_File *file);
	virtual bool Vehicle_Processing (Db_File *file);
	virtual bool Stop_Processing (Db_File *file);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * SUBAREA_BOUNDARY_POLYGON;
	static char * EXTERNAL_OFFSET_LENGTH;
	static char * NEW_PLAN_FILE;
	static char * NEW_PLAN_FORMAT;
	static char * NEW_HOUSEHOLD_LIST;
	static char * RETIME_EARLY_ARRIVALS;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * TIME_PERIOD_EQUIVALENCE;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TIME_PERIODS;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum SubareaPlans_Reports { TRIP_REPORT = 1, ZONE_EQUIV, TIME_EQUIV };

	bool transit_flag, output_flag, range_flag;
	bool delay_flag, location_flag, parking_flag, access_flag, extend_flag, early_flag;
	bool trip_flag, zone_flag, time_flag, turn_flag, type_flag, hhlist_flag, boundary_flag;
	int nlink, nshort, nlocation, nparking, naccess, num_adjust, total_adjust, external_offset;
	int max_location, max_parking, max_access, max_vehicle, max_zone;
	int num_in_in, num_out_in, num_in_out, num_out_out;
	int leg_in_in, leg_out_in, leg_in_out, leg_out_out;
	int ***trips;

	int max_stop, max_route, new_stop, new_route, nstop, nroute, nschedule, ndriver;
	int current_traveler, current_trip, current_leg, boarding_stop, last_location, last_time;

	Plan_File input_plans, subarea_plans;
	Arcview_File subarea_boundary;	
	Ext_File hhlist_file;

	Access_File *access_file;
	Parking_File *parking_file;
	Location_File *location_file;
	Stop_File *stop_file;
	Line_File *line_file;
	Schedule_File *schedule_file;
	Driver_File *driver_file;

	Line_Array new_line_data;
	Driver_Array new_driver_data;

	Time_Step time_step;
	Zone_Equiv zone_equiv;
	Time_Equiv time_equiv;
	Time_Range time_range;

	//---- Boundary List Data ----

	typedef struct {
		int link;
		int type;
		int in_parking;
		int in_location;
		int in_stop;
		int out_parking;
		int out_location;
		int out_stop;
	} Boundary;

	Index_Array boundary;

	//---- methods ----

	void Set_Boundary (void);
	void Write_Boundary (void);
	void Create_Route (void);
	void Read_Plans (void);
	bool Transit_Leg (void);
	bool Write_Plans (int num_sub, Vehicle_Data *veh_ptr, bool *save_flag);
	bool Save_Plans (void);
	void Write_Vehicle (void);
	void Trip_Report (void);
};
#endif
