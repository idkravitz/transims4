//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "SubareaNet.hpp"

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SubareaNet::Program_Control (void)
{
	char *str_ptr;
	Db_Header *file;

	//---- open network files ----

	Network_Service::Program_Control ();

	if (Network_File_Flag (TRANSIT_STOP) || Network_File_Flag (TRANSIT_ROUTE) ||
		Network_File_Flag (TRANSIT_SCHEDULE) || Network_File_Flag (TRANSIT_DRIVER) ||
		Network_File_Flag (NEW_TRANSIT_STOP) || Network_File_Flag (NEW_TRANSIT_ROUTE) ||
		Network_File_Flag (NEW_TRANSIT_SCHEDULE) || Network_File_Flag (NEW_TRANSIT_DRIVER)) {

		transit_flag = (Network_File_Flag (TRANSIT_STOP) && Network_File_Flag (TRANSIT_ROUTE) &&
			Network_File_Flag (TRANSIT_SCHEDULE) && Network_File_Flag (TRANSIT_DRIVER) &&
			Network_File_Flag (NEW_TRANSIT_STOP) && Network_File_Flag (NEW_TRANSIT_ROUTE) &&
			Network_File_Flag (NEW_TRANSIT_SCHEDULE) && Network_File_Flag (NEW_TRANSIT_DRIVER));

		if (!transit_flag) {
			Error ("Existing and New Transit Stop, Route, Schedule, and Driver Files are Required");
		}
	}
		
	//---- get the subarea boundary ----

	str_ptr = Get_Control_String (SUBAREA_BOUNDARY_POLYGON);

	if (str_ptr == NULL) goto control_error;
	Print (1);
	subarea_boundary.File_Type ("Subarea Boundary Polygon");

	subarea_boundary.Open (Project_Filename (str_ptr));
	
	if (!subarea_boundary.Read_Record ()) {
		Error ("Reading the Boundary Polygon");
	}

	//---- get the external offset length ----

	str_ptr = Get_Control_String (EXTERNAL_OFFSET_LENGTH);

	if (str_ptr != NULL) {
		external_offset = atoi (str_ptr);

		if (external_offset < 1 || external_offset > 50) {
			Error ("External Offset Length %d is Out of Range (1..50)", external_offset);
		}
		external_offset = Round (external_offset);
	}
	Print (2, "External Offset Length = %d meters", Resolve (external_offset));

	//---- replicate the field names ----

	Network_Copy_Header (NODE, NEW_NODE);
	Network_Copy_Header (ZONE, NEW_ZONE);
	Network_Copy_Header (LINK, NEW_LINK);
	Network_Copy_Header (POCKET_LANE, NEW_POCKET_LANE);
	Network_Copy_Header (LANE_CONNECTIVITY, NEW_LANE_CONNECTIVITY);
	Network_Copy_Header (PARKING, NEW_PARKING);
	Network_Copy_Header (ACTIVITY_LOCATION, NEW_ACTIVITY_LOCATION);
	Network_Copy_Header (PROCESS_LINK, NEW_PROCESS_LINK);
	Network_Copy_Header (LANE_USE, NEW_LANE_USE);
	Network_Copy_Header (TURN_PROHIBITION, NEW_TURN_PROHIBITION);
	Network_Copy_Header (TOLL, NEW_TOLL);
	Network_Copy_Header (UNSIGNALIZED_NODE, NEW_UNSIGNALIZED_NODE);
	Network_Copy_Header (SIGNALIZED_NODE, NEW_SIGNALIZED_NODE);
	Network_Copy_Header (TIMING_PLAN, NEW_TIMING_PLAN);
	Network_Copy_Header (PHASING_PLAN, NEW_PHASING_PLAN);
	Network_Copy_Header (DETECTOR, NEW_DETECTOR);
	Network_Copy_Header (SIGNAL_COORDINATOR, NEW_SIGNAL_COORDINATOR);

	if (transit_flag) {
		Network_Copy_Header (TRANSIT_STOP, NEW_TRANSIT_STOP);
		Network_Copy_Header (TRANSIT_ROUTE, NEW_TRANSIT_ROUTE);
		Network_Copy_Header (TRANSIT_SCHEDULE, NEW_TRANSIT_SCHEDULE);
		Network_Copy_Header (TRANSIT_DRIVER, NEW_TRANSIT_DRIVER);
	}

	//---- setup the node database ----

	file = (Db_Header *) Network_Db_Base (NODE);

	node_db.Replicate_Fields (file, true);
	node_db.File_ID ("Node");

	//---- replicate the timing plan fields ----

	if (Network_File_Flag (TIMING_PLAN)) {
		file = (Db_Header *) Network_Db_Base (TIMING_PLAN);

		timing_db.Replicate_Fields (file, true);
		timing_db.File_ID ("Timing");
	}

	//---- replicate the signal coordinator fields ----

	if (Network_File_Flag (SIGNAL_COORDINATOR)) {
		file = (Db_Header *) Network_Db_Base (SIGNAL_COORDINATOR);

		coordinator_db.Replicate_Fields (file, true);
		coordinator_db.File_ID ("Coordinataor");
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
