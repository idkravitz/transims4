//*********************************************************
//	Read_ParkRide.cpp - Read the Park and Ride File
//*********************************************************

#include "TransitNet.hpp"

#include "Parking_File.hpp"
#include "Location_File.hpp"
#include "Access_File.hpp"

#define PARKRIDE_OFFSET		30.0

//---------------------------------------------------------
//	Read_ParkRide
//---------------------------------------------------------

void TransitNet::Read_ParkRide (void)
{
	int node_field, capacity_field, hourly_field, daily_field;
	int dir, node, capacity, hourly, daily, anode, bnode, min_offset, offset;
	double x, y, dx, dy, len;
	char *notes;

	Node_Path_Data *node_ptr;
	Dir_Path_Data *dir_ptr, *best_dir;
	Link_Data *link_ptr;
	Location_Data location_rec;
	
	Parking_File *parking_file = (Parking_File *) Network_Db_Base (NEW_PARKING);
	Location_File *location_file = (Location_File *) Network_Db_Base (NEW_ACTIVITY_LOCATION);
	Access_File *access_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);

	//---- read the park and ride file ----

	Show_Message ("Reading %s -- Record", parkride_file.File_Type ());
	Set_Progress ();
	Print (1);

	node_field = parkride_file.Required_Field ("NODE");
	capacity_field = parkride_file.Optional_Field ("CAPACITY", "PARKING", "SPACE", "SPACES");
	hourly_field = parkride_file.Optional_Field ("HOURLY", "RATE");
	daily_field = parkride_file.Optional_Field ("DAILY", "MAXIMUM");
	parkride_file.Notes_Field (parkride_file.Optional_Field ("NOTES"));

	min_offset = Round (PARKRIDE_OFFSET * 1.5);

	while (parkride_file.Read ()) {
		Show_Progress ();

		parkride_file.Get_Field (node_field, &node);
		if (node == 0) continue;

		node_ptr = node_data.Get (node);

		if (node_ptr == NULL) {
			Warning ("Park&Ride Node %d was Not Found in the Node File", node);
			parking_warnings++;
			continue;
		}

		parkride_file.Get_Field (capacity_field, &capacity);
		parkride_file.Get_Field (hourly_field, &hourly);
		parkride_file.Get_Field (daily_field, &daily);

		notes = parkride_file.Notes ();
		if (*notes == '\0') notes = "Park&Ride Lot";

		//---- select the best link ----

		offset = 0;
		best_dir = NULL;

		for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];

			if (dir_ptr->Offset () < min_offset) continue;

			if (best_dir != NULL) {
				if (best_dir->Stop_List () == 0 && dir_ptr->Stop_List () > 0) {
					offset = dir_ptr->Offset ();
					best_dir = dir_ptr;
					continue;
				}
			}
			if (dir_ptr->Offset () > offset) {
				offset = dir_ptr->Offset ();
				best_dir = dir_ptr;
			} else if (dir_ptr->Offset () == offset) {
				if (best_dir->Thru () < dir_ptr->Thru ()) {
					best_dir = dir_ptr;
				}
			}
		}

		if (best_dir == NULL) {
			Warning ("Park&Ride Node %d did not have Auto Access", node);
			parking_warnings++;
			continue;
		}
		nparkride++;

		//---- get the link ----

		link_ptr = link_data.Get (best_dir->Link ());

		if (best_dir->Dir () > 0) {
			anode = link_ptr->Bnode ();
			bnode = link_ptr->Anode ();
			offset = Round (PARKRIDE_OFFSET);
		} else {
			anode = link_ptr->Anode ();
			bnode = link_ptr->Bnode ();
			offset = link_ptr->Length () - Round (PARKRIDE_OFFSET);
		}

		//---- write the parking record ----

		parking_file->ID (++max_parking);
		parking_file->Link (best_dir->Link ());
		parking_file->Node (bnode);
		parking_file->Offset (PARKRIDE_OFFSET);
		parking_file->Type (Parking_Code (PARKRIDE));
		parking_file->Use ("CAR");
		parking_file->Space (capacity);
		parking_file->Start ("ALL");
		parking_file->End ("ALL");
		parking_file->Hourly (hourly);
		parking_file->Daily (daily);
		parking_file->Notes (notes);

		if (!parking_file->Write ()) {
			Error ("Writing %s", parking_file->File_Type ());
		}
		nparking++;

		//---- get the activity location ----

		node_ptr = node_data.Get (bnode);

		x = UnRound (node_ptr->X ());
		y = UnRound (node_ptr->Y ());

		node_ptr = node_data.Get (anode);

		dx = UnRound (node_ptr->X ()) - x;
		dy = UnRound (node_ptr->Y ()) - y;

		len = UnRound (link_ptr->Length ());
		if (len == 0.0) len = 1.0;

		x += (dx * PARKRIDE_OFFSET + dy * 15.0) / len;
		y += (dy * PARKRIDE_OFFSET - dx * 15.0) / len;
		
		//---- add activity locations ----

		location_file->Location (++max_location);
		location_file->Link (best_dir->Link ());
		location_file->Node (bnode);
		location_file->Offset (PARKRIDE_OFFSET);
		location_file->X (x);
		location_file->Y (y);
		location_file->Zone (0);
		location_file->Notes (notes);

		if (!location_file->Write ()) {
			Error ("Writing %s", location_file->File_Type ());
		}
		nlocation++;

		//---- add to location data ----

		location_rec.ID (max_location);
		location_rec.Link_Dir (best_dir->Link_Dir ());
		location_rec.Offset (offset);
		location_rec.X (Round (x));
		location_rec.Y (Round (y));
		location_rec.Zone (0);

		if (!location_data.Add (&location_rec)) {
			Error ("Adding Location Data");
		}

		//---- add process link ----

		access_file->ID (++max_access);
		access_file->From_ID (max_location);
		access_file->From_Type (End_Code (LOCATION_ID));
		access_file->To_ID (max_parking);
		access_file->To_Type (End_Code (PARKING_ID));
		access_file->Time (access_time);
		access_file->Cost (0.0);
		access_file->Notes ("ParkRide Access");

		if (!access_file->Write ()) goto access_error;
		naccess++;

		access_file->ID (++max_access);
		access_file->From_ID (max_parking);
		access_file->From_Type (End_Code (PARKING_ID));
		access_file->To_ID (max_location);
		access_file->To_Type (End_Code (LOCATION_ID));
		access_file->Time (access_time);
		access_file->Cost (0.0);
		access_file->Notes ("ParkRide Egress");

		if (!access_file->Write ()) goto access_error;
		naccess++;
	}
	End_Progress ();

	Print (1, "Number of Park and Ride Records = %d", nparkride);

	parking_file->Close ();
	return;

access_error:
	Error ("Writing %s", access_file->File_Type ());
}
