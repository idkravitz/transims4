//*********************************************************
//	Write_Lane_Use.cpp - process lane use records
//*********************************************************

#include "TransimsNet.hpp"

#include "Lane_Use_File.hpp"

//---------------------------------------------------------
//	Write_Lane_Use
//---------------------------------------------------------

void TransimsNet::Write_Lane_Use (Link_Data *link_ptr)
{
	int i, dir, id, lanes, tot_lanes, last_thru, num, max_len, area_type, zone;
	double length;
	char *start, *end;

	//---- get the pocket length ----

	Lane_Use_File *lane_use_file = (Lane_Use_File *) Network_Db_File (NEW_LANE_USE);

	Link_Detail_Data *detail_ptr;
	Dir_Connect *dir_ptr;
	Node_Data *node_ptr;
	Zone_Data *zone_ptr;

	for (dir=0; dir < 2; dir++) {
		if (dir == 0) {
			id = link_ptr->AB_Dir ();
		} else {
			id = link_ptr->BA_Dir ();
		}
		if (id <= 0) continue;

		dir_ptr = dir_data [id];
		if (dir_ptr == NULL) continue;

		detail_ptr = detail_data.Get (dir_ptr->Link_Dir ());
		if (detail_ptr == NULL) continue;

		lanes = detail_ptr->Lanes ();
		if (lanes == 0) continue;
		if (detail_ptr->Period () == NULL) continue;

		//---- get the area type ----

		if (zone_flag) {
			node_ptr = node_data.Get (link_ptr->Anode ());

			zone = Closest_Zone (node_ptr->X (), node_ptr->Y ());

			zone_ptr = zone_data.Get (zone);

			if (zone_ptr == NULL) {
				Error ("Zone %d was Not Found in the Zone file", zone);
			}
			area_type = zone_ptr->Area_Type ();
		} else {
			area_type = 2;
		}
		max_len = pocket_len [dir_ptr->Type ()].Best (area_type);

		last_thru = dir_ptr->Left () + dir_ptr->Thru ();
		tot_lanes = last_thru + dir_ptr->Right ();

		num = detail_ptr->Period ()->Num_Ranges ();

		for (i=1; i <= num; i++) {
			lane_use_file->Link (link_ptr->Link ());
			lane_use_file->Dir (dir);

			if (lanes == tot_lanes) {
				lane_use_file->Lane (0);
			} else if (lanes <= 2) {
				lane_use_file->Lane (last_thru);
			}
			if (detail_ptr->Use () == 0) {
				lane_use_file->Use ("ANY");
				lane_use_file->Type ("N");
			} else {
				lane_use_file->Use (Use_Code (detail_ptr->Use ()));
				lane_use_file->Type ("O");
			}
			start = detail_ptr->Period ()->Range_Format (i);

			end = strstr (start, "..");

			*end = '\0';
			while (*(++end) == '.');

			lane_use_file->Start (start);
			lane_use_file->End (end);
			lane_use_file->Offset (0);

			if (dir_ptr->Right () > 0) {
				length = -1.0;
				if (dir_ptr->Right_Turn () > 0) {
					length = link_ptr->Length () - max_len;
					if (length == 0.0) length = -1.0;
				}
			} else {
				length = 0.0;
			}
			if (length >= 0.0) {
				lane_use_file->Length (UnRound ((int) length));

				nlane_use++;
				if (!lane_use_file->Write ()) {
					Error ("Writing Lane Use Record %d", nlane_use);
				}
			}
			if (lanes == 2) {
				lane_use_file->Lane (1 + dir_ptr->Left ());
				length = -1.0;

				if (dir_ptr->Left_Turn () > 0) {
					length = link_ptr->Length () - max_len;
					if (length == 0.0) length = -1.0;
				}
				if (length > 0.0) {
					lane_use_file->Length (UnRound ((int) length));

					nlane_use++;
					if (!lane_use_file->Write ()) {
						Error ("Writing Lane Use Record %d", nlane_use);
					}
				}
			}
		}
	}
} 
