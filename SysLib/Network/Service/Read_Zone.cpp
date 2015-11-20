//*********************************************************
//	Read_Zone.cpp - read the zone file
//*********************************************************

#include "Network_Service.hpp"

#include "Zone_File.hpp"

//---------------------------------------------------------
//	Read_Zone
//---------------------------------------------------------

void Network_Service::Read_Zone (void)
{
	Db_File *file = Network_Db_File (ZONE);

	//---- store the zone data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();
	
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Zone_Processing (file)) {
			if (!zone_data.Add ()) {
				Error ("Adding Record to Zone Data");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = zone_data.Num_Records ();

	if (num) {
		if (num != Progress_Count ()) {
			Print (1, "Number of Zone Data Records = %d", num);
		}
		Zone_Data *zone_ptr = zone_data.Last_Key ();
		if (zone_ptr->Zone () != num) {
			Print (1, "Highest Zone Number = %d", zone_ptr->Zone ());
		}
	}
	zone_data.Optimize ();

	if (num > 0) Network_Data_True (ZONE);
}

//---------------------------------------------------------
//	Zone_Processing
//---------------------------------------------------------

bool Network_Service::Zone_Processing (Db_File *fh)
{
	double x, y;

	Zone_Data *zone_ptr;

	Zone_File *file = (Zone_File *) fh;

	//---- reserve memory ----

	if (zone_data.Max_Records () == 0) {
		if (!zone_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	zone_ptr = zone_data.New_Record ();
	if (zone_ptr == NULL) goto mem_error;

	zone_ptr->Zone (file->Zone ());
	if (zone_ptr->Zone () == 0) return (false);
	
	x = file->X ();
	y = file->Y ();

	if (Projection_Flag ()) {
		if (!(Get_Projection ()->Convert (&x, &y))) {
			return (Check_Message ("Converting Coordinate %lf, %lf", x, y));
		}
	}
	zone_ptr->X (Round (x));
	zone_ptr->Y (Round (y));
	zone_ptr->Area_Type (file->Area_Type ());

	return (true);

mem_error:
	Error ("Insufficient Memory for Zone Data");
	return (false);
}
