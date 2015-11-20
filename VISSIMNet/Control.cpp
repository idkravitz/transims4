//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "VISSIMNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void VISSIMNet::Program_Control (void)
{
	char *str_ptr;

	//---- create the network files ----

	Demand_Service::Program_Control ();

	new_flag = Network_File_Flag (NEW_NODE) || Network_File_Flag (NEW_LINK) ||
		Network_File_Flag (NEW_SHAPE) || Network_File_Flag (NEW_POCKET_LANE) ||
		Network_File_Flag (NEW_LANE_CONNECTIVITY) || 
		Network_File_Flag (NEW_ACTIVITY_LOCATION) ||
		Network_File_Flag (NEW_PARKING) || Network_File_Flag (NEW_PROCESS_LINK) ||
		Network_File_Flag (NEW_UNSIGNALIZED_NODE) || Network_File_Flag (NEW_SIGNALIZED_NODE) ||
		Network_File_Flag (NEW_TIMING_PLAN) || Network_File_Flag (NEW_PHASING_PLAN) ||
		Network_File_Flag (NEW_DETECTOR) || Network_File_Flag (NEW_SIGNAL_COORDINATOR);

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_range.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);
	
	//---- get the select time period ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIOD);

	if (str_ptr != NULL) {
		Print (2, "Select Time Period = %s", str_ptr);

		if (!time_range.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
	} else {
		time_range.Add_Ranges ("0:00..23:59:59");
	}

	//---- open the vissim xml file  ----
	
	str_ptr = Get_Control_String (NEW_VISSIM_XML_FILE);

	if (str_ptr != NULL) {
		xml_file.File_Type ("New VISSIM XML File");
		Print (1);

		xml_file.Create (Project_Filename (str_ptr));
	}
} 
