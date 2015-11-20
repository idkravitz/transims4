//*********************************************************
//	Write_Stop.cpp - Write Transit Stop Files
//*********************************************************

#include "TransitNet.hpp"

#include "Stop_File.hpp"
#include "Location_File.hpp"
#include "Access_File.hpp"

#define MAX_ACT_OFFSET		10.0

//---------------------------------------------------------
//	Write_Stop
//---------------------------------------------------------

void TransitNet::Write_Stop (void)
{
	int anode, bnode, node, dir, link_dir, link_off, file_off, best_act, max_diff, dir_off, setback, zone;
	double x, y, dx, dy, len, offset;

	Stop_Data *stop_ptr;
	Link_Data *link_ptr;
	Node_Path_Data *node_ptr;
	Dir_Path_Data *dir_ptr;
	Location_Data *location_ptr;
	
	Stop_File *stop_file = (Stop_File *) Network_Db_Base (NEW_TRANSIT_STOP);
	Location_File *location_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);
	Access_File *access_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);

	Show_Message ("Writing %s - Record", stop_file->File_Type ());
	Set_Progress ();

	max_diff = Round (MAX_ACT_OFFSET);
	dir_off = stop_offset * 3 / 2;

	location_file->Reset_Record ();

	for (stop_ptr = stop_data.First_Key (); stop_ptr; stop_ptr = stop_data.Next_Key ()) {
		Show_Progress ();

		if (stop_ptr->ID () <= new_stop || stop_ptr->Space () == 0) continue;

		//---- get the link ----

		link_ptr = link_data.Get (stop_ptr->Link ());

		if (stop_ptr->Dir () > 0) {
			anode = link_ptr->Bnode ();
			bnode = link_ptr->Anode ();
			link_off = link_ptr->Length () - stop_ptr->Offset ();
			if (link_off <= 0) {
				link_off = 1;
			}
			file_off = link_off;
		} else {
			anode = link_ptr->Anode ();
			bnode = link_ptr->Bnode ();
			link_off = stop_ptr->Offset ();
			file_off = link_ptr->Length () - link_off;
			if (file_off <= 0) {
				file_off = 1;
			}
		}
		nstop++;
		stop_file->ID (stop_ptr->Stop ());

		if (stop_file->LinkDir_Type () == LINK_NODE) {
			stop_file->Link (stop_ptr->Link ());
			stop_file->Dir (bnode);
		} else if (stop_file->LinkDir_Type () == LINK_DIR) {
			stop_file->Link (stop_ptr->Link ());
			stop_file->Dir (stop_ptr->Dir ());
		} else if (stop_ptr->Dir () > 0) {
			stop_file->Link (-(stop_ptr->Link ()));
		} else {
			stop_file->Link (stop_ptr->Link ());
		}
		stop_file->Offset (UnRound (file_off));
		stop_file->Use (Use_Code (stop_ptr->Use ()));
		stop_file->Space (2);
		stop_file->Name ("");
		zone = 0;

		if (link_ptr->Type () == EXTERNAL) {
			stop_file->Type (Stop_Code (EXTLOAD));
			stop_file->Notes ("Transit External");
			zone = MIN (link_ptr->Anode (), link_ptr->Bnode ());
		} else if (Use_Permission (stop_ptr->Use (), RAIL)) {
			stop_file->Type (Stop_Code (STATION));
			stop_file->Notes ("Transit Station");
		} else {
			stop_file->Type (Stop_Code ((Stop_Type) stop_ptr->Type ()));
			stop_file->Notes ("Transit Stop");
		}
		if (!stop_file->Write ()) {
			Error ("Writing %s", stop_file->File_Type ());
		}

		//---- identify the activity location ----

		best_act = 0;
		link_dir = stop_ptr->Link_Dir ();

		if (!Use_Permission (link_ptr->Use (), WALK) && link_ptr->Type () != EXTERNAL) {

			//---- search for a cross street ----

			node = 0;
			setback = MAX (link_ptr->Aoffset (), link_ptr->Boffset ());

			if (file_off > dir_off + setback) {
				if ((link_ptr->Length () - file_off) < dir_off + setback) {
					node = anode;
				}
			} else {
				node = bnode;
			}

			//---- search for a walk link leaving the node ----

			if (node > 0) {
				node_ptr = node_data.Get (node);


				for (dir = node_ptr->From_Dir (); dir; dir = dir_ptr->From_List ()) {
					dir_ptr = dir_data [dir];

					if (Use_Permission (dir_ptr->Use (), WALK)) {
						anode = node;
						node_ptr = node_data [dir_ptr->Bnode ()];
						bnode = node_ptr->Node ();
						link_dir = dir_ptr->Link_Dir ();

						if (dir_ptr->Dir () > 0) {
							file_off = dir_ptr->Length () - stop_offset - dir_ptr->Aoffset ();
							if (file_off <= 0) {
								file_off = 1;
							}
							link_off = file_off;
						} else {
							link_off = stop_offset + dir_ptr->Aoffset ();
							if (link_off >= dir_ptr->Length ()) {
								link_off = dir_ptr->Length () - 1;
							}
							file_off = dir_ptr->Length () - link_off;
						}
						link_ptr = link_data.Get (dir_ptr->Link ());
					}
				}
			}
		}

		//---- search for an existing activity location -----

		for (location_ptr = location_data.First (); location_ptr; location_ptr = location_data.Next ()) {
			if (link_dir == location_ptr->Link_Dir ()) {
				if (abs (location_ptr->Offset () - link_off) <= max_diff) {
					best_act = location_ptr->ID ();
					break;
				}
			}
		}
	
		//---- add activity locations ----

		if (!best_act) {
			best_act = ++max_location;
			nlocation++;

			//---- activity location XY ----

			node_ptr = node_data.Get (bnode);

			x = UnRound (node_ptr->X ());
			y = UnRound (node_ptr->Y ());

			node_ptr = node_data.Get (anode);

			dx = UnRound (node_ptr->X ()) - x;
			dy = UnRound (node_ptr->Y ()) - y;

			len = UnRound (link_ptr->Length ());
			if (len == 0.0) len = 1.0;

			offset = UnRound (file_off);

			x += (dx * offset + dy * 15.0) / len;
			y += (dy * offset - dx * 15.0) / len;

			//---- save the activity location ----

			location_file->Location (best_act);
			location_file->Link (link_ptr->Link ());
			location_file->Node (bnode);
			location_file->Offset (UnRound (file_off));
			location_file->X (x);
			location_file->Y (y);
			location_file->Zone (zone);

			if (link_ptr->Type () == EXTERNAL) {
				location_file->Notes ("Transit External");
			} else {
				location_file->Notes ("Transit Stop");

				if (!Use_Permission (link_ptr->Use (), WALK)) {
					Warning ("Activity Location %d does Not Permit Walking", best_act);
				}
			}
			if (!location_file->Write ()) {
				Error ("Writing %s", location_file->File_Type ());
			}
		}

		//---- add process link ----

		naccess++;
		access_file->ID (++max_access);
		access_file->From_ID (best_act);
		access_file->From_Type (End_Code (LOCATION_ID));
		access_file->To_ID (stop_ptr->Stop ());
		access_file->To_Type (End_Code (STOP_ID));
		access_file->Time (access_time);
		access_file->Cost (0.0);
		access_file->Notes ("Transit Access");

		if (!access_file->Write ()) goto access_error;

		naccess++;
		access_file->ID (++max_access);
		access_file->From_ID (stop_ptr->Stop ());
		access_file->From_Type (End_Code (STOP_ID));
		access_file->To_ID (best_act);
		access_file->To_Type (End_Code (LOCATION_ID));
		access_file->Time (access_time);
		access_file->Cost (0.0);
		access_file->Notes ("Transit Egress");

		if (!access_file->Write ()) goto access_error;
	}
	End_Progress ();
	return;

access_error:
	Error ("Writing %s", access_file->File_Type ());
}
