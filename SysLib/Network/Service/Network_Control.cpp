//*********************************************************
//	Network_Control.cpp - process the control parameters
//*********************************************************

#include "Network_Service.hpp"

#include "Node_File.hpp"
#include "Zone_File.hpp"
#include "Shape_File.hpp"
#include "Link_File.hpp"
#include "Pocket_File.hpp"
#include "Lane_Use_File.hpp"
#include "Toll_File.hpp"
#include "Turn_File.hpp"
#include "Connect_File.hpp"
#include "Parking_File.hpp"
#include "Location_File.hpp"
#include "Access_File.hpp"
#include "Sign_File.hpp"
#include "Signal_File.hpp"
#include "Timing_File.hpp"
#include "Phasing_File.hpp"
#include "Detector_File.hpp"
#include "Coordinator_File.hpp"
#include "Stop_File.hpp"
#include "Fare_File.hpp"
#include "Line_File.hpp"
#include "Schedule_File.hpp"
#include "Driver_File.hpp"
#include "Route_Header_File.hpp"
#include "Route_Nodes_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Network_Service::Program_Control (void)
{
	bool first, new_flag, notes_flag;
	char *str_ptr, *format_ptr;
	Format_Type default_format, file_format, network_format;
	Access_Type access;

	first = true;
	new_flag = false;
	network_format = default_format = Network_Format ();

	if (Get_Control_String (CREATE_NOTES_AND_NAME_FIELDS) != NULL) {
		Notes_Name_Flag (Get_Control_Flag (CREATE_NOTES_AND_NAME_FIELDS));
	}
	notes_flag = Notes_Name_Flag ();

	File_Data *file = network_file;

	//---- open the files ----

	for (int i=0; i < END_NETWORK; i++, file++) {
		if (file->flag || file->option) {
			if (i == DIRECTORY) {
				first = true;

				//---- get the network format ----

				format_ptr = Get_Control_String (file->format);

				if (format_ptr != NULL) {
					network_format = Format_Code (format_ptr);
					if (network_format != default_format) {
						Print (2, "Default Network Format = %s", format_ptr);
						first = false;
					}
				} else {
					network_format = default_format;
				}
				Network_Format (network_format);
			} else if (i == NEW_DIRECTORY) {
				first = true;

				//---- get the new network format ----

				format_ptr = Get_Control_String (file->format);

				if (format_ptr != NULL) {
					network_format = Format_Code (format_ptr);
					if (network_format != default_format) {
						Print (2, "Default New Network Format = %s", format_ptr);
						first = false;
					}
				} else {
					network_format = default_format;
				}
				Network_Format (network_format);
			}

			//---- get the filename ----

			str_ptr = Get_Control_String (file->key);

			if (str_ptr == NULL) {
				if (file->flag) goto control_error;
			} else {
				if (first) {
					Print (1);
					first = false;
				}
				file->flag = true;

				if (i == DIRECTORY) {
					Print_Filename (1, "Network Directory", str_ptr);
					Network_Directory (str_ptr);

				} else if (i == NEW_DIRECTORY) {
					Print_Filename (1, "New Network Directory", str_ptr);
					New_Directory (str_ptr);

				} else {
					format_ptr = Get_Control_String (file->format);

					if (format_ptr != NULL) {
						file_format = Format_Code (format_ptr);
						//if (file_format != network_format) Print (1);
					} else {
						file_format = network_format;
					}
					access = file->access;

					if (i > NEW_DIRECTORY) {
						new_flag = true;
						str_ptr = New_Filename (str_ptr);
					} else {
						str_ptr = Network_Filename (str_ptr);
					}

					switch (i) {
						case NODE:
						case NEW_NODE:
							file->file = new Node_File (str_ptr, access, file_format, notes_flag);
							break;
						case ZONE:
						case NEW_ZONE:
							file->file = new Zone_File (str_ptr, access, file_format, notes_flag);
							break;
						case SHAPE:
						case NEW_SHAPE:
							file->file = new Shape_File (str_ptr, access, file_format, notes_flag);
							break;
						case LINK:
						case NEW_LINK:
							file->file = new Link_File (str_ptr, access, file_format, notes_flag);
							break;
						case POCKET_LANE:
						case NEW_POCKET_LANE:
							file->file = new Pocket_File (str_ptr, access, file_format, notes_flag);
							break;
						case LANE_USE:
						case NEW_LANE_USE:
							file->file = new Lane_Use_File (str_ptr, access, file_format, notes_flag);
							break;
						case TOLL:
						case NEW_TOLL:
							file->file = new Toll_File (str_ptr, access, file_format, notes_flag);
							break;
						case LANE_CONNECTIVITY:
						case NEW_LANE_CONNECTIVITY:
							file->file = new Connect_File (str_ptr, access, file_format, notes_flag);
							break;
						case TURN_PROHIBITION:
						case NEW_TURN_PROHIBITION:
							file->file = new Turn_File (str_ptr, access, file_format, notes_flag);
							if (((Turn_File *) (file->file))->Node_Based ()) AB_Key_Flag (true);
							break;
						case PARKING:
						case NEW_PARKING:
							file->file = new Parking_File (str_ptr, access, file_format, notes_flag);
							break;
						case ACTIVITY_LOCATION:
						case NEW_ACTIVITY_LOCATION:
							file->file = new Location_File (str_ptr, access, file_format, notes_flag);
							break;
						case PROCESS_LINK:
						case NEW_PROCESS_LINK:
							file->file = new Access_File (str_ptr, access, file_format, notes_flag);
							break;
						case UNSIGNALIZED_NODE:
						case NEW_UNSIGNALIZED_NODE:
							file->file = new Sign_File (str_ptr, access, file_format, notes_flag);
							break;
						case SIGNALIZED_NODE:
						case NEW_SIGNALIZED_NODE:
							file->file = new Signal_File (str_ptr, access, file_format, notes_flag);
							break;
						case TIMING_PLAN:
						case NEW_TIMING_PLAN:
							file->file = new Timing_File (str_ptr, access, file_format, notes_flag);
							break;
						case PHASING_PLAN:
						case NEW_PHASING_PLAN:
							file->file = new Phasing_File (str_ptr, access, file_format, notes_flag);
							break;
						case DETECTOR:
						case NEW_DETECTOR:
							file->file = new Detector_File (str_ptr, access, file_format, notes_flag);
							break;
						case SIGNAL_COORDINATOR:
						case NEW_SIGNAL_COORDINATOR:
							file->file = new Coordinator_File (str_ptr, access, file_format, notes_flag);
							break;
						case TRANSIT_STOP:
						case NEW_TRANSIT_STOP:
							file->file = new Stop_File (str_ptr, access, file_format, notes_flag);
							break;
						case TRANSIT_FARE:
						case NEW_TRANSIT_FARE:
							file->file = new Fare_File (str_ptr, access, file_format, notes_flag);
							break;
						case TRANSIT_ROUTE:
						case NEW_TRANSIT_ROUTE:
							file->file = new Line_File (str_ptr, access, file_format, notes_flag);
							break;
						case TRANSIT_SCHEDULE:
						case NEW_TRANSIT_SCHEDULE:
							file->file = new Schedule_File (str_ptr, access, file_format, notes_flag);
							break;
						case TRANSIT_DRIVER:
						case NEW_TRANSIT_DRIVER:
							file->file = new Driver_File (str_ptr, access, file_format, notes_flag);
							break;
						case ROUTE_HEADER:
						case NEW_ROUTE_HEADER:
							file->file = new Route_Header_File (str_ptr, access, file_format, notes_flag);
							break;
						case ROUTE_NODES:
						case NEW_ROUTE_NODES:
							file->file = new Route_Nodes_File (str_ptr, access, file_format, notes_flag);
							break;
						default:
							file->file = new Db_Header (Project_Filename (str_ptr));
							break;
					}
				}
			}
		}
	}
	if (new_flag && notes_flag) {
		Print (2, "Notes and Name Fields will be Created");
	}
	Network_Format (default_format);
	control_flag = true;
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
