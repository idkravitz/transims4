//*********************************************************
//	PlanSelect.hpp - Select Households for Re-Routing
//*********************************************************

#ifndef PLANSELECT_HPP
#define PLANSELECT_HPP

#include "Demand_Service.hpp"
#include "Integer_Array.hpp"
#include "Plan_File.hpp"
#include "Random.hpp"
#include "Data_Range.hpp"
#include "Time_Range.hpp"
#include "Arcview_File.hpp"
#include "HHRec_File.hpp"
#include "HHRec_Array.hpp"
#include "Ext_File.hpp"

//---------------------------------------------------------
//	PlanSelect - execution class definition
//---------------------------------------------------------

class PlanSelect : public Demand_Service
{
public:
	PlanSelect (void);
	virtual ~PlanSelect (void);

	virtual void Execute (void);

protected:

	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * POPULATION_FILE;
	static char * NEW_HOUSEHOLD_LIST;
	static char * NEW_HOUSEHOLD_RECORD_FILE;
	static char * NEW_HOUSEHOLD_RECORD_FORMAT;
	static char * SELECT_TRAVELERS;
	static char * SELECT_NODES_x;
	static char * SELECT_LINKS_x;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_TRAVEL_MODES;
	static char * SELECT_FACILITY_TYPES;
	static char * SELECT_LOCATIONS;
	static char * SELECT_PARKING_LOTS;
	static char * SELECT_TRANSIT_STOPS;
	static char * SELECT_TRANSIT_ROUTES;
	static char * SELECT_VC_RATIOS;
	static char * SELECT_TIME_RATIOS;
	static char * SELECT_COORDINATES;
	static char * SELECT_COORDINATES_x;
	static char * SELECT_OD_COORDINATES;
	static char * SELECT_OD_COORDINATES_x;
	static char * EXCLUDE_OD_COORDINATES;
	static char * EXCLUDE_OD_COORDINATES_x;
	static char * SELECT_SUBAREA_POLYGON;
	static char * SELECT_VEHICLE_TYPES;
	static char * PERCENT_TIME_DIFFERENCE;
	static char * MINIMUM_TIME_DIFFERENCE;
	static char * MAXIMUM_TIME_DIFFERENCE;
	static char * SELECTION_PERCENTAGE;
	static char * MAXIMUM_PERCENT_SELECTED;
	static char * RANDOM_NUMBER_SEED;

	virtual void Program_Control (void);

private:
	enum PlanSelect_Reports { SUBAREA_IO = 1 };

	bool delay_flag, type_flag, pop_flag, vc_flag, diff_flag, select_flag, vehicle_flag;
	bool parking_flag, turn_flag, coord_flag, out_flag, in_flag, subarea_flag, ratio_flag;
	bool transit_flag, mode_flag [DRIVER_MODE+1], facility_flag [EXTERNAL+1], factype_flag, hhrec_flag;
	int num_ii, num_ie, num_ei, num_ee;
	int min_time, max_time, num_hhold;
	double vc_ratio, time_ratio, percent_diff, percent, max_percent;

	Db_File pop_file;
	Arcview_File select_subarea;

	Plan_File plan_file;
	Ext_File hhlist_file;
	HHRec_File hhrec_file;
	HHRec_Array hhrec_data;

	Random random;
	Time_Range times;
	Data_Range travelers, locations, parking_lots, transit_stops, vehicle_types, transit_routes;
	Data_Array nodes, links;
	Integer_Sort hhold_list;

	//---- coordinate box ----

	typedef struct {
		int x1;
		int y1;
		int x2;
		int y2;
	} Box;

	Data_Array coord_box, od_box, exclude_box;

	//---- person list data ----

	typedef struct {
		int person;
		int hhold;
	} Person_Data;

	Index_Array person_data;

	//---- methods ----

	void Read_Population (void);
	void Read_Plan (void);

};
#endif
