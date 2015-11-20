//*********************************************************
//	Write_Boundary.cpp - Create Boundary Access Points
//*********************************************************

#include "SubareaNet.hpp"

#include "Location_File.hpp"
#include "Parking_File.hpp"
#include "Access_File.hpp"
#include "Stop_File.hpp"
#include "Utility.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Boundary
//---------------------------------------------------------

void SubareaNet::Write_Boundary (void)
{
	bool ab_flag, ba_flag;
	int i, node, anode, bnode, ab_dir, ba_dir;
	double x, y, length, len, xy_len, dx, dy, x_offset, y_offset, xa, ya, xb, yb;
	double factor;
	char string [STRING_BUFFER];

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Driver_Data *driver_ptr;
	Dir_Data *dir_ptr;
	Stop_Data stop_rec;

	Location_File *location_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);
	Parking_File *parking_file = (Parking_File *) Network_Db_Base (NEW_PARKING);
	Access_File *access_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);
	Stop_File *stop_file;

	char *ext_dir [] = {
		"Origin", "Destination"
	};

	//---- flag links on transit routes ----

	if (transit_flag) {
		max_stop = ((max_stop + 1000) / 1000) * 1000;
		stop_file = (Stop_File *) Network_Db_Base (NEW_TRANSIT_STOP);

		for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {
			dir_ptr->Stop_List (0);
		}
		for (driver_ptr = driver_data.First (); driver_ptr; driver_ptr = driver_data.Next ()) {
			for (i=1; i <= driver_ptr->Links (); i++) {
				dir_ptr = dir_data.Get (driver_ptr->Link_Dir (i));

				if (dir_ptr != NULL) {
					dir_ptr->Stop_List (1);
				}
			}
		}
	}

	//---- round up the ID codes ----

	max_location = ((max_location + 1000) / 1000) * 1000;
	max_parking = ((max_parking + 1000) / 1000) * 1000;
	max_access = ((max_access + 1000) / 1000) * 1000;
	max_zone = ((max_zone + 10) / 10) * 10;
	if (max_zone > 0) max_zone--;

	//---- initialize the data fields ----

	for (i=1; i <= location_file->Num_Fields (); i++) {
		location_file->Put_Field (i, 0);
	}

	//---- process each boundary link ----

	Show_Message ("Writing Boundary Link Data -- Record");
	Set_Progress (1000);

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();

		//---- set the link orientation ----

		if (link_ptr->Type () == 1) {
			anode = link_ptr->Bnode ();
			bnode = link_ptr->Anode ();
			ab_dir = link_ptr->BA_Dir ();
			ba_dir = link_ptr->AB_Dir ();
		} else if (link_ptr->Type () == 2) {
			anode = link_ptr->Anode ();
			bnode = link_ptr->Bnode ();
			ab_dir = link_ptr->AB_Dir ();
			ba_dir = link_ptr->BA_Dir ();
		} else {
			continue;
		}
		ab_flag = (ab_dir > 0);
		ba_flag = (ba_dir > 0);

		//---- get the coordinates ----

		node_ptr = node_data.Get (anode);

		xa = node_ptr->X ();
		ya = node_ptr->Y ();

		node_ptr = node_data.Get (bnode);

		xb = node_ptr->X ();
		yb = node_ptr->Y ();

		//---- check the link length ----

		dx = xb - xa;
		dy = yb - ya;

		xy_len = sqrt (dx * dx + dy * dy);

		if (xy_len < 4 * external_offset) {
			Warning ("Link %d from %d to %d is Too Short for Boundary Access ", 
				link_ptr->Link (), link_ptr->Anode (), link_ptr->Bnode ());

			if (xy_len < 2 * external_offset) {
				nshort++;
				continue;
			}
		}

		length = UnRound (link_ptr->Length ());

		//--- set the access point ----

		factor = external_offset / xy_len;

		len = UnRound (Round (length * (1.0 - factor)));

		x = xa + dx * factor;
		y = ya + dy * factor;
		
		x_offset = external_offset / xy_len * dy;			
		y_offset = -external_offset / xy_len * dx;
		node = bnode;
		max_zone++;

		//--- build activity connections in each direction ----
		
		for (i=0; i < 2; i++) {
			if (i) {
				node = anode;
				x_offset = -x_offset;
				y_offset = -y_offset;
				len = length - len;
				if (!ba_flag) continue;
				if (transit_flag) {
					dir_ptr = dir_data [ba_dir];
				}
			} else {
				if (!ab_flag) continue;
				if (transit_flag) {
					dir_ptr = dir_data [ab_dir];
				}
			}
			xb = x + x_offset;
			yb = y + y_offset;

			str_fmt (string, sizeof (string), "Subarea %s", ext_dir [i]);

			//---- add activity locations ----

			nlocation++;
			new_location++;
			max_location++;

			location_file->Location (max_location);
			location_file->Node (node);
			location_file->Link (link_ptr->Link ());
			location_file->Offset (len);
			location_file->Put_Field ("LAYER", "AUTO");
			location_file->X (UnRound ((int) (xb + 0.5)));
			location_file->Y (UnRound ((int) (yb + 0.5)));
			location_file->Z (0.0);
			location_file->Put_Field ("NOTES", string);
			location_file->Zone (max_zone);

			if (!location_file->Write ()) {
				Error ("Writing %s", location_file->File_Type ());
			}

			//---- add parking lot ----

			nparking++;
			new_parking++;
			max_parking++;

			parking_file->ID (max_parking);
			parking_file->Node (node);
			parking_file->Link (link_ptr->Link ());
			parking_file->Offset (len);
			parking_file->Type ("BNDRY");
			parking_file->Space (0);
			parking_file->Put_Field ("GENERIC", "T");
			parking_file->Put_Field ("VEHICLE", "ANY");
			parking_file->Start ("ALL");
			parking_file->End ("ALL");
			parking_file->Put_Field ("NOTES", string);

			if (!parking_file->Write ()) {
				Error ("Writing %s", parking_file->File_Type ());
			}

			//---- add process link ----

			naccess++;
			new_access++;
			max_access++;

			access_file->ID (max_access);
			access_file->From_ID (max_location);
			access_file->From_Type ("ACTIVITY");
			access_file->To_ID (max_parking);
			access_file->To_Type ("PARKING");
			access_file->Time (1.0);
			access_file->Cost (0.0);
			access_file->Put_Field ("NOTES", string);
			
			if (!access_file->Write ()) {
				Error ("Writing %s", access_file->File_Type ());
			}

			naccess++;
			new_access++;
			max_access++;

			access_file->ID (max_access);
			access_file->From_ID (max_parking);
			access_file->From_Type ("PARKING");
			access_file->To_ID (max_location);
			access_file->To_Type ("ACTIVITY");
			access_file->Time (1.0);
			access_file->Cost (0.0);
			access_file->Put_Field ("NOTES", string);

			if (!access_file->Write ()) {
				Error ("Writing %s", access_file->File_Type ());
			}

			//---- add the transit external ----

			if (transit_flag) {
				if (dir_ptr->Stop_List () == 1) {

					//---- add transit stop ----

					nstop++;
					new_stop++;
					max_stop++;

					dir_ptr->Stop_List (max_stop);

					stop_file->ID (max_stop);
					stop_file->Name ("EXTERNAL");

					if (stop_file->LinkDir_Type () == LINK_SIGN) {
						if (dir_ptr->Dir () == 0) {
							stop_file->Link (dir_ptr->Link ());
						} else {
							stop_file->Link (-dir_ptr->Link ());
						}
					} else if (stop_file->LinkDir_Type () == LINK_DIR) {
						stop_file->Link (dir_ptr->Link ());
						stop_file->Dir (dir_ptr->Dir ());
					} else {
						stop_file->Link (dir_ptr->Link ());
						stop_file->Dir (node);
					}
					stop_file->Offset (len);
					stop_file->Type ("EXTERNAL");
					stop_file->Use ("ANY");
					stop_file->Space (0);
					stop_file->Notes (string);

					if (!stop_file->Write ()) {
						Error ("Writing %s", stop_file->File_Type ());
					}

					//---- add stop data ----

					stop_rec.Stop (max_stop);
					stop_rec.Link_Dir (dir_ptr->Link_Dir ());
					if (dir_ptr->Dir () == 0) {
						stop_rec.Offset (Round (length - len));
					} else {
						stop_rec.Offset (Round (len));
					}
					if (!stop_data.Add (&stop_rec)) {
						Error ("Adding Stop Data");
					}

					//---- add process link ----

					naccess++;
					new_access++;
					max_access++;

					access_file->ID (max_access);
					access_file->From_ID (max_location);
					access_file->From_Type ("ACTIVITY");
					access_file->To_ID (max_stop);
					access_file->To_Type ("TRANSIT");
					access_file->Time (1.0);
					access_file->Cost (0.0);
					access_file->Put_Field ("NOTES", string);
					
					if (!access_file->Write ()) {
						Error ("Writing %s", access_file->File_Type ());
					}

					naccess++;
					new_access++;
					max_access++;

					access_file->ID (max_access);
					access_file->From_ID (max_stop);
					access_file->From_Type ("TRANSIT");
					access_file->To_ID (max_location);
					access_file->To_Type ("ACTIVITY");
					access_file->Time (1.0);
					access_file->Cost (0.0);
					access_file->Put_Field ("NOTES", string);

					if (!access_file->Write ()) {
						Error ("Writing %s", access_file->File_Type ());
					}
				}
			}
		}
	}
	End_Progress ();
}
