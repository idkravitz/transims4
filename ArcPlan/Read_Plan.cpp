//*********************************************************
//	Read_Plan.cpp - Read Each Plan File
//*********************************************************

#include "ArcPlan.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void ArcPlan::Read_Plan (void)
{
	int id, start_type, end_type, index1, index2, num, num_path, *path, mode, anode, last_random;
	int total_time, total_distance, last_traveler, last_trip, start_time, origin, destination;
	int org_field, start_field, des_field, ttime_field, distance_field, org_index, des_index;
	bool select, traveler_flag, location_flag, node_flag, link_flag, tod_flag, park_lot_flag, transit_flag;
	bool keep, routes_flag, random_select;

	XYZ_Point point;
	AB_Data *ab_ptr;
	Link_Data *link_ptr;
	Line_Data *line_ptr;
	Dir_Data *dir_ptr;
	Offset_Data *org_ptr, *des_ptr;

	static char *mode_name [] = {
		"Auto",
		"Transit",
		"Walk",
		"Bicycle",
		"Passenger",
		"Driver",
		"Magic Move"
	};

	if (access_flag) {
		org_field = arcview_access.Field_Number ("ORIGIN");
		start_field = arcview_access.Field_Number ("START");
		des_field = arcview_access.Field_Number ("LOCATION");
		ttime_field = arcview_access.Field_Number ("TTIME");
		distance_field = arcview_access.Field_Number ("DISTANCE");
		total_time = total_distance = start_time = origin = destination = 0;
		keep = false;
	}

	//---- set the filename and extension ----

	if (plan_file.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	//---- read each plan record ----

	traveler_flag = (travelers.Num_Ranges () > 0);
	tod_flag = (times.Num_Ranges () > 0);
	location_flag = (locations.Num_Ranges () > 0);
	park_lot_flag = (parking_lots.Num_Ranges () > 0);
	transit_flag = (transit_stops.Num_Ranges () > 0);
	routes_flag = (transit_routes.Num_Ranges () > 0);
	node_flag = (nodes.Num_Records () > 0);
	link_flag = (links.Num_Records () > 0);
	last_traveler = last_trip = org_index = des_index = last_random = 0;
	random_select = false;

	while (plan_file.Read ()) {
		Show_Progress ();

		//---- check for a new trip ----

		if (access_flag) {
			if (last_traveler != plan_file.Traveler () || last_trip != plan_file.Trip ()) {

				//---- save the previous trip ----

				if (keep && destination > 0) {
					arcview_access.Put_Field (org_field, origin);
					arcview_access.Put_Field (start_field, trip_time.Format_Time (start_time));
					arcview_access.Put_Field (des_field, destination);
					//arcview_access.Put_Field (ttime_field, trip_time.Format_Duration (total_time));
					arcview_access.Put_Field (ttime_field, total_time);
					arcview_access.Put_Field (distance_field, total_distance);

					des_ptr = location_offset.Get (destination);
					if (des_ptr == NULL) goto location_error;
					arcview_access.points.Reset ();

					point.x = des_ptr->X ();
					point.y = des_ptr->Y ();
					if (!arcview_access.points.Add (&point)) goto point_error;

					if (!arcview_access.Write_Record ()) {
						Error ("Writing ArcView Accessibility File");
					}
					num_access++;
				}

				//---- initialize a new trip ----

				last_traveler = plan_file.Traveler ();
				last_trip = plan_file.Trip ();
				total_time = total_distance = start_time = origin = destination = 0;
				keep = false;

				if (plan_file.Start_Type () == LOCATION_ID) {
					origin = plan_file.Start_ID ();
					start_time = plan_file.Time ();
					total_time = total_distance = plan_file.Duration ();
				}
			} else if (keep && plan_file.End_Type () == LOCATION_ID) {
				destination = plan_file.End_ID ();
				total_time += plan_file.Duration ();
				total_distance += plan_file.Duration ();
			}
		}

		//---- check for travel activities ----

		mode = plan_file.Mode ();
		if (mode == ACTIVITY_MODE) continue;

		//---- check the traveler selection criteria ----

		if (traveler_flag) {
			if (!travelers.In_Range (plan_file.Traveler ())) continue;
		}

		//---- check the time period criteria ----

		if (tod_flag) {
			if (!times.In_Range (plan_file.Time ())) continue;
		}

		//---- check the activity location criteria ----

		if (location_flag) {
			num = 0;
			if (plan_file.Start_Type () == LOCATION_ID) {
				if (locations.In_Range (plan_file.Start_ID ())) {
					num++;
				}
			}
			if (plan_file.End_Type () == LOCATION_ID) {
				if (locations.In_Range (plan_file.End_ID ())) {
					num++;
				}
			}
			if (!num) continue;
		}

		//---- check the parking lot criteria ----

		if (park_lot_flag) {
			num = 0;
			if (plan_file.Start_Type () == PARKING_ID) {
				if (parking_lots.In_Range (plan_file.Start_ID ())) {
					num++;
				}
			}
			if (plan_file.End_Type () == PARKING_ID) {
				if (parking_lots.In_Range (plan_file.End_ID ())) {
					num++;
				}
			}
			if (!num) continue;
		}

		//---- check for transit legs ----

		if (!stop_flag) {
			if (plan_file.Start_Type () == STOP_ID ||
				plan_file.End_Type () == STOP_ID) continue;
		}

		//---- check the transit stop criteria ----

		if (transit_flag) {
			num = 0;
			if (plan_file.Start_Type () == STOP_ID) {
				if (transit_stops.In_Range (plan_file.Start_ID ())) {
					num++;
				}
			}
			if (plan_file.End_Type () == STOP_ID) {
				if (transit_stops.In_Range (plan_file.End_ID ())) {
					num++;
				}
			}
			if (!num) continue;
		}

		//---- check the transit route criteria ----

		if (routes_flag) {
			if (plan_file.Start_Type () != STOP_ID || plan_file.End_Type () != STOP_ID) continue;
			if (!transit_routes.In_Range (plan_file.Route ())) continue;
		}

		//---- check the transit mode criteria ----

		if (mode_flag) {
			if (plan_file.Start_Type () != STOP_ID || plan_file.End_Type () != STOP_ID) continue;
			line_ptr = line_data.Get (plan_file.Route ());
			if (line_ptr == NULL) continue;
			if (!travel_modes [line_ptr->Mode ()]) continue;
		}

		//---- check the problem selection criteria ----

		if (problem_flag) {
			if (!problem_list.Get_Index (plan_file.Traveler ())) continue;
		}

		//---- check the household list ----

		if (hhlist_flag) {
			if (!hhold_list.Get_Index (plan_file.Household ())) continue;
		}

		//---- check the nodes groups ----

		if (node_flag) {
			select = false;
			Data_Range *range;

			for (range = (Data_Range *) nodes.First (); range != NULL && !select; range = (Data_Range *) nodes.Next ()) {

				path = plan_file.Path (&num_path);	

				num = range->Num_Ranges ();
				index1 = index2 = 0;

				while (num_path-- > 0) {
					id = *path++;
					if (type_flag) {
						index2 = range->In_Index (id);
					} else {
						if (id < 0) {
							link_ptr = link_data.Get (-id);
							index2 = range->In_Index (link_ptr->Anode ());
						} else {
							link_ptr = link_data.Get (id);
							index2 = range->In_Index (link_ptr->Bnode ());
						}
					}

					if (!index2) continue;

					if (index2 != index1 + 1) {
						index2 = 0;
						break;
					}
					if (index2 == num) {
						select = true;
						break;
					}
					index1 = index2;
				}
			}
			if (!select) continue;
		}

		//---- check the link groups ----

		if (link_flag) {
			select = false;
			Data_Range *range;

			for (range = (Data_Range *) links.First (); range != NULL && !select; range = (Data_Range *) links.Next ()) {

				path = plan_file.Path (&num_path);	
				anode = 0;

				num = range->Num_Ranges ();
				index1 = index2 = 0;

				while (num_path-- > 0) {
					id = *path++;
					if (type_flag) {
						index2 = 0;
						if (anode > 0) {
							ab_ptr = ab_key.Get (anode, id);
							if (ab_ptr != NULL) {
								dir_ptr = dir_data [ab_ptr->dir];
								index2 = range->In_Index (dir_ptr->Link ());
							}
						}
						anode = id;
					} else {
						index2 = range->In_Index (abs (id));
					}
					if (!index2) continue;

					if (index2 != index1 + 1) {
						index2 = 0;
						break;
					}
					if (index2 == num) {
						select = true;
						break;
					}
					index1 = index2;
				}
			}
			if (!select) continue;
		}

		//---- check the random selection ----

		if (random_flag) {
			if (last_random != plan_file.Traveler ()) {
				last_random = plan_file.Traveler ();
				random_select = (random.Probability () <= random_share);
			}
			if (!random_select) continue;
		}

		//---- redefine modes ----

		if (mode == AUTO_MODE) {
			if (plan_file.Driver_Flag () == 0) mode = 4;	//---- passenger ----
		} else if (mode == TRANSIT_MODE) {
			if (plan_file.Driver_Flag () == 1) mode = 5;	//---- driver ----
		}

		//---- store the data fields ----

		if (plan_flag) {
			arcview_plan.Put_Field (traveler_field, plan_file.Traveler ());
			arcview_plan.Put_Field (trip_field, plan_file.Trip ());
			arcview_plan.Put_Field (leg_field, plan_file.Leg ());
			arcview_plan.Put_Field (time_field, trip_time.Format_Time (plan_file.Time ()));
			arcview_plan.Put_Field (duration_field, trip_time.Format_Duration (plan_file.Duration ()));
			arcview_plan.Put_Field (cost_field, plan_file.Cost ());
			arcview_plan.Put_Field (impedance_field, plan_file.GCF ());
			arcview_plan.Put_Field (mode_field, mode_name [mode]);
			arcview_plan.Put_Field (mode_id_field, plan_file.Mode_ID ());
		}

		//---- get the origin offset ----

		id = plan_file.Start_ID ();
		start_type = plan_file.Start_Type ();

		if (start_type == LOCATION_ID) {
			org_ptr = location_offset.Get (id);
			if (org_ptr == NULL) goto location_error;
		} else if (start_type == PARKING_ID) {
			org_ptr = parking_offset.Get (id);
			if (org_ptr == NULL) goto parking_error;
			org_index = parking_offset.Record_Index ();
		} else if (start_type == STOP_ID) {
			org_ptr = stop_offset.Get (id);
			if (org_ptr == NULL) goto stop_error;
			org_index = stop_offset.Record_Index ();
		}

		//---- get the destination offset ----

		id = plan_file.End_ID ();
		end_type = plan_file.End_Type ();

		if (end_type == LOCATION_ID) {
			des_ptr = location_offset.Get (id);
			if (des_ptr == NULL) goto location_error;
		} else if (end_type == PARKING_ID) {
			des_ptr = parking_offset.Get (id);
			if (des_ptr == NULL) goto parking_error;
			des_index = parking_offset.Record_Index ();
		} else if (end_type == STOP_ID) {
			des_ptr = stop_offset.Get (id);
			if (des_ptr == NULL) goto stop_error;
			des_index = stop_offset.Record_Index ();
		}

		//---- write the path shape ----

		if (plan_flag) {

			//---- add the start point ----

			arcview_plan.points.Reset ();

			point.x = org_ptr->X ();
			point.y = org_ptr->Y ();
			if (!arcview_plan.points.Add (&point)) goto point_error;

			//---- add the path shape ----

			if (plan_file.Tokens () > 0) {
				if (start_type == STOP_ID && end_type == STOP_ID) {
					Write_Route (org_ptr, des_ptr);
				} else if (type_flag) {
					Write_Node_Path (org_ptr, des_ptr, (start_type == PARKING_ID && end_type == PARKING_ID));
				} else {
					Write_Link_Path (org_ptr, des_ptr, (start_type == PARKING_ID && end_type == PARKING_ID));
				}
			}

			//---- add the end point ----

			point.x = des_ptr->X ();
			point.y = des_ptr->Y ();
			if (!arcview_plan.points.Add (&point)) goto point_error;

			//---- write the shape record ----

			if (!arcview_plan.Write_Record ()) {
				Error ("Writing ArcView Plan File");
			}
			num_out++;
		}

		//---- summarize path data ----

		if (start_type == PARKING_ID && end_type == PARKING_ID) {
			if (width_flag) {
				Sum_Volume (org_ptr, des_ptr);
			}
			if (time_flag || distance_flag) {
				Write_Contour (org_ptr);
			}
			if (access_flag) {
				Sum_Access (org_ptr, des_ptr, &total_time, &total_distance);
				keep = true;
			}
			if (parking_flag) {
				parking_out [org_index]++;
				parking_in [des_index]++;
			}
		}

		//---- summarize transit data ----

		if (mode == TRANSIT_MODE && start_type == STOP_ID && end_type == STOP_ID) {
			if (rider_flag || on_off_flag) {
				board_data [org_index]++;
				alight_data [des_index]++;
			}
			if (rider_flag) {
				Sum_Riders (org_ptr, des_ptr);
			}
		}
		continue;

location_error:
		Warning ("Activity Location %d was Not Found", id);
		continue;

parking_error:
		Warning ("Parking Lot %d was Not Found", id);
		continue;

stop_error:
		Warning ("Transit Stop %d was Not Found", id);
		continue;
	}
	End_Progress ();

	plan_file.Close ();
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_plan.points.Num_Points ());
}
