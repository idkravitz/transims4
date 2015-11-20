//*********************************************************
//	Write_Boundary.cpp - Create Boundary Access Points
//*********************************************************

#include "SubareaPlans.hpp"

#include "Boundary_File.hpp"
#include "Utility.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Boundary
//---------------------------------------------------------

void SubareaPlans::Write_Boundary (void)
{
	bool ab_flag, ba_flag;
	int i, node, anode, bnode, period, nperiod, start, end, ab_dir, ba_dir;
	double x, y, length, len, xy_len, dx, dy, x_offset, y_offset, xa, ya, xb, yb;
	double factor, speed;
	char string [STRING_BUFFER];

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Boundary *boundary_ptr;
	Boundary_File *boundary_file;
	Driver_Data *driver_ptr;
	Dir_Data *dir_ptr;
	Stop_Data stop_rec;

	char *ext_dir [] = {
		"Origin", "Destination"
	};

	//---- flag links on transit routes ----

	if (transit_flag) {
		max_stop = ((max_stop + 1000) / 1000) * 1000;

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

	if (location_flag) {
		for (i=1; i <= location_file->Num_Fields (); i++) {
			location_file->Put_Field (i, 0);
		}
	}

	if (boundary_flag) {
		boundary_file = (Boundary_File *) Demand_Db_Base (NEW_BOUNDARY_SPEED);

		if (delay_flag) {
			nperiod = ttime_data.Periods ();
		} else {
			nperiod = 1;
		}
	}

	//---- process each boundary link ----
	
	Show_Message ("Writing Boundary Link Data -- Record");
	Set_Progress (1000);

	for (boundary_ptr = (Boundary *) boundary.First (); boundary_ptr; boundary_ptr = (Boundary *) boundary.Next ()) {
		Show_Progress ();

		link_ptr = link_data.Get (boundary_ptr->link);

		if (link_ptr == NULL) {
			Error ("Boundary Link %d was Not Found in the Link File", boundary_ptr->link);
		}

		//---- set the link orientation ----

		if (boundary_ptr->type == 1) {
			anode = link_ptr->Bnode ();
			bnode = link_ptr->Anode ();
			ab_dir = link_ptr->BA_Dir ();
			ba_dir = link_ptr->AB_Dir ();
		} else {
			anode = link_ptr->Anode ();
			bnode = link_ptr->Bnode ();
			ab_dir = link_ptr->AB_Dir ();
			ba_dir = link_ptr->BA_Dir ();
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
			max_location++;

			if (i) {
				boundary_ptr->out_location = max_location;
			} else {
				boundary_ptr->in_location = max_location;
			}

			if (location_flag) {
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
					Error ("Writing Activity Location Record %d", max_location);
				}
			}

			//---- add parking lot ----

			nparking++;
			max_parking++;

			if (i) {
				boundary_ptr->out_parking = max_parking;
			} else {
				boundary_ptr->in_parking = max_parking;
			}

			if (parking_flag) {
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
					Error ("Writing Parking Record %d", max_parking);
				}
			}

			//---- write boundary parking speeds ----

			if (boundary_flag) {

				boundary_file->Parking (max_parking);

				//---- get the directional link ----

				if (i > 0) {
					ttime_ptr = ttime_data [ba_dir];
				} else {
					ttime_ptr = ttime_data [ab_dir];
				}
				if (ttime_ptr == NULL) {
					Error ("Link Direction was Not Found");
				}

				//---- get the travel time ----

				if (delay_flag && ttime_ptr->Periods () > 0) {
					for (start=0, period=1; period <= nperiod; period++, start = end) {
						end = start + Time_Increment ();

						speed = UnRound (ttime_ptr->TTime (period));
						if (speed == 0.0) {
							speed = UnRound (ttime_ptr->Time0 ());
						} else if (speed < 0.0) {
							speed = MIDNIGHT;
						}
						if (speed > 0.0) {
							speed = length / speed;
							if (speed < 0.5) speed = 0.5;
						}
						boundary_file->Start_Time (time_step.Format_Time (start));
						boundary_file->End_Time (time_step.Format_Time (end));
						boundary_file->Speed (speed);

						if (!boundary_file->Write ()) goto boundary_error;
					}
				} else {
					speed = UnRound (ttime_ptr->Time0 ());
					if (speed > 0.0) {
						speed = length / speed;
						if (speed < 0.5) speed = 0.5;
					}
					start = 0;
					end = MIDNIGHT;

					boundary_file->Start_Time (time_step.Format_Time (start));
					boundary_file->End_Time (time_step.Format_Time (end));
					boundary_file->Speed (speed);

					if (!boundary_file->Write ()) goto boundary_error;
				}
			}

			//---- add process link ----

			naccess++;
			max_access++;

			if (access_flag) {
				access_file->ID (max_access);
				access_file->From_ID (max_location);
				access_file->From_Type ("ACTIVITY");
				access_file->To_ID (max_parking);
				access_file->To_Type ("PARKING");
				access_file->Time (1.0);
				access_file->Cost (0.0);
				access_file->Put_Field ("NOTES", string);
				
				if (!access_file->Write ()) {
					Error ("Writing Process Link Record %d", max_access);
				}
			}

			naccess++;
			max_access++;

			if (access_flag) {
				access_file->ID (max_access);
				access_file->From_ID (max_parking);
				access_file->From_Type ("PARKING");
				access_file->To_ID (max_location);
				access_file->To_Type ("ACTIVITY");
				access_file->Time (1.0);
				access_file->Cost (0.0);
				access_file->Put_Field ("NOTES", string);

				if (!access_file->Write ()) {
					Error ("Writing Process Link Record %d", max_access);
				}
			}

			//---- add the transit external ----

			if (transit_flag) {
				if (dir_ptr->Stop_List () == 1) {

					//---- add transit stop ----

					nstop++;
					new_stop++;
					max_stop++;

					dir_ptr->Stop_List (max_stop);

					if (i) {
						boundary_ptr->out_stop = max_stop;
					} else {
						boundary_ptr->in_stop = max_stop;
					}
					if (output_flag) {
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
					max_access++;

					if (access_flag) {
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
					}

					naccess++;
					max_access++;

					if (access_flag) {
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
	}
	End_Progress ();

	if (location_flag) location_file->Close ();
	if (parking_flag) parking_file->Close ();
	if (access_flag) access_file->Close ();
	if (boundary_flag) boundary_file->Close ();

	return;

boundary_error:
	Error ("Writing Boundary Speed Record");
}
