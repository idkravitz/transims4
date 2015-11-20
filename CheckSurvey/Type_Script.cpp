//*********************************************************
//	Type_Script.cpp - read the household type scripts
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Type_Script
//---------------------------------------------------------

void CheckSurvey::Type_Script (void)
{
	Print (1);

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
