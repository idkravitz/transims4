//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "GISNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void GISNet::Execute (void)
{

	//---- read the conversion script ----

	if (convert_flag) {
		if (Report_Flag (SCRIPT_REPORT)) {
			Header_Number (SCRIPT_REPORT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		convert.Set_Files (2, &gis_file, Network_Db_Base (NEW_LINK));

		if (!convert.Compile (&script_file, Report_Flag (SCRIPT_REPORT))) {
			Error ("Compiling Conversion Script");
		}
		if (Report_Flag (STACK_REPORT)) {
			Header_Number (STACK_REPORT);

			convert.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- gis node file ----

	if (node_flag) {
		Read_Node ();
	}

	//---- gis link file ----

	Read_Link ();

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void GISNet::Page_Header (void)
{
	switch (Header_Number ()) {
		case SCRIPT_REPORT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case STACK_REPORT:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		default:
			break;
	}
}
