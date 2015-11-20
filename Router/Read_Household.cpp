//*********************************************************
//	Read_Household.cpp - Read the Household List and Data Files
//*********************************************************

#include "Router.hpp"
#include "Utility.hpp"
#include "Household_File.hpp"

//---------------------------------------------------------
//	Read_Household
//---------------------------------------------------------

void Router::Read_Household (void)
{
	int hhold, last_hh, type, max_type, num_rec;

	Household_Type hh_rec, *hh_ptr;
	Household_File *hhold_file = 0;

	//---- process the household type script ----

	if (hhold_flag) {

		hhold_file = (Household_File *) Demand_Db_Base (HOUSEHOLD);
		Demand_File_False (HOUSEHOLD);

		//---- read the household type script ----

		if (Report_Flag (HOUSEHOLD_SCRIPT)) {
			Header_Number (HOUSEHOLD_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		hhold_type.Set_Files (1, hhold_file);

		if (!hhold_type.Compile (&hhold_type_file, Report_Flag (HOUSEHOLD_SCRIPT))) {
			Error ("Compiling Household Type Script");
		}
		if (Report_Flag (HOUSEHOLD_STACK)) {
			Header_Number (HOUSEHOLD_STACK);

			hhold_type.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- initialize the hhold data ----

	if (hhlist_flag) {
		num_rec = hhlist_file.Num_Records ();

		if (num_rec == 0) {
			Error ("No Household List Records");
		}

		if (!household_type.Max_Records (num_rec)) {
			Error ("Insufficient Memory for %d Household List Records", num_rec);
		}

		Show_Message ("Reading %s -- Record", hhlist_file.File_Type ());
		Set_Progress ();

		//---- store the household list ----

		nhh_list = 0;

		while (hhlist_file.Read ()) {
			Show_Progress ();

			Get_Integer (hhlist_file.Record (), &hhold);

			if (hhold <= 0) continue;

			hh_rec.Household (hhold);
			hh_rec.Person (0);
			hh_rec.Record (0);
			hh_rec.Type (0);

			if (!household_type.Add (&hh_rec)) {
				Error ("Adding Household %d", hhold);
			}
			nhh_list++;
		}
		End_Progress ();

		hhlist_file.Close ();

		Print (2, "Number of Household List Records = %d", Progress_Count ());

		household_type.Max_Records (household_type.Num_Records ());

	} else if (hhrec_flag) {

		num_rec = hhrec_file.Num_Records ();

		if (num_rec == 0) {
			Error ("%s is Empty", hhrec_file.File_Type ());
		}

		if (!household_type.Max_Records (num_rec)) {
			Error ("Insufficient Memory for %d Household Records", num_rec);
		}

		Show_Message ("Reading %s -- Record", hhrec_file.File_Type ());
		Set_Progress ();

		//---- store the household records ----

		nhh_list = last_hh = 0;

		while (hhrec_file.Read ()) {
			Show_Progress ();

			hhold = hhrec_file.Household ();
			if (hhold <= 0) continue;

			hh_rec.Household (hhold);
			hh_rec.Person (hhrec_file.Person ());
			hh_rec.Record (hhrec_file.Record ());
			hh_rec.Type (0);

			if (!household_type.Add (&hh_rec)) {
				Error ("Adding Household %d", hhold);
			}
			if (hhold != last_hh) {
				nhh_list++;
				last_hh = hhold;
			}
		}
		End_Progress ();

		hhrec_file.Close ();

		Print (2, "Number of %s Records = %d", hhrec_file.File_Type (), Progress_Count ());

		household_type.Max_Records (household_type.Num_Records ());
	}

	if (!hhold_flag) return;

	if (!hhlist_flag && !hhrec_flag) {
		hhold = hhold_file->Estimate_Records ();

		if (hhold == 0) {
			Error ("No Household Records");
		}
		if (!household_type.Max_Records (hhold)) {
			Error ("Insufficient Memory for %d Household Records", hhold);
		}
	}

	Show_Message ("Reading %s -- Record", hhold_file->File_Type ());
	Set_Progress (50000);
	max_type = 0;

	//---- store the household type ----

	while (hhold_file->Read ()) {
		Show_Progress ();

		hhold = hhold_file->Household ();
		if (hhold <= 0) continue;

		if (hhlist_flag) {
			hh_ptr = household_type.Get (hhold);
			if (hh_ptr == NULL) continue;
		} else if (hhrec_flag) {
			hh_ptr = household_type.Get_GE (hhold);
			if (hh_ptr == NULL || hh_ptr->Household () != hhold) continue;
		} else {
			hh_ptr = household_type.New_Record (true);
			if (hh_ptr == NULL) {
				Error ("Adding Household %d", hhold);
			}
			hh_ptr->Household (hhold);

			household_type.Add ();
		}
		type = hhold_type.Execute ();

		if (type < 1) {
			Error ("Household %d Type Value %d is Out of Range", hhold, type);
		}
		if (type > max_type) max_type = type;

		hh_ptr->Type (type);

		if (hhrec_flag) {
			for (;;) {
				hh_ptr = household_type.Next_Key ();
				if (hh_ptr == NULL || hh_ptr->Household () != hhold) break;
				hh_ptr->Type (type);
			}
		}
	}
	End_Progress ();

	hhold_file->Close ();
	hhold_type.Clear ();

	Print (2, "Number of Household Records = %d", Progress_Count ());

	//---- scan the household list ----

	if (hhlist_flag) {
		for (hh_ptr = household_type.First (); hh_ptr != NULL; hh_ptr = household_type.Next ()) {
			if (hh_ptr->Type () == 0) {
				Error ("A Household Record was Not Found for Household %d", hh_ptr->Household ());
			}
		}
	} else {
		household_type.Max_Records (household_type.Num_Records ());
	}
	Print (1, "Maximum Household Type = %d", max_type);
}
