//*********************************************************
//	Read_Volume.cpp - Read the Volume Files
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Read_Volume
//---------------------------------------------------------

void Validate::Read_Volume (Link_Data_File *file, bool type)
{
	int i, link, anode, bnode, ab_dir, ba_dir, num, nmap, p1, p2, vol;
	int *volume_map, *volume;

	TTime_Data *ttime_ptr;
	Link_Data *link_ptr;
	Range_Data *range_ptr;
    
	//---- read the volume file ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	//---- build the volume map ----

	nmap = file->Num_Ranges () + 1;

	volume_map = new int [nmap];

	num = time_periods.Num_Ranges () + 1;

	volume = new int [num];

	if (volume_map == NULL || volume == NULL) {
		Error ("Insufficient Memory for the Volume Map");
	}

	for (i=1, range_ptr = file->First (); range_ptr != NULL; range_ptr = file->Next (), i++) {
		p1 = time_periods.In_Index (range_ptr->Low ());
		p2 = time_periods.In_Index (range_ptr->High ());

		volume_map [i] = time_periods.In_Index ((range_ptr->Low () + range_ptr->High () + 1) / 2);

		if (p1 != p2 || p1 != volume_map [i]) {
			Db_Field *field = file->Field (file->AB (i));
			Warning ("Link Data Field (%s) does not fit the Time Period", field->Name ());
		}
	}

	//---- read the count data ----

	while (file->Read ()) {
		Show_Progress ();

		link = file->Link ();
		if (link == 0) continue;

		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Error ("Link %d was Not Found in the Link File", link);
		}
		if (select_flag && !type_flag [link_ptr->Type ()]) continue;

		anode = file->Anode ();
		bnode = file->Bnode ();

		if (anode != 0 && bnode != 0) {
			if (anode == link_ptr->Anode () && bnode == link_ptr->Bnode ()) {
				ab_dir = link_ptr->AB_Dir ();
				ba_dir = link_ptr->BA_Dir ();
			} else if (bnode == link_ptr->Anode () && anode == link_ptr->Bnode ()) {
				ab_dir = link_ptr->BA_Dir ();
				ba_dir = link_ptr->AB_Dir ();
			} else {
				Error ("Anode %d or Bnode %d were Not Found on Link %d", anode, bnode, link);
			}
		} else {
			ab_dir = link_ptr->AB_Dir ();
			ba_dir = link_ptr->BA_Dir ();
		}

		//---- add the counts to the directional links ----

		if (ab_dir > 0) {
			memset (volume, '\0', num * sizeof (int));

			for (i=1; i < nmap; i++) {
				if (volume_map [i] > 0) {
					volume [volume_map [i]] += file->Volume_AB (i);
				}
			}
			ttime_ptr = ttime_data [ab_dir];

			if (ttime_ptr->Periods () == 0) {
				if (!ttime_ptr->Periods (num-1)) goto dir_error;
			}

			for (i=1; i < num; i++) {
				if (type) {
					vol = ttime_ptr->Count (i);
					if (vol > 0) {
						vol = (vol + volume [i]) / 2;
					} else {
						vol = volume [i];
					}
					ttime_ptr->Count (i, vol);
				} else {
					vol = ttime_ptr->Volume (i);
					if (vol > 0) {
						vol = (vol + volume [i]) / 2;
					} else {
						vol = volume [i];
					}
					ttime_ptr->Volume (i, vol);
					ttime_ptr->Count (i, 0);
				}
			}
		}
		if (ba_dir > 0) {
			memset (volume, '\0', num * sizeof (int));

			for (i=1; i < nmap; i++) {
				if (volume_map [i] > 0) {
					volume [volume_map [i]] += file->Volume_BA (i);
				}
			}
			ttime_ptr = ttime_data [ba_dir];

			if (ttime_ptr->Periods () == 0) {
				if (!ttime_ptr->Periods (num-1)) goto dir_error;
			}

			for (i=1; i < num; i++) {
				if (type) {
					vol = ttime_ptr->Count (i);
					if (vol > 0) {
						vol = (vol + volume [i]) / 2;
					} else {
						vol = volume [i];
					}
					ttime_ptr->Count (i, vol);
				} else {
					vol = ttime_ptr->Volume (i);
					if (vol > 0) {
						vol = (vol + volume [i]) / 2;
					} else {
						vol = volume [i];
					}
					ttime_ptr->Volume (i, vol);
					ttime_ptr->Count (i, 0);
				}
			}
		}
	}
	End_Progress ();
	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	file->Close ();

	delete [] volume_map;
	delete [] volume;
	return;

dir_error:
	Error ("Insufficient Memory for Time Period Data");
}

