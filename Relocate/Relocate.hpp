//*********************************************************
//	Relocate.hpp - Update activity locations
//*********************************************************

#ifndef RELOCATE_HPP
#define RELOCATE_HPP

#include "Demand_Service.hpp"
#include "Ext_File.hpp"
#include "Node_File.hpp"
#include "Link_File.hpp"
#include "Location_File.hpp"
#include "Parking_File.hpp"
#include "Access_File.hpp"
#include "Plan_File.hpp"
#include "Activity_File.hpp"
#include "Trip_File.hpp"
#include "Integer_Array.hpp"
#include "List_Data.hpp"
#include "Complex_Array.hpp"
#include "Random.hpp"

#define MIN_DIFF	100		//---- 100 meters ----

//---------------------------------------------------------
//	Relocate - execution class definition
//---------------------------------------------------------

class Relocate : public Demand_Service
{
public:
	Relocate (void);
	virtual ~Relocate (void);

	virtual void Execute (void);

protected:

	static char * ZONE_RELOCATION_FILE;
	static char * RANDOM_NUMBER_SEED;
	static char * PLAN_FILE;
	static char * PLAN_FORMAT;
	static char * NODE_LIST_PATHS;
	static char * NEW_PLAN_FILE;
	static char * NEW_PLAN_FORMAT;
	static char * OLD_NODE_FILE;
	static char * OLD_LINK_FILE;
	static char * OLD_ACTIVITY_LOCATIONS;
	static char * OLD_PARKING_FILE;
	static char * OLD_PROCESS_LINKS;
	static char * NEW_HOUSEHOLD_LIST;
	static char * NEW_LOCATION_MAP_FILE;
	static char * NEW_PARKING_MAP_FILE;
	static char * NEW_CHANGE_FILE;

	virtual void Program_Control (void);

private:

	bool vehicle_flag, activity_flag, trip_flag, plan_flag, hhlist_flag, loop_flag;
	bool loc_map_flag, park_map_flag, network_flag, type_flag, zone_flag, change_flag;

	int total_in, total_out, total_update, total_hhold;
	
	Activity_File *activity_file, *new_act_file;
	Trip_File *trip_file, *new_trip_file;
	Db_File zone_file, change_file;

	Node_File old_node_file;
	Link_File old_link_file;
	Location_File old_loc_file;
	Parking_File old_park_file;
	Access_File old_acc_file;

	Parking_Array old_park_data;
	Link_Array old_link_data;
	AB_Key old_ab_key;

	Db_Header new_loc_file;
	Db_Header new_park_file;

	Ext_File hhlist_file;

	Plan_File plan_file, new_plan_file;

	typedef struct {
		int old_id;
		int new_id;
	} Map_Data;

	Index_Array activity_map, parking_map, node_map, old_link_map, old_new_map, loc_park, veh_map;

	typedef struct {
		int zone;
		int new_id;
		double weight;
	} Zone_Weight;

	Complex_Array zone_weight;

	Random random;

	Integer_Sort zone_list;
	Integer_List node_list, new_link_list, loc_zone;
	List_Array node_link;

	//---- methods ----

	void Read_Zone_List (void);
	void Match_Locations (void);
	void Map_Network (void);
	void Read_Trip (int file_num);
	void Read_Activity (int file_num);
	bool Read_Plan (int file_num);
	void Read_Vehicle (void);
	int  Zone_Location (int hhold, int person, int trip, int location, int vehicle = 0, bool org_flag = false);
};

#endif
