//*********************************************************
//	Demand_Service.cpp - travel demand data class
//*********************************************************

#include "Demand_Service.hpp"

char * Demand_Service::SORT_VEHICLES			= "SORT_VEHICLES";
char * Demand_Service::DEMAND_FILE_FORMAT		= "DEMAND_FILE_FORMAT";
char * Demand_Service::MAX_LINK_DELAY_ERRORS	= "MAX_LINK_DELAY_ERRORS";

//---------------------------------------------------------
//	Demand_Service constructor
//---------------------------------------------------------

Demand_Service::Demand_Service (void) : Network_Service (), Demand_Code (), Static_Scale ()
{
	static char *keys [] = {
		"HOUSEHOLD_FILE",
		"POPULATION_FILE",
		"LINK_DELAY_FILE",			//---- ttime -----
		"LINK_DELAY_FILE",			//---- performance ----
		"RIDERSHIP_FILE",
		"VEHICLE_TYPE_FILE",
		"VEHICLE_FILE",
		"ACTIVITY_FILE",
		"TRIP_FILE",
		"BOUNDARY_SPEED_FILE",
		"NEW_HOUSEHOLD_FILE",
		"NEW_POPULATION_FILE",
		"NEW_LINK_DELAY_FILE",		//---- ttime ----
		"NEW_LINK_DELAY_FILE",		//---- performance ----
		"NEW_RIDERSHIP_FILE",
		"NEW_VEHICLE_TYPE_FILE",
		"NEW_VEHICLE_FILE",
		"NEW_ACTIVITY_FILE",
		"NEW_TRIP_FILE",
		"NEW_BOUNDARY_SPEED_FILE",
		NULL
	};

	static char *formats [] = {
		"HOUSEHOLD_FORMAT",
		"POPULATION_FORMAT",
		"LINK_DELAY_FORMAT",		//---- ttime ----
		"LINK_DELAY_FORMAT",		//---- performance ----
		"RIDERSHIP_FORMAT",
		"VEHICLE_TYPE_FORMAT",
		"VEHICLE_FORMAT",
		"ACTIVITY_FORMAT",
		"TRIP_FORMAT",
		"BOUNDARY_SPEED_FORMAT",
		"NEW_HOUSEHOLD_FORMAT",
		"NEW_POPULATION_FORMAT",
		"NEW_LINK_DELAY_FORMAT",	//---- ttime ----
		"NEW_LINK_DELAY_FORMAT",	//---- performance ----
		"NEW_RIDERSHIP_FORMAT",
		"NEW_VEHICLE_TYPE_FORMAT",
		"NEW_VEHICLE_FORMAT",
		"NEW_ACTIVITY_FORMAT",
		"NEW_TRIP_FORMAT",
		"NEW_BOUNDARY_SPEED_FORMAT",
		SORT_VEHICLES,
		DEMAND_FILE_FORMAT,
		MAX_LINK_DELAY_ERRORS,
		NULL
	};

	Key_List (keys);
	Key_List (formats);

	//---- initialize the file information ----

	for (int i=0; i < END_DEMAND; i++) {
		demand_file [i].key = keys [i];
		demand_file [i].format = formats [i];
		demand_file [i].file = NULL;
		demand_file [i].flag = false;
		demand_file [i].option = false;
		demand_file [i].data = false;
		demand_file [i].access = (i < NEW_HOUSEHOLD) ? READ : CREATE;

		Disable_Control_Key (keys [i]);
		Disable_Control_Key (formats [i]);
	}
	household_array = NULL;
	population_array = NULL;
	veh_type_array = NULL;
	vehicle_array = NULL;
	activity_array = NULL;
	trip_array = NULL;
	boundary_array = NULL;

	sort_flag = compress_flag = true;
	max_delay_errors = 0;
	control_flag = false;
}

//---------------------------------------------------------
//	Demand_Service destructor
//---------------------------------------------------------

Demand_Service::~Demand_Service (void)
{
	for (int i=0; i < END_DEMAND; i++) {
		if (demand_file [i].file != NULL) {
			delete demand_file [i].file;
			demand_file [i].file = NULL;
		}
	}
	if (household_array != NULL) {
		delete household_array;
		household_array = NULL;
	}
	if (population_array != NULL) {
		delete population_array;
		population_array = NULL;
	}
	if (veh_type_array != NULL) {
		delete veh_type_array;
		veh_type_array = NULL;
	}
	if (vehicle_array != NULL) {
		delete vehicle_array;
		vehicle_array = NULL;
	}
	if (activity_array != NULL) {
		delete activity_array;
		activity_array = NULL;
	}
	if (trip_array != NULL) {
		delete trip_array;
		trip_array = NULL;
	}
	if (boundary_array != NULL) {
		delete boundary_array;
		boundary_array = NULL;
	}
}

//---------------------------------------------------------
//	Required_Demand_Files
//---------------------------------------------------------

void Demand_Service::Required_Demand_Files (Demand_File files [])
{
	for (int i=0; files [i] != END_DEMAND; i++) {
		File_Data *file = demand_file + files [i];

		file->flag = true;
		Activate_Control_Key (file->key);
		Activate_Control_Key (file->format);
	}
}

//---------------------------------------------------------
//	Optional_Demand_Files
//---------------------------------------------------------

void Demand_Service::Optional_Demand_Files (Demand_File files [])
{
	for (int i=0; files [i] != END_DEMAND; i++) {
		File_Data *file = demand_file + files [i];

		file->option = true;
		Activate_Control_Key (file->key);
		Activate_Control_Key (file->format);
	}
}

//---------------------------------------------------------
//	Time_Increment
//---------------------------------------------------------

void Demand_Service::Time_Increment (int increment)
{
	File_Data *file, *new_file;
	
	file = demand_file + PERFORMANCE;
	new_file = demand_file + NEW_PERFORMANCE;

	if (file->flag || file->option || new_file->flag || new_file->option) {
		if (dir_array == NULL) {
			dir_array = (Dir_Array *) new Performance_Array ();
		}
		performance_data.Period_Size (increment);
	} else {
		if (dir_array == NULL) {
			dir_array = (Dir_Array *) new TTime_Array ();
		}
		ttime_data.Period_Size (increment);
	}
	if (connect_array == NULL) {
		connect_array = (Connect_Array *) new Connect_Time_Array ();
	}
	connect_time.Period_Size (increment);
}

//-----------------------------------------------------------
//	Demand_Copy_Header
//-----------------------------------------------------------

bool Demand_Service::Demand_Copy_Header (Demand_File from, Demand_File to)
{ 
	if (Demand_File_Flag (from) && Demand_File_Flag (to)) {
		Db_Header *file, *new_file;

		file = (Db_Header *) Demand_Db_Base (from);
		new_file = (Db_Header *) Demand_Db_Base (to);

		new_file->Clear_Fields ();
		new_file->Replicate_Fields (file);

		return (new_file->Write_Header ());
	}
	return (false);
}
