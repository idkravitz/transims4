//*********************************************************
//	Read_Location.cpp - read the activity location file
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Read_Location
//---------------------------------------------------------

void PlanCompare::Read_Location (void)
{
	Location_Data *location_ptr = NULL;

	//---- reserve memory ----

	if (compare_loc_data.Max_Records () == 0) {
		if (!compare_loc_data.Max_Records (compare_loc_file.Num_Records ())) {
			Error ("Insufficient Memory for Activity Location Data");
		}
	}
	Show_Message ("Reading %s -- Record", compare_loc_file.File_Type ());
	Set_Progress ();

	while (compare_loc_file.Read ()) {
		Show_Progress ();

		//---- read and save the data ----
			
		location_ptr = compare_loc_data.New_Record ();

		location_ptr->Location (compare_loc_file.Location ());

		if (location_ptr->Location () == 0) continue;

		location_ptr->Link (compare_loc_file.Link ());
		location_ptr->Dir (0);

		//----- round the link offset ----

		location_ptr->Offset (Round (compare_loc_file.Offset ()));
		location_ptr->X (Round (compare_loc_file.X ()));
		location_ptr->Y (Round (compare_loc_file.Y ()));

		//----- optional fields ----

		location_ptr->Zone (compare_loc_file.Zone ());

		if (!compare_loc_data.Add ()) {
			Error ("Adding Location Record");
		}
	}
	End_Progress ();
	compare_loc_file.Close ();

	Print (2, "Number of %s Records = %d", compare_loc_file.File_Type (), Progress_Count ());
}
