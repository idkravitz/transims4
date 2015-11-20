//*********************************************************
//	ArcNet.hpp - Network Conversion Utility
//*********************************************************

#ifndef ARCNET_HPP
#define ARCNET_HPP

#include "Network_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Offset_Data.hpp"
#include "Time_Range.hpp"
#include "Utility.hpp"
#include "Db_Array.hpp"
#include "Subzone_File.hpp"
#include "Integer_Array.hpp"

//---------------------------------------------------------
//	ArcNet - execution class definition
//---------------------------------------------------------

class ArcNet : public Network_Service
{
public:
	ArcNet (void);
	virtual ~ArcNet (void);

	virtual void Execute (void);

	virtual bool Node_Processing (Db_File *file);
	virtual bool Zone_Processing (Db_File *file);
	virtual bool Link_Processing (Db_File *file);
	virtual bool Pocket_Processing (Db_File *file);
	virtual bool Lane_Use_Processing (Db_File *file);
	virtual bool Connectivity_Processing (Db_File *file);
	virtual bool Turn_Processing (Db_File *file);
	virtual bool Parking_Processing (Db_File *file);
	virtual bool Location_Processing (Db_File *file);
	virtual bool Access_Processing (Db_File *file);
	virtual bool Sign_Processing (Db_File *file);
	virtual bool Signal_Processing (Db_File *file);
	virtual bool Timing_Processing (Db_File *file);
	virtual bool Phasing_Processing (Db_File *file);
	virtual bool Detector_Processing (Db_File *file);
	virtual bool Coordinator_Processing (Db_File *file);
	virtual bool Stop_Processing (Db_File *file);
	virtual bool Toll_Processing (Db_File *file);
	virtual bool Route_Header_Processing (Db_File *file);

protected:

	static char * SUBZONE_DATA_FILE;
	static char * ARCVIEW_DIRECTORY;
	static char * DRAW_NETWORK_LANES;
	static char * LANE_WIDTH;
	static char * CENTER_ONEWAY_LINKS;
	static char * LINK_DIRECTION_OFFSET;
	static char * DRAW_AB_DIRECTION;
	static char * POCKET_LANE_SIDE_OFFSET;
	static char * PARKING_SIDE_OFFSET;
	static char * ACTIVITY_LOCATION_SIDE_OFFSET;
	static char * UNSIGNALIZED_NODE_SIDE_OFFSET;
	static char * UNSIGNALIZED_NODE_SETBACK;
	static char * TRANSIT_STOP_SIDE_OFFSET;
	static char * TRANSIT_DIRECTION_OFFSET;
	static char * TRANSIT_TIME_PERIODS;
	static char * TRANSIT_OVERLAP_FLAG;
	static char * VISUALIZER_GUIDEWAY_FILE;
	static char * DRAW_ONEWAY_ARROWS;
	static char * ONEWAY_ARROW_LENGTH;
	static char * ONEWAY_ARROW_SIDE_OFFSET;
	static char * SELECT_TIME_PERIOD;
	static char * USE_FIXED_SETBACK_FOR_ARCS;
	static char * FIXED_SETBACK_DISTANCE;
	static char * USE_CONNECTIVITY_ARCS;

	virtual void Program_Control (void);

private:

	Arcview_File arcview_node, arcview_zone, arcview_link;
	Arcview_File arcview_pocket, arcview_lane_use;
	Arcview_File arcview_location, arcview_parking, arcview_access;
	Arcview_File arcview_turn, arcview_connect, arcview_sign, arcview_coordinator;
	Arcview_File arcview_signal, arcview_phasing, arcview_timing, arcview_detector; 
	Arcview_File arcview_stop, arcview_route, arcview_toll;
	Arcview_File guideway_file, arcview_subzone;

	Offset_Array parking_offset;
	Offset_Array location_offset;
	Offset_Array stop_offset;

	Subzone_File subzone_file;

	Db_Data_Array timing_db, coordinator_db, route_header_db;

	Point_Data points;

	double link_offset, pocket_side, parking_side, location_side, sign_side, sign_setback, stop_side;
	double route_offset, lane_width, arrow_length, arrow_side;

	bool route_flag, schedule_flag, driver_flag, route_node_flag, lanes_flag, center_flag, overlap_flag;
	bool timing_flag, coordinator_flag, guideway_flag, arrow_flag, direction_flag, subzone_flag;
	bool select_time;

	int run_field;
	Time_Range time_period, time_range;
	Integer_Sort timing_list;

	Projection_Service projection;	
	//double length_units, speed_units;

	void Write_Subzone (void);

	char * Shapename (char *arcdir, char *str_ptr);

	void Write_Timing (void);
	void Write_Coordinator (void);
	void Write_Route (void);
	void Write_Driver (void);
	void Route_Nodes (void);

	void Add_Arrow (Point_Data *points);

	static char shapename [FILE_NAME_SIZE];

	bool rounded_connectivity;
	bool setback_override_flag;
	double setback_distance;

};
#endif


