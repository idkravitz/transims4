//*********************************************************
//	Driver_Plan.cpp - convert the driver plan file
//*********************************************************

#include "NewFormat.hpp"

#include "Driver_File.hpp"

//---------------------------------------------------------
//	Driver_Plan 
//---------------------------------------------------------

void NewFormat::Driver_Plan (void)
{
	int i, nroute, nrec, route, last_route, anode, bnode, link, dir, num, parking, *node;
	AB_Data *ab_ptr;
	bool parking_flag;

	Driver_File *driver_file;
	Parking_Data *parking_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	driver_file = (Driver_File *) Network_Db_Base (NEW_TRANSIT_DRIVER);

	parking_flag = (parking_data.Num_Records () > 0);

	nroute = nrec = 0;
	route = last_route = 0;

	//---- read each record ----

	Show_Message ("Reading %s -- Record", driver_plan.File_Type ());
	Set_Progress ();

	while (driver_plan.Read ()) {
		Show_Progress ();

		//---- must start and end at a parking lot ----

		if (driver_plan.Start_Type () != PARKING_ID && driver_plan.End_Type () != PARKING_ID) continue;

		route = driver_plan.Route ();

		if (route == last_route) continue;
		last_route = route;

		//---- save the route header ----

		driver_file->Route (route);

		node = driver_plan.Path (&num);

		if (parking_flag) {
			driver_file->Links (num+1);
		} else {
			driver_file->Links (num-1);
		}
		driver_file->Veh_Type (5);		//----- assume bus ----
		driver_file->Sub_Type (0);

		if (!driver_file->Write (false)) goto write_error;
		nroute++;
		nrec++;

		//---- write each link ----

		if (parking_flag) {
			parking = driver_plan.Start_ID ();
			parking_ptr = parking_data.Get (parking);

			link_ptr = link_data.Get (parking_ptr->Link ());

			if (*node == link_ptr->Bnode ()) {
				anode = link_ptr->Anode ();
			} else if (*node == link_ptr->Anode ()) {
				anode = link_ptr->Bnode ();
			} else {
				Error ("Parking Link %d and Route %d Node %d are incompatible", link_ptr->Link (), route, *node);
			}
		} else {
			anode = 0;
		}

		for (i=0; i <= num; i++, node++) {
			if (i == num) {
				if (!parking_flag) break;

				parking = driver_plan.End_ID ();
				parking_ptr = parking_data.Get (parking);

				link_ptr = link_data.Get (parking_ptr->Link ());

				if (anode == link_ptr->Anode ()) {
					bnode = link_ptr->Bnode ();
				} else if (anode == link_ptr->Bnode ()) {
					bnode = link_ptr->Anode ();
				} else {
					Error ("Parking Link %d and Route %d Node %d are incompatible", link_ptr->Link (), route, anode);
				}
			} else {
				bnode = *node;
			}
			if (anode > 0) {
				ab_ptr = ab_key.Get (anode, bnode);

				if (ab_ptr == NULL) {
					Error ("A Link between Route %d Nodes %d and %d was Not Found", route, anode, bnode);
				}
				dir_ptr = dir_data [ab_ptr->dir];

				link = dir_ptr->Link ();
				dir = dir_ptr->Dir ();

				if (driver_file->LinkDir_Type () == LINK_SIGN) {
					if (dir == 1) link = -link;
				} else if (driver_file->LinkDir_Type () == LINK_NODE) {
					link_ptr = link_data.Get (link);
					if (dir == 1) {
						dir = link_ptr->Anode ();
					} else {
						dir = link_ptr->Bnode ();
					}
				}
				driver_file->Link (link);
				driver_file->Dir (dir);

				if (!driver_file->Write (true)) goto write_error;
				nrec++;
			}
			anode = bnode;
		}
	}
	End_Progress ();
	driver_plan.Close ();

	Print (2, "Number of %s Records = %d", driver_plan.File_Type (), Progress_Count ());

	driver_file->Close ();

	Write (2, "Number of %s Routes = %d", driver_file->File_Type (), nroute);
	Write (1, "Number of %s Records = %d", driver_file->File_Type (), nrec);
	
	return;

write_error:
	Error ("Writing Driver File");
}
