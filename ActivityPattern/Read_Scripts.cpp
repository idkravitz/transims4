//*********************************************************
//	Read_Scripts.cpp - read the user program scripts
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Read_Scripts
//---------------------------------------------------------

void ActivityPattern::Read_Scripts (void)
{
	Print (1);

	//---- read the household type script ----

	if (hhold_type_flag) {
		Write (1, "Compiling Household Type Script");

		if (Report_Flag (HOUSEHOLD_SCRIPT)) {
			Header_Number (HOUSEHOLD_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		hhold_type.Set_Files (1, Demand_Db_Base (HOUSEHOLD));

		if (!hhold_type.Compile (&hhold_type_file, Report_Flag (HOUSEHOLD_SCRIPT))) {
			Error ("Compiling Household Type Script");
		}
		if (Report_Flag (HOUSEHOLD_STACK)) {
			Header_Number (HOUSEHOLD_STACK);

			hhold_type.Print_Commands (false);
		}
	}

	//---- read the survey type script ----

	if (survey_type_flag) {
		Write (1, "Compiling Survey Type Script");

		if (Report_Flag (SURVEY_SCRIPT)) {
			Header_Number (SURVEY_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		survey_type.Set_Files (1, &survey_hh_file);

		if (!survey_type.Compile (&survey_type_file, Report_Flag (SURVEY_SCRIPT))) {
			Error ("Compiling Survey Type Script");
		}
		if (Report_Flag (SURVEY_STACK)) {
			Header_Number (SURVEY_STACK);

			survey_type.Print_Commands (false);
		}
	}
	Header_Number (0);
	Show_Message (1);
}
