//*********************************************************
//	Read_Count.cpp - Read the Household Person Count Files
//*********************************************************

#include "TripSum.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Count
//---------------------------------------------------------

void TripSum::Read_Count (void)
{
	int nfile, hhold, person, count;
	char *str_ptr;

	Survey_Person *person_ptr, person_rec;

	//---- read the household person files ----

	for (nfile=0; ; nfile++) {
		if (!count_file.Open (nfile)) break;

		if (count_file.Extend ()) {
			Show_Message ("Reading %s %s -- Record", count_file.File_Type (), count_file.Extension ());
		} else {
			Show_Message ("Reading %s -- Record", count_file.File_Type ());
		}
		Set_Progress ();

		//---- store the household person count ----

		while (count_file.Read ()) {
			Show_Progress ();

			str_ptr = Get_Integer (count_file.Record (), &hhold);

			if (hhold <= 0) continue;

			str_ptr = Get_Integer (str_ptr, &person);
			Get_Integer (str_ptr, &count);

			if (count == 0) continue;

			person_ptr = person_count.Get (hhold, person);

			if (person_ptr == NULL) {
				person_rec.Household (hhold);
				person_rec.Person (person);
				person_rec.Count (count);

				if (!person_count.Add (&person_rec)) {
					Error ("Adding Household Person Count %d-%d", hhold, person);
				}
			} else {
				person_ptr->Add_Count (count);
			}
		}
		End_Progress ();

		count_file.Close ();
	}
	hhold = person_count.Num_Records ();

	person_count.Max_Records (hhold);
	
	Print (2, "Number of Household Person Count Records = %d", hhold);
}
