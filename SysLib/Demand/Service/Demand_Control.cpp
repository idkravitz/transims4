//*********************************************************
//	Demand_Control.cpp - process the control parameters
//*********************************************************

#include "Demand_Service.hpp"

#include "Household_File.hpp"
#include "Population_File.hpp"
#include "Delay_File.hpp"
#include "Ridership_File.hpp"
#include "Vehicle_File.hpp"
#include "Veh_Type_File.hpp"
#include "Activity_File.hpp"
#include "Trip_File.hpp"
#include "Boundary_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Demand_Service::Program_Control (void)
{
	int max_errors;
	char *str_ptr, *format_ptr;
	Format_Type default_format, file_format, demand_format;
	Access_Type access;

	//---- open network files ----

	Network_Service::Program_Control ();
	
	demand_format = default_format = Demand_Format ();

	File_Data *file = demand_file;

	//---- get the default demand file format ----

	format_ptr = Get_Control_String (DEMAND_FILE_FORMAT);

	if (format_ptr != NULL) {
		demand_format = Format_Code (format_ptr);
		if (demand_format != default_format) {
			Print (2, "Demand File Format = %s", format_ptr);
		}
	} else {
		demand_format = default_format;
	}
	Demand_Format (demand_format);

	//---- open the files ----

	for (int i=0; i < END_DEMAND; i++, file++) {
		if (file->flag || file->option) {

			//---- initialize overlaid data classes ----

			if (i == LINK_DELAY || i == NEW_LINK_DELAY) {
				if (dir_array == NULL) {
					dir_array = (Dir_Array *) new TTime_Array ();
				}
				if (connect_array == NULL) {
					connect_array = (Connect_Array *) new Connect_Time_Array ();
				}
			} else if (i == PERFORMANCE || i == NEW_PERFORMANCE) {
				if (dir_array == NULL) {
					dir_array = (Dir_Array *) new Performance_Array ();
				}
				if (connect_array == NULL) {
					connect_array = (Connect_Array *) new Connect_Time_Array ();
				}
			} else if (i == RIDERSHIP || i == NEW_RIDERSHIP) {
				if (line_array == NULL) {
					line_array = (Line_Array *) new Rider_Array ();
				}
			}

			//---- read the key ----

			str_ptr = Get_Control_String (file->key);

			if (str_ptr == NULL) {
				if (file->flag) goto control_error;
			} else {
				Print (1);
				file->flag = true;

				format_ptr = Get_Control_String (file->format);

				if (format_ptr != NULL) {
					file_format = Format_Code (format_ptr);
				} else {
					file_format = demand_format;
				}
				access = file->access;

				switch (i) {
					case HOUSEHOLD:
					case NEW_HOUSEHOLD:
						file->file = new Household_File (Project_Filename (str_ptr), access, file_format);
						break;

					case POPULATION:
					case NEW_POPULATION:
						file->file = new Population_File (Project_Filename (str_ptr), access, file_format);
						break;

					case LINK_DELAY:
					case PERFORMANCE:
						file->file = new Delay_File (Project_Filename (str_ptr), access, file_format);

						if (((Db_Base *) file->file)->Dbase_Format () == VERSION3) {
							if (i == LINK_DELAY) {
								ttime_data.Period_Size (((Delay_File *) file->file)->Increment ());
								connect_time.Period_Size (ttime_data.Period_Size ());
							} else {
								performance_data.Period_Size (((Delay_File *) file->file)->Increment ());
								connect_time.Period_Size (performance_data.Period_Size ());
							}
						}

						//---- max link delay errors ----

						str_ptr = Get_Control_String (MAX_LINK_DELAY_ERRORS);

						if (str_ptr != NULL) {
							Get_Integer (str_ptr, &max_errors);

							if (max_delay_errors < 0) {
								Error ("Maximum Link Delay Errors %d is Out of Range", max_errors);
							}
							Print (1, "Maximum Number of Link Delay Errors = %d", max_errors);
							Max_Delay_Errors (max_errors);
						}
						break;

					case NEW_LINK_DELAY:
					case NEW_PERFORMANCE:
						file->file = new Delay_File (Project_Filename (str_ptr), access, file_format, Time_Increment ());
						break;

					case RIDERSHIP:
					case NEW_RIDERSHIP:
						file->file = new Ridership_File (Project_Filename (str_ptr), access, file_format);
						break;

					case VEHICLE_TYPE:
					case NEW_VEHICLE_TYPE:
						file->file = new Veh_Type_File (Project_Filename (str_ptr), access, file_format);
						break;

					case VEHICLE:
					case NEW_VEHICLE:
						file->file = new Vehicle_File (Project_Filename (str_ptr), access, file_format);

						//---- sort vehicles ----
						
						if (i == VEHICLE) {
							if (Get_Control_String (SORT_VEHICLES) != NULL) {
								sort_flag = Get_Control_Flag (SORT_VEHICLES);
							}
							if (sort_flag) {
								Print (1, "Vehicle File will be Sorted by Vehicle ID");
							} else {
								Print (1, "Vehicle File is Sorted by Vehicle ID");
							}
						}
						break;

					case ACTIVITY:
					case NEW_ACTIVITY:
						str_ptr = Project_Filename (str_ptr);

						if (Enable_Partitions ()) {
							Activity_File * act_file = new Activity_File (access, file_format);
							file->file = act_file;

							if (Partition ()) {
								act_file->Filename (str_ptr, Partition_Number ());
							} else {
								act_file->Filename (str_ptr);
							}
							Print_Filename (1, act_file->File_Type (), act_file->Groupname ());

							if (!act_file->Open (0)) {
								File_Error (act_file->File_Type (), act_file->Filename ());
							}
						} else {
							file->file = new Activity_File (str_ptr, access, file_format);
						}
						break;

					case TRIP:
					case NEW_TRIP:
						str_ptr = Project_Filename (str_ptr);

						if (Enable_Partitions ()) {
							Trip_File * trip_file = new Trip_File (access, file_format);
							file->file = trip_file;

							if (Partition ()) {
								trip_file->Filename (str_ptr, Partition_Number ());
							} else {
								trip_file->Filename (str_ptr);
							}
							Print_Filename (1, trip_file->File_Type (), trip_file->Groupname ());

							if (!trip_file->Open (0)) {
								File_Error (trip_file->File_Type (), trip_file->Filename ());
							}
						} else {
							file->file = new Trip_File (str_ptr, access, file_format);
						}
						break;

					case BOUNDARY_SPEED:
					case NEW_BOUNDARY_SPEED:
						file->file = new Boundary_File (Project_Filename (str_ptr), access, file_format);
						break;
				}
			}
		}
	}
	Demand_Format (default_format);
	control_flag = true;
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
