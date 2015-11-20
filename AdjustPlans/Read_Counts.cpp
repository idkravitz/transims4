//*********************************************************
//	Read_Counts.cpp - read traffic count file
//*********************************************************

#include "AdjustPlans.hpp"

#include "Time_Step.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Counts
//---------------------------------------------------------

void AdjustPlans::Read_Counts (void)
{
	int anode, bnode, start, end, count, lnk, link, group;
	char *str_ptr;

	Count_Data count_rec, *count_ptr;
	AB_Data *ab_ptr;
	Dir_Data *dir_ptr;
	Integer_List *list;

	Show_Message ("Reading %s -- Plan", count_file.File_Type ());
	Set_Progress (10000);

	Time_Step time_data (1, Time_Step::HOURS);

	//---- renumber the link equiv ----

	if (equiv_flag) {
		int i, *lnk;

		for (group=1; group <= link_equiv.Max_Group (); group++) {
			list = link_equiv.Group_List (group);
			if (list == NULL) continue;

			for (i=1; i <= list->Num_Records (); i++) {
				lnk = list->Record (i);

				if (*lnk < 0) {
					link = -(link_data.Get_Index (-(*lnk)));
				} else {
					link = link_data.Get_Index (*lnk);
				}
				if (link != 0) {
					*lnk = link;
				} else {
					Write (1, "Warning: Link %d on Link Group %d was Not Found", *lnk, group);
					*lnk = MAX_INTEGER;
				}
			}
		}
	}

	//---- read the count file ----

	while (count_file.Read ()) {
		Show_Progress ();

		//---- convert the anode and bnode ----

		anode = count_file.Anode ();
		if (anode == 0) continue;

		count = (int) (count_file.Count () * factor + 0.5);
		if (count <= 0) continue;

		count_rec.Count (count);

		bnode = count_file.Bnode ();

		ab_ptr = ab_key.Get (anode, bnode);

		if (ab_ptr == NULL) {
			Write (1, "Warning: A Count Link between %d and %d was Not Found", anode, bnode);
			continue;
		}

		//---- get the start and end time ----

		str_ptr = count_file.Start_Time ();
		if (*str_ptr != '\0') {
			start = time_data.Step (str_ptr);
		} else {
			start = 0;
		}
		count_rec.Start_Time (Round (start));
		
		str_ptr = count_file.End_Time ();
		if (*str_ptr != '\0') {
			end = time_data.Step (str_ptr);
		} else {
			end = time_data.Step ("24:00");
		}
		count_rec.End_Time (Round (end));

		if (count_rec.End_Time () <= count_rec.Start_Time ()) {
			char start_time [20], end_time [20];
			str_cpy (start_time, sizeof (start_time), count_file.Start_Time ());
			str_cpy (end_time, sizeof (end_time), count_file.End_Time ());

			Error ("Count Time Range %s to %s is Illogical", start_time, end_time);
		}
		count_rec.Volume (0);

		//---- check for screenline processing ----

		if (equiv_flag) {
			dir_ptr = dir_data [ab_ptr->dir];

			link = dir_ptr->Link ();
			if (dir_ptr->Dir () > 0) link = -link;

			for (group=1; group <= link_equiv.Max_Group (); group++) {
				list = link_equiv.Group_List (group);
				if (list == NULL) continue;

				for (lnk = list->First (); lnk != 0; lnk = list->Next ()) {
					if (lnk == link) {
						count_rec.Dir_Index (group);

						count_ptr = count_data.Get (&count_rec);

						if (count_ptr == NULL) {
							if (!count_data.Add (&count_rec)) goto mem_error;
						} else {
							count_ptr->Add_Count (count);
						}
					}
				}
			}
			continue;
		}

		//---- confirm that the key is unique ----

		count_rec.Dir_Index (ab_ptr->dir);

		if (count_data.Get_Index (&count_rec) != 0) {
			Write (1, "Warning: Duplicate Count for Nodes %d-%d at %s", anode, bnode, str_ptr);
			continue;
		}
		if (!count_data.Add (&count_rec)) goto mem_error;
	}
	End_Progress ();

	count_data.Optimize ();

	count_file.Close ();
	return;

mem_error:
	Error ("Insufficient Memory for Traffic Count Data");
}
