//*********************************************************
//	Read_Household.cpp - Read Each Household File
//*********************************************************

#include "ProblemSelect.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Household
//---------------------------------------------------------

void ProblemSelect::Read_Household (void)
{
	int hhold, hh, num_hh, num_sel, num;
	char *record, buffer [16];
	bool select;
	double share;

	if (hh_flag) {

		//---- set the filename and extension ----

		if (hhold_file.Extend ()) {
			Show_Message ("Reading Household File %s -- Record", hhold_file.Extension ());
		} else {
			Show_Message ("Reading Household File -- Record");
		}
		Set_Progress (10000);

		//---- read each plan record ----

		num_hh = num_sel = 0;

		record = hhold_file.Record ();
		hh_list.Reset ();

		while (hhold_file.Read ()) {
			Show_Progress ();

			Get_Integer (record, &hhold);

			if (hhold == 0) continue;

			num_hh++;

			if (!hhold_list.Get_Index (hhold)) continue;

			if (!hh_list.Get_Index (hhold)) {
				num_sel++;
				if (!hh_list.Add (hhold)) {
					Error ("Adding Household to the List");
				}
			}
		}
		End_Progress ();

		hhold_file.Close ();

		Print (2, "Household File: %s", hhold_file.Filename ());
		Print (1, "\tTotal Number of Households = %d", num_hh);
	} else {
		num_hh = hhold_list.Num_Records ();
		num_sel = hh_list.Num_Records ();

		Print (2, "Writing Household File");
		if (new_hhold_file.Extend ()) {
			Print (0, " %s", new_hhold_file.Extension ());
		}
		Print (1, "\tNumber of Problem Households = %d", num_hh);
	}

	nhhold += num_hh;
	if (num_hh < 1) num_hh = 1;

	Print (1, "\tNumber of Households Selected = %d (%.1lf%%)", 
		num_sel, (100.0 * num_sel / num_hh));

	//---- select the households to re-route ----

	select = select_flag;
	share = percent;

	//---- check the maximum selection ----

	if (num_sel * percent / num_hh > max_percent) {
		select = true;
		share = max_percent * num_hh / num_sel;
	}

	//---- write the selected household ids ----

	num = 0;

	for (hh = hh_list.First (); hh != 0; hh = hh_list.Next ()) {
		if (select) {
			if (random.Probability () > share) continue;
		}
		str_fmt (buffer, sizeof (buffer), "%d", hh);

		if (!new_hhold_file.Write (buffer)) {
			Error ("Writing %s", new_hhold_file.File_Type ());
		}
		num++;
	}
	new_hhold_file.Close ();

	nselect += num;

	Print (1, "\tNumber of Households Written = %d (%.1lf%%)", 
		num, (100.0 * num / num_hh));
}
