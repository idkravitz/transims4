//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TPPlusNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TPPlusNet::Execute (void)
{
	//---- read the conversion script ----

	if (convert_flag) {
		int num;
		Db_Base *files [3];

		if (Report_Flag (SCRIPT_REPORT)) {
			Header_Number (SCRIPT_REPORT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		if (link_shape_flag) {
			files [0] = &link_shape_file;
		} else {
			files [0] = &link_file;
		}
		files [1] = Network_Db_Base (NEW_LINK);
		if (detail_flag) {
			files [2] = &detail_file;
			num = 3;
		} else {
			num = 2;
		}
		convert.Set_Files (num, files);

		if (!convert.Compile (&script_file, Report_Flag (SCRIPT_REPORT))) {
			Error ("Compiling Conversion Script");
		}
		if (Report_Flag (STACK_REPORT)) {
			Header_Number (STACK_REPORT);

			convert.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- setup network data ----

	Network_Service::Execute ();

	//---- read the speed capacity file ----

	if (spdcap_flag) {
		Read_SpdCap ();
	}

	//---- read the node file ----

	Read_Node ();

	//---- read the link file ----

	Read_Link ();

	//---- write the link file ----

	Write_Link ();

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TPPlusNet::Page_Header (void)
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
