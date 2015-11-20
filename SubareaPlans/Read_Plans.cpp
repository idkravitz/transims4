//*********************************************************
//	Read_Plans.cpp - Read Each Plan File
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void SubareaPlans::Read_Plans (void)
{
	int i, j, p, id, n, n1, n2, num, link, time, num_path, *path, *subpath, num_sub, ttime;
	int location, parking, dir, traveler, trip, veh_id, last_trip;
	int in_link_dir;
	bool in_subarea, n2_subarea, org_in, des_in, save_flag;
	double offset;

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	AB_Data *ab_ptr;
	Parking_Data *park_ptr;
	Location_Data *loc_ptr;
	Plan_Data *plan_ptr;
	Vehicle_Data *veh_ptr;
	Boundary *boundary_ptr;
	Connect_Time *connect_ptr;

	//---- set the filename and extension ----

	if (input_plans.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", input_plans.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	//---- read each plan record ----

	in_subarea = n2_subarea = org_in = des_in = save_flag = false;
	current_traveler = current_trip = last_trip = last_location = last_time = 0;

	while (input_plans.Read ()) {
		Show_Progress ();

		plan_ptr = input_plans.Plan ();
		traveler = input_plans.Traveler ();
		trip = input_plans.Trip ();

		if (traveler != current_traveler || trip != last_trip) {
			if (traveler != current_traveler) {
				current_trip = trip;
			} else {
				current_trip++;
			}
			current_traveler = traveler;
			last_trip = trip;
			current_leg = input_plans.Leg ();
			n2_subarea = save_flag = false;
			last_location = 0;
			last_time = input_plans.Time ();
		}

		if (input_plans.Start_Type () != PARKING_ID ||
			input_plans.End_Type () != PARKING_ID) {

			//---- check for transit legs ----

			if (transit_flag) {
				if (input_plans.Mode () == TRANSIT_MODE) {
					n2_subarea = Transit_Leg ();
					last_time = subarea_plans.Stop_Time ();
					boarding_stop = 0;
					continue;
				} else {
					boarding_stop = 0;
				}
			}

			//---- process an access leg ----

			if (input_plans.Start_Type () == LOCATION_ID) {
				id = input_plans.Start_ID ();
				loc_ptr = location_data.Get (id);

				if (loc_ptr == NULL) {
					Warning ("Traveler %d Activity Location %d was Not Found", traveler, id);
					continue;
				}
				if (loc_ptr->Zone () == 0) continue;
				n2_subarea = true;

				if (transit_flag) {
					if (last_location > 0 && last_location != id) {

						//---- add a magic move ----

						if (!subarea_plans.Plan (plan_ptr)) {
							Error ("Coping Plan Record");
						}
						time = subarea_plans.Time ();

						subarea_plans.Leg (current_leg);
						subarea_plans.Time (last_time);
						subarea_plans.Start_ID (last_location);
						subarea_plans.Start_Type (LOCATION_ID);
						subarea_plans.Driver_Flag (0);
						subarea_plans.Mode (Db_Code::MAGIC_MODE);
						subarea_plans.End_ID (id);
						subarea_plans.End_Type (LOCATION_ID);
						if (last_time > time) {
							subarea_plans.Stop_Time (last_time+1);
						} else {
							subarea_plans.Stop_Time (time);
						}
						subarea_plans.Duration (subarea_plans.Stop_Time () - last_time);
						subarea_plans.GCF (0);
						subarea_plans.Tokens (0);

						Save_Plans ();
						current_leg++;
						last_time = subarea_plans.Stop_Time ();
						last_location = id;

						//---- write the plan records ----

						Save_Plans ();
						current_leg++;
					}
					if (input_plans.End_Type () == STOP_ID) {
						boarding_stop = input_plans.End_ID ();
					}
				}
			}
			if (input_plans.End_Type () == LOCATION_ID) {
				id = input_plans.End_ID ();
				loc_ptr = location_data.Get (id);

				if (loc_ptr == NULL) {
					Warning ("Traveler %d Activity Location %d was Not Found", traveler, id);
					continue;
				}
				if (loc_ptr->Zone () == 0) continue;
				n2_subarea = true;
				last_location = id;
			}
			if (!n2_subarea) continue;

			//---- copy the plan ----

			if (!subarea_plans.Plan (plan_ptr)) {
				Error ("Coping Plan Record");
			}

			//---- check the travel time ----

			if (early_flag || subarea_plans.Time () < last_time) {
				time = last_time - subarea_plans.Time ();
				if (time != 0) {
					num_adjust++;
					total_adjust += abs (time);

					subarea_plans.Time (last_time);
					time += subarea_plans.Stop_Time ();
					subarea_plans.Stop_Time (time);
				}
			}
			subarea_plans.Trip (current_trip);
			subarea_plans.Leg (current_leg);
			last_time = subarea_plans.Stop_Time ();

			//---- write the plan records ----

			Save_Plans ();
			current_leg++;
			continue;
		}

		//---- drive legs ----
		//---- copy the plan to the output record ----

		if (!subarea_plans.Plan (plan_ptr)) {
			Error ("Coping Plan Record");
		}
		subpath = subarea_plans.Path (&num_sub);

		num_sub = 0;

		veh_id = subarea_plans.Vehicle ();

		if (veh_id > 0) {
			veh_ptr = vehicle_data.Get (veh_id);

			if (veh_ptr == NULL) {
				Warning ("Traveler %d Vehicle %d was Not Found", traveler, veh_id);
				continue;
			}
		} else {
			veh_ptr = NULL;
		}

		//---- check if the ending parking lot is in the subarea ----

		id = input_plans.End_ID ();
		park_ptr = parking_data.Get (id);

		if (park_ptr == NULL) {
			Warning ("Traveler %d Parking Lot %d was Not Found", traveler, id);
			continue;
		}
		link = park_ptr->Link ();

		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Warning ("Traveler %d Link %d was Not Found", traveler, link);
			continue;
		}
		des_in = (link_ptr->Type () != 0);

		if (zone_flag) {
			j = link_ptr->Use ();
		} else {
			j = (des_in) ? 1 : 2;
		}

		//---- check if the starting parking lot is in the subarea ----

		id = input_plans.Start_ID ();
		park_ptr = parking_data.Get (id);

		if (park_ptr == NULL) {
			Warning ("Traveler %d Parking Lot %d was Not Found", traveler, id);
			continue;
		}
		link = park_ptr->Link ();

		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Warning ("Traveler %d Link %d was Not Found", traveler, link);
			continue;
		}
		in_subarea = org_in = (link_ptr->Type () != 0);
		save_flag = false;
		
		if (zone_flag) {
			i = link_ptr->Use ();
		} else {
			i = (org_in) ? 1 : 2;
		}

		//---- trace the path ----

		time = Round (input_plans.Time ());

		if (time_flag) {
			p = time_equiv.Period (input_plans.Time ());
		} else {
			p = 1;
		}
		path = input_plans.Path (&num_path);
		in_link_dir = 0;

		for (n1 = 0; num_path > 0; num_path--, n1 = n2) {
			n2 = *path++;

			if (type_flag) {

				//---- node path ----

				n = n2;

				if (n1 != 0) {
					ab_ptr = ab_key.Get (n1, n2);

					if (ab_ptr == NULL) {
						Warning ("Traveler %d No Link between %d and %d", traveler, n1, n2);
						break;
					}
					dir = ab_ptr->dir;
				} else if (n2 == link_ptr->Bnode ()) {
					dir = 0;
				} else {
					dir = 1;
				}
			} else {

				//---- get the link ----

				if (n2 < 0) {
					link = -n2;
					dir = 1;
				} else {
					link = n2;
					dir = 0;
				}
				link_ptr = link_data.Get (link);

				if (link_ptr == NULL) {
					Warning ("Traveler %d Link %d was Not Found", traveler, link);
					break;
				}
				n = (dir == 0) ? link_ptr->Bnode () : link_ptr->Anode ();

				if (n1 != 0) {
					dir = (dir == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();
				}
			}

			//---- get the node ----
			
			node_ptr = node_data.Get (n);

			if (node_ptr == NULL) {
				Warning ("Traveler %d Node %d was Not Found", traveler, n);
				continue;
			}
			n2_subarea = (node_ptr->Type () != 0);

			//---- get the link ----

			if (n1 == 0) {
				if (dir == 0) {
					dir = link_ptr->AB_Dir ();
					offset = (double) (link_ptr->Length () - park_ptr->Offset ()) / link_ptr->Length ();
				} else {
					dir = link_ptr->BA_Dir ();
					offset = (double) park_ptr->Offset () / link_ptr->Length ();
				}
			}

			//---- determine the travel time ----

			ttime_ptr = ttime_data [dir];

			if (ttime_ptr == NULL) {
				if (type_flag && n1 != 0) {
					Warning ("Traveler %d No Link between %d and %d", traveler, n1, n2);
				} else {
					Warning ("Traveler %d Link %d is Not Available", traveler, link_ptr->Link ());
				}
				continue;
			}
			link = ttime_ptr->Link ();

			if (delay_flag && ttime_ptr->Periods () > 0) {
				num = ttime_data.Period (Resolve (time));
				ttime = ttime_ptr->TTime (num);
			} else {
				ttime = num = 0;
			}
			if (ttime == 0) {
				ttime = ttime_ptr->Time0 ();
			} else if (ttime < 0) {
				ttime = Round (MIDNIGHT);
			}
			if (n1 == 0) {
				ttime = (int) (ttime * offset + 0.5);
			}

			//---- add the turn delay ----

			if (turn_flag) {
				if (in_link_dir > 0) {
					connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());

					if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
						ttime += connect_ptr->TTime (num);
						if (ttime < 1) ttime = 1;
					}
				}
				in_link_dir = ttime_ptr->Link_Dir ();
			}

			//---- check the subarea boundary ----

			if (in_subarea != n2_subarea) {
				boundary_ptr = (Boundary *) boundary.Get (&link);

				if (boundary_ptr == NULL) {
					Error ("Boundary Link %d was Not Found for Traveler %d", link, traveler);
				}
				if (in_subarea) {
					if (n1 != 0) {

						//---- end the trip at the boundary ----

						location = boundary_ptr->out_location;
						parking = boundary_ptr->out_parking;

						if (parking == 0) break;

						subarea_plans.Trip (current_trip);
						subarea_plans.Leg (current_leg++);
						subarea_plans.End_ID (parking);
						subarea_plans.Stop_Time (Resolve (time + ttime));

						if (!type_flag) {
							*subpath++ = n2;
							num_sub++;
						}
						if (Write_Plans (num_sub, veh_ptr, &save_flag)) {
							veh_ptr = vehicle_data.Get (veh_id);

							if (veh_ptr == NULL) {
								Warning ("Traveler %d Vehicle %d was Not Found", 
									subarea_plans.Traveler (), veh_id);
								break;
							}
						}
						subarea_plans.Leg (current_leg++);
						subarea_plans.Time (subarea_plans.Stop_Time ());
						subarea_plans.Start_ID (subarea_plans.End_ID ());
						subarea_plans.Start_Type (subarea_plans.End_Type ());
						subarea_plans.Driver_Flag (0);
						subarea_plans.Mode (Db_Code::WALK_MODE);
						subarea_plans.End_ID (location);
						subarea_plans.End_Type (LOCATION_ID);
						subarea_plans.Stop_Time (subarea_plans.Time () + 1);
						subarea_plans.Duration (1);
						subarea_plans.GCF (0);
						subarea_plans.Tokens (0);

						Save_Plans ();

						current_trip++;
						current_leg = 1;
					}
					in_subarea = false;

				} else {

					//---- start the trip at the boundary ----

					location = boundary_ptr->in_location;
					parking = boundary_ptr->in_parking;

					if (parking == 0) break;

					time = Resolve (time);

					subarea_plans.Trip (current_trip);
					subarea_plans.Leg (current_leg++);
					subarea_plans.Time (time++);
					subarea_plans.Start_ID (location);
					subarea_plans.Start_Type (LOCATION_ID);
					subarea_plans.Driver_Flag (0);
					subarea_plans.Mode (Db_Code::WALK_MODE);
					subarea_plans.End_ID (parking);
					subarea_plans.End_Type (PARKING_ID);
					subarea_plans.Stop_Time (time);
					subarea_plans.Duration (1);
					subarea_plans.GCF (0);
					subarea_plans.Tokens (0);

					save_flag = Save_Plans ();

					subarea_plans.Plan (plan_ptr);

					subarea_plans.Trip (current_trip);
					subarea_plans.Leg (current_leg++);

					subpath = subarea_plans.Path (&num_sub);
					num_sub = 0;

					if (time_flag) {
						p = time_equiv.Period (time);
					} else {
						p = 1;
					}
					subarea_plans.Time (time);
					subarea_plans.Start_ID (parking);
					subarea_plans.Start_Type (PARKING_ID);

					time = Round (subarea_plans.Time ());

					in_subarea = true;
				}
			}

			//---- update the time of day ----

			time += ttime;

			//---- save the subarea node ----
			
			if (in_subarea) {
				*subpath++ = n2;
				num_sub++;
			}
		}
		if (num_path > 0) continue;

		n2_subarea = (num_sub > 0);

		if (in_subarea && n2_subarea) {
			subarea_plans.Stop_Time (Resolve (time));

			Write_Plans (num_sub, veh_ptr, &save_flag);

			last_time = subarea_plans.Stop_Time ();
		}

		if (save_flag) {
			if (org_in == true) {
				if (des_in == true) {
					num_in_in++;
				} else {
					num_in_out++;
				}
			} else {
				if (des_in == true) {
					num_out_in++;
				} else {
					num_out_out++;
				}
			}
			if (trip_flag && p > 0 && i > 0 && j > 0) {
				(trips [p] [i] [j])++;
			}
		}
	}
	End_Progress ();

	input_plans.Close ();
}
