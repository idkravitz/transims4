//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LocationData::Execute (void)
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

	//---- read each subzone data file ----

	if (subzone_map_flag) {
		Read_Subzone_Map ();
	}
	for (i=0; i < num_subzone; i++) {
		Read_Subzone (i);
	}

	//---- read zone boundary file ----

	if (zone_flag) {
		Read_Boundary ();
	}

	//---- read boundary polygon files ----

	if (polygon_flag) {
		Read_Polygons ();
	}

	//---- read the network data ----

	Network_File_False (ACTIVITY_LOCATION);
	
	Network_Service::Execute ();

	//---- read the activity location file ----

	Read_Location ();

	//---- print the zone coverage report ----

	if (Report_Flag (ZONE_CHECK)) {
		Check_Zones ();
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void LocationData::Page_Header (void)
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
