//*********************************************************
//	Zone_Summary.cpp - summarize activity locations on links
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Zone_Summary
//---------------------------------------------------------

void LinkSum::Zone_Summary (void)
{
	int i, j, z, count, link, nrec, nzone, num, volume;
	double factor, vmt, vht, ttime;

	Performance_Data *perf_ptr;
	Link_Data *link_ptr;
	Link_Location *loc_ptr, *first_ptr;

	Show_Message ("Summarize Activity Locations on Links -- Record");
	Set_Progress ();

	link = count = nrec = 0;
	first_ptr = NULL;

	for (loc_ptr = (Link_Location *) link_location.First_Key (); loc_ptr; 
		loc_ptr = (Link_Location *) link_location.Next_Key ()) {
		Show_Progress ();

		if (loc_ptr->link == link) {
			count++;
		} else {
			if (link > 0) {
				link_ptr = link_data.Get (link);

				if (!select_flag || link_ptr->Use () != 0) {
					first_ptr->count = count;
				}
			}
			first_ptr = loc_ptr;
			link = loc_ptr->link;
			count = 1;
		}
	}
	Set_Progress ();

	//---- set the count data for all records ----

	link = 0;

	for (loc_ptr = (Link_Location *) link_location.First_Key (); loc_ptr; 
		loc_ptr = (Link_Location *) link_location.Next_Key ()) {
		Show_Progress ();

		if (loc_ptr->link == link) {
			loc_ptr->count = count;
		} else {
			link = loc_ptr->link;
			count = loc_ptr->count;
		}
		if (count > 0) {
			nrec += count;
			if (zone_list.Get_Index (loc_ptr->zone) == 0) {
				zone_list.Add (loc_ptr->zone);
			}
		}
	}
	nzone = zone_list.Num_Records ();

	Break_Check (4);

	Print (2, "Number of Link Locations = %d", nrec);
	Print (1, "Number of Selected Zones = %d", nzone);

	//---- allocate space for zone summaries ----

	zone_vmt = new double * [num_inc];
	zone_vht = new double * [num_inc];

	if (zone_vmt == NULL || zone_vht == NULL) goto mem_error;

	for (i=0; i < num_inc; i++) {
		zone_vmt [i] = new double [nzone];
		zone_vht [i] = new double [nzone];

		if (zone_vmt [i] == NULL || zone_vht [i] == NULL) goto mem_error;

		memset (zone_vmt [i], '\0', nzone * sizeof (double));
		memset (zone_vht [i], '\0', nzone * sizeof (double));
	}

	//---- summarizes link data by zone ----

	link = 0;

	for (loc_ptr = (Link_Location *) link_location.First_Key (); loc_ptr;
		loc_ptr = (Link_Location *) link_location.Next_Key ()) {
		Show_Progress ();

		if (loc_ptr->count == 0) continue;
		
		if (loc_ptr->link != link) {
			link = loc_ptr->link;
			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) {
				link = 0;
				continue;
			}
		}
		z = zone_list.Get_Index (loc_ptr->zone) - 1;
		if (z < 0) continue;

		factor = UnRound (link_ptr->Length ()) / loc_ptr->count; 

		//---- process each direction ----

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_ptr->Use () == -1) continue;
				perf_ptr = performance_data [link_ptr->BA_Dir ()];
			} else {
				if (link_ptr->Use () == -2) continue;
				perf_ptr = performance_data [link_ptr->AB_Dir ()];
			}
			if (perf_ptr != NULL) {
				num = perf_ptr->Periods ();

				for (j=1; j <= num; j++) {
					volume = perf_ptr->Volume (j);
					ttime = perf_ptr->TTime (j);

					if (volume > 0) {
						vmt = volume * factor;
						vht = volume * ttime / loc_ptr->count;

						zone_vmt [j-1] [z] += vmt;
						zone_vht [j-1] [z] += vht;
					}
				}
			}
		}
	}
	End_Progress ();
	return;

mem_error:
	Error ("Allocating Memory for %d Zones and %d Time Periods", nzone, num_inc);
}
