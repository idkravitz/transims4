//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "NewFormat.hpp"

#include "Activity_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void NewFormat::Program_Control (void)
{
	char *str_ptr;

	//---- open the transit driver plan file ----
	
	str_ptr = Get_Control_String (VERSION3_DRIVER_PLANS);

	if (str_ptr != NULL) {
		driver_flag = true;
		driver_plan.File_Type ("Version3 Driver Plans");

		Print (1);
		driver_plan.Open (Project_Filename (str_ptr));

		Network_File_True (NODE);
		Network_File_True (LINK);
		Network_File_True (NEW_TRANSIT_DRIVER);

		AB_Key_Flag (true);
	} else {
		Network_File_False (NODE);
		//Network_File_False (LINK);
		Network_File_False (PARKING);
		Network_File_False (NEW_TRANSIT_DRIVER);
	}

	//---- open the population file ----
	
	str_ptr = Get_Control_String (VERSION3_POPULATION_FILE);

	if (str_ptr != NULL) {
		population_flag = true;
		pop_file.File_Type ("Version3 Population File");

		Print (1);
		pop_file.Open (Project_Filename (str_ptr));

		Demand_File_True (NEW_HOUSEHOLD);
		Demand_File_True (NEW_POPULATION);

	} else {
		Demand_File_False (NEW_HOUSEHOLD);
		Demand_File_False (NEW_POPULATION);
	}

	//---- open the activity file ----
	
	str_ptr = Get_Control_String (VERSION3_ACTIVITY_FILE);

	if (str_ptr != NULL) {
		if (!population_flag) {
			Error ("Version3 Population is needed for Activity Conversion");
		}
		activity_flag = true;
		activity_file.File_Type ("Version3 Activity File");

		Print (1);
		activity_file.Open (Project_Filename (str_ptr));

		Demand_File_True (NEW_ACTIVITY);

		//---- get the time of day format ----
		
		str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

		if (str_ptr == NULL) {
			str_ptr = "HOURS";
		}
		if (!activity_time.Format (str_ptr)) {
			Error ("Time of Day Format %s was Unrecognized", str_ptr);
		}
		Print (2, "Time of Day Format = %s", str_ptr);

		str_ptr = Get_Control_String (VERSION3_SURVEY_ACTIVITY);

		if (str_ptr != NULL) {
			Write (1, "Activity and Survey Conversion need to be Separated");
		}

	} else {

		//---- open the survey activity file ----
		
		str_ptr = Get_Control_String (VERSION3_SURVEY_ACTIVITY);

		if (str_ptr != NULL) {
			survey_flag = true;
			survey_file.File_Type ("Version3 Survey Activity");
			survey_file.Dbase_Format (SPACE_DELIMITED);

			Print (1);
			survey_file.Open (Project_Filename (str_ptr));

			Demand_File_True (NEW_ACTIVITY);
	
			//---- get the time of day format ----
			
			str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

			if (str_ptr == NULL) {
				str_ptr = "HOURS";
			}
			if (!activity_time.Format (str_ptr)) {
				Error ("Time of Day Format %s was Unrecognized", str_ptr);
			}
			Print (2, "Time of Day Format = %s", str_ptr);
		} else {
			Demand_File_False (NEW_ACTIVITY);
		}
	}

	//---- check the flags ----

	if (!driver_flag && !population_flag && !survey_flag && !activity_flag) {
		Error ("No Version3 Files have been Identified");
	}

	//---- open the support files ----

	Demand_Service::Program_Control ();

	if (survey_flag) {
		Activity_File *activity_file;

		//---- get the projection data ----

		xy_flag = proj_service.Read_Control ();

		if (xy_flag) {
			projection.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

			activity_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

			activity_file->Add_Field ("X_COORD", DOUBLE, 14, 2);
			activity_file->Add_Field ("Y_COORD", DOUBLE, 14, 2);

			loc_flag = Network_File_Flag (ACTIVITY_LOCATION);

			if (loc_flag) {
				activity_file->Add_Field ("CODE", INTEGER, 5);
			}
			activity_file->Write_Header ();

		} else {
			Network_File_False (ACTIVITY_LOCATION);
			loc_flag = false;
		}
	}
	return;
} 
