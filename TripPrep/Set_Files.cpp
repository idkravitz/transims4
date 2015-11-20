//*********************************************************
//	Set_Files.cpp - Setup Data Files for User Program
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Set_Files
//---------------------------------------------------------

void TripPrep::Set_Files (void)
{
	Db_Base *data_file [4];

	int i = 0;

	if (trip_flag) {
		data_file [i++] = Demand_Db_Base (TRIP);
		data_file [i++] = Demand_Db_Base (NEW_TRIP);
	}
	if (activity_flag) {
		data_file [i++] = Demand_Db_Base (ACTIVITY);
		data_file [i++] = Demand_Db_Base (NEW_ACTIVITY);
	}
	Print (1);
	Write (1, "Compiling Data Processing Script");

	if (Report_Flag (PRINT_SCRIPT)) {
		Header_Number (PRINT_SCRIPT);

		if (!Break_Check (10)) {
			Print (1);
			Page_Header ();
		}
	}
	program.Set_Files (i, data_file);

	if (!program.Compile (&program_file, Report_Flag (PRINT_SCRIPT))) {
		Error ("Compiling Data Processing Script");
	}
	program.Random_Seed (random.Seed ());

	if (Report_Flag (PRINT_STACK)) {
		Header_Number (PRINT_STACK);

		program.Print_Commands (false);
	}
	Show_Message (1);
} 
