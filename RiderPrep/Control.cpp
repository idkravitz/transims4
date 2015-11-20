//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "RiderPrep.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void RiderPrep::Program_Control (void)
{
	char *str_ptr, *format_ptr;

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- open the subarea ridership file ----

	str_ptr = Get_Control_String (SUBAREA_RIDERSHIP_FILE);	

	if (str_ptr == NULL) goto control_error;

	subarea_file.File_Type ("Subarea Ridership File");
	Print (1);

	format_ptr = Get_Control_String (SUBAREA_RIDERSHIP_FORMAT);

	if (format_ptr != NULL) {
		subarea_file.Dbase_Format (format_ptr);
	}
	subarea_file.Open (Project_Filename (str_ptr));

	//---- output all stops flag ----

	str_ptr = Get_Control_String (OUTPUT_ALL_TRANSIT_STOPS);

	if (str_ptr != NULL) {
		Print (2, "Output All Transit Stops = %s", str_ptr);

		stops_flag = Get_Control_Flag (OUTPUT_ALL_TRANSIT_STOPS);
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!trip_time.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (1, "Time of Day Format = %s", str_ptr);

	//---- read report types ----

	List_Reports ();

	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
