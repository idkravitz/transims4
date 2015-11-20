//*********************************************************
//	Network_Service.hpp - network data class
//*********************************************************

#ifndef NETWORK_SERVICE_HPP
#define NETWORK_SERVICE_HPP

#include "Execution_Service.hpp"
#include "Db_Base.hpp"
#include "Network_Code.hpp"
#include "Point_Array.hpp"
#include "Compass.hpp"
#include "Projection.hpp"

#include "Event_Output.hpp"
#include "Problem_Output.hpp"
#include "Snapshot_Output.hpp"
#include "Summary_Output.hpp"
#include "System_Event_Output.hpp"
#include "Traveler_Output.hpp"
#include "Speed_Bin_Output.hpp"

#include "Node_Data.hpp"
#include "Zone_Data.hpp"
#include "Shape_Data.hpp"
#include "Link_Data.hpp"
#include "Dir_Data.hpp"
#include "AB_Data.hpp"
#include "Pocket_Data.hpp"
#include "Lane_Use_Data.hpp"
#include "Link_Use_Data.hpp"
#include "Toll_Data.hpp"
#include "Connect_Data.hpp"
#include "Turn_Data.hpp"
#include "Parking_Data.hpp"
#include "Location_Data.hpp"
#include "Access_Data.hpp"
#include "Sign_Data.hpp"
#include "Signal_Data.hpp"
#include "Timing_Data.hpp"
#include "Phasing_Data.hpp"
#include "Detector_Data.hpp"
#include "Coordinator_Data.hpp"
#include "Stop_Data.hpp"
#include "Fare_Data.hpp"
#include "Line_Data.hpp"
#include "Driver_Data.hpp"
#include "Route_Data.hpp"

#define node_data			(*node_array)
#define zone_data			(*zone_array)
#define shape_data			(*shape_array)
#define link_data			(*link_array)
#define dir_data			(*dir_array)
#define ab_key				(*ab_key_array)
#define pocket_data			(*pocket_array)
#define lane_use_data		(*lane_use_array)
#define link_use_data		(*link_use_array)
#define toll_data			(*toll_array)
#define connect_data		(*connect_array)
#define turn_data			(*turn_array)
#define parking_data		(*parking_array)
#define location_data		(*location_array)
#define access_data			(*access_array)
#define sign_data			(*sign_array)
#define signal_data			(*signal_array)
#define timing_data			(*timing_array)
#define phasing_data		(*phasing_array)
#define detector_data		(*detector_array)
#define coordinator_data	(*coordinator_array)
#define stop_data			(*stop_array)
#define fare_data			(*fare_array)
#define line_data			(*line_array)
#define driver_data			(*driver_array)
#define route_data			(*route_array)

//---------------------------------------------------------
//	Network_Service - network class definition
//---------------------------------------------------------

class Network_Service : public Execution_Service, public Network_Code
{
	friend class Event_Output;
	friend class Problem_Output;
	friend class Snapshot_Output;
	friend class Summary_Output;
	friend class System_Event_Output;
	friend class Traveler_Output;
	friend class Turn_Output;
	friend class Speed_Bin_Output;

public:
	enum Network_File {
		DIRECTORY, NODE, ZONE, SHAPE, LINK, 
		POCKET_LANE, LANE_USE, TOLL, LANE_CONNECTIVITY, TURN_PROHIBITION, 
		PARKING, ACTIVITY_LOCATION, PROCESS_LINK, 
		UNSIGNALIZED_NODE, SIGNALIZED_NODE, 
		PHASING_PLAN, TIMING_PLAN, DETECTOR, SIGNAL_COORDINATOR,
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, 
		TRANSIT_SCHEDULE, TRANSIT_DRIVER, ROUTE_HEADER, ROUTE_NODES,
		NEW_DIRECTORY, NEW_NODE, NEW_ZONE, NEW_SHAPE, NEW_LINK, 
		NEW_POCKET_LANE, NEW_LANE_USE, NEW_TOLL, NEW_LANE_CONNECTIVITY, NEW_TURN_PROHIBITION, 
		NEW_PARKING, NEW_ACTIVITY_LOCATION, NEW_PROCESS_LINK, 
		NEW_UNSIGNALIZED_NODE, NEW_SIGNALIZED_NODE, 
		NEW_PHASING_PLAN, NEW_TIMING_PLAN, NEW_DETECTOR, NEW_SIGNAL_COORDINATOR,
		NEW_TRANSIT_STOP, NEW_TRANSIT_FARE, NEW_TRANSIT_ROUTE, 
		NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER, NEW_ROUTE_HEADER, NEW_ROUTE_NODES,
		END_NETWORK};

	Network_Service (void);
	virtual ~Network_Service (void);

	virtual void Execute (void);

	virtual bool Node_Processing (Db_File *file);
	virtual bool Zone_Processing (Db_File *file);
	virtual bool Shape_Processing (Db_File *file);
	virtual bool Link_Processing (Db_File *file);
	virtual bool Pocket_Processing (Db_File *file);
	virtual bool Lane_Use_Processing (Db_File *file);
	virtual bool Toll_Processing (Db_File *file);
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
	virtual bool Line_Processing (Db_File *file);
	virtual bool Driver_Processing (Db_File *file);
	virtual bool Route_Header_Processing (Db_File *file);

protected:
	static char * CREATE_NOTES_AND_NAME_FIELDS;
	static char * file_error;

	void   Required_Network_Files (Network_File files []);
	void   Optional_Network_Files (Network_File files []);

	bool   Network_File_Flag (Network_File file)	{ return ((file < END_NETWORK) ? network_file [file].flag : false); }
	void   Network_File_False (Network_File file)	{ if (file < END_NETWORK) { network_file [file].flag = false; network_file [file].option = false; }}
	void   Network_File_True (Network_File file)	{ if (file < END_NETWORK) network_file [file].flag = true; }

	bool   Network_Option_Flag (Network_File file)	{ return ((file < END_NETWORK) ? network_file [file].option : false); }
	void   Network_Option_False (Network_File file)	{ if (file < END_NETWORK) network_file [file].option = false; }
	void   Network_Option_True (Network_File file)	{ if (file < END_NETWORK) network_file [file].option = true; }

	Access_Type Network_File_Access (Network_File file)	{ return ((file < END_NETWORK) ? network_file [file].access : READ); }
	void   Network_File_Access (Network_File file, Access_Type access)	{ if (file < END_NETWORK) network_file [file].access = access; }
	
	Format_Type Network_Format (void)				{ return ((Format_Type) Network_Code::Default_Format ()); }
	void Network_Format (Format_Type format)		{ Network_Code::Default_Format (format); }

	bool   Network_Data_Flag (Network_File file)	{ return ((file < END_NETWORK) ? network_file [file].data : false); }
	void   Network_Data_True (Network_File file)	{ if (file < END_NETWORK) network_file [file].data = true; }

	void   Resolution (int value);
	int    Resolution (void)				{ return (resolution); }

	void   Check_Data (bool value)			{ check_data = value; }
	bool   Check_Data (void)				{ return (check_data); }
	bool   Check_Message (char *format, ...);

	void   Renumber (bool value)			{ renumber = value; }
	bool   Renumber (void)					{ return (renumber); }

	void   First_Record (bool value)		{ first_record = value; }
	bool   First_Record (void)				{ return (first_record); }

	void   AB_Key_Flag (bool value)			{ ab_flag = value; }
	bool   AB_Key_Flag (void)				{ return (ab_flag); }

	void   Lane_Use (bool value)			{ lane_use = value; }
	bool   Lane_Use (void)					{ return (lane_use); }

	bool   Projection_Flag (void)			{ return (projection_flag); }
	void   Set_Projection (Projection *p)	{ projection_flag = ((projection = p) != NULL); }
	Projection * Get_Projection (void)		{ return (projection); }

	double UnRound (int value)				{ return ((double) value / resolution); }
	int    Round (double value)				{ return ((int) (value * resolution + ((value < 0) ? -0.5 : 0.5))); }
	int    Resolve (int value)				{ return ((value + res_round) / resolution); }

	void   Network_Directory (char *dir);
	char * Network_Directory (void)			{ return (network_directory); }

	char * Network_Filename (char *name, char *ext = NULL)	
											{ return (Filename (Network_Directory (), name, ext)); }

	void   New_Directory (char *dir);
	char * New_Directory (void)				{ return (new_directory); }

	char * New_Filename (char *name, char *ext = NULL)	
											{ return (Filename (New_Directory (), name, ext)); }
	
	Db_File * Network_Db_File (Network_File file)	{ return ((file < END_NETWORK) ? network_file [file].file : NULL); }
	Db_Base * Network_Db_Base (Network_File file)	{ return ((Db_Base *) Network_Db_File (file)); }

	bool Network_Copy_Header (Network_File from, Network_File to);

	int Num_Fare_Zones (void)				{ return (num_fare_zone); }
	bool Zero_Fare_Zone (void)				{ return (zero_flag); }
	void Zero_Fare_Zone (bool flag)			{ zero_flag = flag; }

	int  Bearing_Offset (void)				{ return (bearing_offset); }
	void Bearing_Offset (int value)			{ bearing_offset = value; }

	Compass_Points compass;

	bool Link_Shape (Link_Data *link_ptr, int dir, Point_Data *points, double offset = -1.0, double length = -1.0, double side = 0.0);

	bool Notes_Name_Flag (void)				{ return (notes_flag); }
	void Notes_Name_Flag (bool flag)		{ notes_flag = flag; }

	virtual void Program_Control (void);

	//---- data arrays ----

	Node_Array *node_array;
	Zone_Array *zone_array;
	Shape_Array *shape_array;
	Link_Array *link_array;
	Dir_Array *dir_array;
	AB_Key *ab_key_array;
	Pocket_Array *pocket_array;
	Lane_Use_Array *lane_use_array;
	Link_Use_Array *link_use_array;
	Toll_Array *toll_array;
	Connect_Array *connect_array;
	Turn_Array *turn_array;
	Parking_Array *parking_array;
	Location_Array *location_array;
	Access_Array *access_array;
	Sign_Array *sign_array;
	Signal_Array *signal_array;
	Timing_Array *timing_array;
	Phasing_Array *phasing_array;
	Detector_Array *detector_array;
	Coordinator_Array *coordinator_array;
	Stop_Array *stop_array;
	Fare_Array *fare_array;
	Line_Array *line_array;
	Driver_Array *driver_array;
	Route_Array *route_array;

private:
	typedef struct {
		char    *key;
		char    *format;
		Db_File *file;
		bool     flag;
		bool     option;
		bool     data;
		Access_Type  access;
	} File_Data;

	File_Data network_file [END_NETWORK];

	bool projection_flag;
	bool check_data, renumber, first_record, ab_flag, lane_use, notes_flag, control_flag, zero_flag;
	int bearing_offset, num_fare_zone, resolution, res_round;

	char *network_directory, *new_directory;

	Projection *projection;

	//---- methods ----

	void Read_Node (void);
	void Read_Zone (void);
	void Read_Shape (void);
	void Read_Link (void);
	void Read_Pocket (void);
	void Read_Lane_Use (void);
	void Read_Toll (void);
	void Read_Connect (void);
	void Read_Turn (void);
	void Read_Parking (void);
	void Read_Location (void);
	void Read_Access (void);
	void Read_Sign (void);
	void Read_Signal (void);
	void Read_Timing (void);
	void Read_Phasing (void);
	void Read_Detector (void);
	void Read_Coordinator (void);
	void Read_Stop (void);
	void Read_Fare (void);
	void Read_Line (void);
	void Read_Schedule (void);
	void Read_Driver (void);
	void Read_Route_Header (void);
	void Read_Route_Nodes (void);

	void Time_of_Day_Restrictions ();
};
#endif
