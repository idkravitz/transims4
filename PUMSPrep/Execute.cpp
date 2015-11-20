//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PUMSPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PUMSPrep::Execute (void)
{

	if (script_flag) {
		Write (1, "Compiling Conversion Script");

		if (Report_Flag (PRINT_SCRIPT)) {
			Header_Number (PRINT_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		program.Set_Files (2, &new_hh_file, &new_pop_file);

		if (!program.Compile (&program_file, Report_Flag (PRINT_SCRIPT))) {
			Error ("Compiling Conversion Script");
		}
		if (Report_Flag (PRINT_STACK)) {
			Header_Number (PRINT_STACK);

			program.Print_Commands (false);
		}
	}

	//---- copy the household and population data ----

	if (input_flag) {
		Copy_Data ();
	}

	//---- read the PUMS file ----

	for (int i=0; i < num_pums; i++) {
		if (pums_file [i] != NULL) {
			Read_PUMS (pums_file [i]);
		}
	}

	//---- processing messages ----

	new_hh_file.Close ();
	new_pop_file.Close ();

	Write (2, "Number of PUMS Data Records = %d", nrec);
	Write (1, "\tHousehold Records = %d", pums_hh);
	Write (1, "\tPopulation Records = %d", pums_pop);

	Write (1);
	if (input_flag) {
		Write (1, "Number of Input Household Records = %d", hh_in);
		Write (1, "Number of Input Population Records = %d", pop_in);
	}
	Write (1, "Number of Output Household Records = %d", hh_out);
	if (input_flag) {
		Write (0, " (%d new)", hh_out - hh_in);
	}
	Write (1, "Number of Output Population Records = %d", pop_out);
	if (input_flag) {
		Write (0, " (%d new)", pop_out - pop_in);
	}
	Exit_Stat (DONE);
}
