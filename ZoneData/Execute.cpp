//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ZoneData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ZoneData::Execute (void)
{
	int i;

	//---- set the data files ----

	if (script_flag) {
		Set_Files ();
	}

	//---- read each data file ----

	Show_Message (1);

	for (i=0; i < num_data_files; i++) {
		Read_Data (i);
	}

	//---- read boundary polygon files ----

	if (polygon_flag) {
		Read_Polygons ();
	}

	//---- read the network data ----

	Network_File_False (ZONE);
	
	Network_Service::Execute ();

	//---- read the zone file ----

	Read_Zone ();

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ZoneData::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		default:
			break;
	}
}
