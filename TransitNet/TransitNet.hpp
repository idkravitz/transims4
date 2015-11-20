//*********************************************************
//	TransitNet.hpp - Transit Conversion Utility
//*********************************************************

#ifndef TRANSITNET_HPP
#define TRANSITNET_HPP

#include "Network_Service.hpp"
#include "Db_Header.hpp"
#include "Random.hpp"
#include "Zone_Equiv.hpp"
#include "Integer_Array.hpp"
#include "Time_Range.hpp"

#include "Data/Route_Stop.hpp"
#include "Data/Route_Node.hpp"
#include "Data/Path_Node.hpp"
#include "Data/Node_Path_Data.hpp"
#include "Data/Dir_Path_Data.hpp"

#define MIN_LENGTH		37.5
#define MAX_AREA_TYPE	16

#undef  node_data
#define node_data		(*((Node_Path_Array *) node_array))

#undef  dir_data
#define dir_data		(*((Dir_Path_Array *) dir_array))

//---------------------------------------------------------
//	TransitNet - execution class definition
//---------------------------------------------------------

class TransitNet : public Network_Service
{
public:
	TransitNet (void);
	virtual ~TransitNet (void);

	virtual void Execute (void);
	
	virtual bool Location_Processing (Db_File *file);
	virtual bool Parking_Processing (Db_File *file);
	virtual bool Access_Processing (Db_File *file);
	virtual bool Stop_Processing (Db_File *file);

protected:
	
	static char * PARK_AND_RIDE_FILE;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * STOP_SPACING_BY_AREATYPE;
	static char * TRANSIT_TIME_PERIODS;
	static char * TRANSIT_TRAVEL_TIME_FACTOR;
	static char * MINIMUM_DWELL_TIME;
	static char * INTERSECTION_STOP_TYPE;
	static char * INTERSECTION_STOP_OFFSET;
	static char * TRANSIT_PROCESS_LINK_TIME;
	static char * RANDOM_NUMBER_SEED;

	virtual void Program_Control (void);

private:
	enum TransitNet_Reports {ZONE_EQUIV = 1};

	bool parkride_flag, connect_flag, equiv_flag, dwell_flag, time_flag, speed_flag;

	int naccess, nlocation, nparking, new_stop, route_edit, schedule_edit, driver_edit;
	int nstop, nroute, nschedule, ndriver, end_warnings, parking_warnings, connect_warnings;
	int max_parking, max_access, max_location, max_stop, nparkride, access_time;
	int min_dwell, num_periods, stop_type, left_turn, bus_code, rail_code, stop_offset;
	int min_stop_spacing [MAX_AREA_TYPE+1];
	double *time_factor;

	Random random;
	Time_Range time_period;

	Db_Header parkride_file;

	Route_Stop_Array route_stop;
	Route_Node_Array route_node;
	Path_Node_Array path_node;

	Integer_Sort edit_list;

	Zone_Equiv zone_equiv;

	//---- methods ----

	void Data_Setup (void);
	void Connection_List (void);
	void Read_ParkRide (void);
	void Link_AreaType (void);
	void Read_Route (void);
	void Node_Path (int node1, int node2, Use_Type use);
	void Save_Route (Route_Data *route_ptr);
	void Write_Stop (void);
	void Write_Route (void);
	void Write_Driver (void);
};
#endif
