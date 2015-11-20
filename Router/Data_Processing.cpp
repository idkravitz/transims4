//*********************************************************
//	Data_Processing.cpp - data processing classes
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Data_Processing
//---------------------------------------------------------

void Router::Data_Processing (void)
{
	int record, *list, nstop, nparking, nlocation, naccess;

	int nnode = node_data.Num_Records ();
	int nlink = link_data.Num_Records ();
	int ndir = ttime_data.Num_Records ();

	//---- prepare data for drive paths ----

	if (walk_net || bike_net || drive_net) {
		int dir;
		int nwalk = 0;
		int nbike = 0;
		int ndrive = 0;
		bool cap_update = false;
		double cap_factor = 1.0;

		Link_Data *link_ptr;
		TTime_Data *ttime_ptr;
		List_Data *walk_ptr, *bike_ptr;

		//---- initialize network data ----

		if (walk_net) {
			if (!walk_list.Num_Records (nnode) || !walk_link.Num_Records (nlink)) {
				Error ("Insufficient Memory for Walk Data");
			}
		}
		if (bike_net) {
			if (!bike_list.Num_Records (nnode) || !bike_link.Num_Records (nlink)) {
				Error ("Insufficient Memory for Bike Data");
			}
		}
		if (drive_net && tod_flag) {
			int min_size = ttime_data.Min_Size ();
			int max_size = ttime_data.Max_Size ();

			//---- set the period capacity factor ----

			if (delay_flag) {
				time_period = ttime_data.Period_Size ();
			} else {
				ttime_data.Period_Size (time_period, min_size, max_size);
				connect_time.Period_Size (time_period, min_size, max_size);
			}
			cap_factor = time_period / 3600.0;
			cap_update = (update_flag && cap_factor != 1.0);

			//---- round the time period size ----

			int nperiod = ttime_data.Periods ();
			int size = Round (ttime_data.Period_Size ());

			min_size = Round (min_size);
			max_size = Round (max_size);

			ttime_data.Period_Size (size, min_size, max_size);
			ttime_data.Periods (nperiod);

			connect_time.Period_Size (size, min_size, max_size);
			connect_time.Periods (nperiod);
		}

		//---- process each link ----

		for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
			record = link_data.Record_Index ();

			if (walk_net && Use_Permission (link_ptr->Use (), WALK)) {
				walk_ptr = walk_link [record];
				nwalk++;

				//---- add to the link lists ----

				list = walk_list.Record (link_ptr->Anode ());

				walk_ptr->From_List (*list);
				*list = record;

				list = walk_list.Record (link_ptr->Bnode ());

				walk_ptr->To_List (*list);
				*list = record;
			}

			if (bike_net && Use_Permission (link_ptr->Use (), BIKE)) {
				bike_ptr = bike_link [record];
				nbike++;

				//---- add to the link lists ----

				list = bike_list.Record (link_ptr->Anode ());

				bike_ptr->From_List (*list);
				*list = record;

				list = bike_list.Record (link_ptr->Bnode ());

				bike_ptr->To_List (*list);
				*list = record;
			}

			if (drive_net && (Use_Permission (link_ptr->Use (), CAR) || 
				Use_Permission (link_ptr->Use (), TRUCK) ||
				Use_Permission (link_ptr->Use (), SOV) ||
				Use_Permission (link_ptr->Use (), HOV2) ||
				Use_Permission (link_ptr->Use (), HOV3) ||
				Use_Permission (link_ptr->Use (), HOV4) ||
				Use_Permission (link_ptr->Use (), LIGHTTRUCK) ||
				Use_Permission (link_ptr->Use (), HEAVYTRUCK))) {

				//---- process A->B direction ----

				dir = link_ptr->AB_Dir ();

				if (dir >  0) {
					ndrive++;

					if (tod_flag) {
						ttime_ptr = ttime_data [dir];
						if (ttime_ptr == NULL) goto dir_error;

						int nperiod = ttime_ptr->Periods ();

						if (nperiod == 0) {
							nperiod = ttime_data.Periods ();

							if (!ttime_ptr->Periods (nperiod)) goto mem_error;
						}
						if (!delay_flag) {
							for (int i=1; i <= nperiod; i++) {
								ttime_ptr->TTime (i, ttime_ptr->Time0 ());
							}
						}
						if (cap_update) {
							ttime_ptr->Capacity ((int) (ttime_ptr->Capacity () * cap_factor + 0.5));
						}
					}
				}

				//---- process B->A direction ----

				dir = link_ptr->BA_Dir ();

				if (dir > 0) {
					ndrive++;
					if (tod_flag) {
						ttime_ptr = ttime_data [dir];
						if (ttime_ptr == NULL) goto dir_error;

						int nperiod = ttime_ptr->Periods ();

						if (nperiod == 0) {
							nperiod = ttime_data.Periods ();

							if (!ttime_ptr->Periods (nperiod)) goto mem_error;
						}
						if (!delay_flag) {
							for (int i=1; i <= nperiod; i++) {
								ttime_ptr->TTime (i, ttime_ptr->Time0 ());
							}
						}
						if (cap_update) {
							ttime_ptr->Capacity ((int) (ttime_ptr->Capacity () * cap_factor + 0.5));
						}
					}
				}
			} else {
				link_ptr->AB_Dir (0);
				link_ptr->BA_Dir (0);
			}
		}
		if (walk_net || bike_net || drive_net) {
			Break_Check (4);
			Print (1);
		}
		if (walk_net) {
			Print (1, "Number of Links in the Walk Network = %d", nwalk);
		}
		if (bike_net) {
			Print (1, "Number of Links in the Bicycle Network = %d", nbike);
		}
		if (drive_net) {
			Print (1, "Number of Directional Links in the Highway Network = %d", ndrive);
		}
	}

	//---- construct the approach-departure list ----

	if (drive_net) {
		int from_dir, to_dir;
		Connect_Time *connect_ptr;
		Link_Data *link_ptr;

		int nconnect = connect_data.Num_Records ();

		if (!drive_list.Num_Records (ndir)) {
			Error ("Insufficient Memory for Link Connection Data");
		}

		//---- process each connection ----

		nconnect = 0;

		for (connect_ptr = connect_time.First (); connect_ptr; connect_ptr = connect_time.Next ()) {

			//---- convert the id code to dir index ----

			link_ptr = link_data [connect_ptr->In_Link ()];
			from_dir = (connect_ptr->In_Dir () == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();

			if (from_dir == 0) continue;

			link_ptr = link_data [connect_ptr->Out_Link ()];
			to_dir = (connect_ptr->Out_Dir () == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();

			if (to_dir == 0) continue;

			//---- add to the connection lists ----

			list = drive_list.Record (from_dir);

			connect_ptr->Next_Rec (*list);
			connect_ptr->Dir_Index (to_dir);

			*list = connect_time.Record_Index ();
			nconnect++;
		}

		Print (2, "Number of Highway Link Connections = %d", nconnect);

		//---- process turn prohibitions ----

		nconnect = turn_data.Num_Records ();

		if (nconnect > 0) {
			Turn_Data *turn_ptr;
			TOD_Turn_Data *tod_turn_ptr;

			if (!tod_turn.Num_Records (nconnect)) {
				Error ("Insufficient Memory for Turn Prohibition Data");
			}

			//---- process each turn prohibition ----

			nconnect = 0;

			for (turn_ptr = turn_data.First (); turn_ptr; turn_ptr = turn_data.Next ()) {

				//---- convert the id code to dir index ----

				link_ptr = link_data [turn_ptr->In_Link ()];
				from_dir = (turn_ptr->In_Dir () == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();

				if (from_dir == 0) continue;

				link_ptr = link_data [turn_ptr->Out_Link ()];
				to_dir = (turn_ptr->Out_Dir () == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();

				if (to_dir == 0) continue;

				//---- add to the connection lists ----

				for (record = drive_list [from_dir]; record; record = connect_ptr->Next_Rec ()) {

					connect_ptr = connect_time [record];

					if (connect_ptr->Dir_Index () == to_dir) {
						tod_turn_ptr = tod_turn [++nconnect];

						tod_turn_ptr->TOD_List (connect_ptr->TOD_List ());
						connect_ptr->TOD_List (nconnect);

						tod_turn_ptr->Start (turn_ptr->Start ());
						tod_turn_ptr->End (turn_ptr->End ());
						tod_turn_ptr->Use (turn_ptr->Use ());
						tod_turn_ptr->Penalty (turn_ptr->Penalty ());
						break;
					}
				}
			}
			tod_turn.Max_Records (nconnect);

			Print (2, "Number of Highway Turn Prohibitions = %d", nconnect);

			turn_data.Clear ();
		}

		//---- process toll records ----

		if (toll_flag && toll_data.Num_Records () > 0) {
			int dir;
			Toll_Data *toll_ptr;
			Dir_Data *dir_ptr;

			for (toll_ptr = toll_data.First (); toll_ptr; toll_ptr = toll_data.Next ()) {

				//---- convert the link code to dir index ----

				link_ptr = link_data [toll_ptr->Link ()];
				dir = (toll_ptr->Dir () == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();

				if (dir == 0) continue;

				dir_ptr = dir_data [dir];

				toll_ptr->TOD_List (dir_ptr->Cost_List ());
				dir_ptr->Cost_List (toll_data.Record_Index ());
			}
		}
	}

	//---- build the access list ----

	naccess = access_data.Num_Records ();
	nlocation = location_data.Num_Records ();
	nparking = parking_data.Num_Records ();

	if (!access_list.Num_Records (naccess) ||
		!parking_access.Num_Records (nlocation) || !parking_egress.Num_Records (nparking)) {
		Error ("Insufficient Memory for Access List Data");
	}
	nstop = 0;
	if (transit_net) {
		nstop = stop_data.Num_Records ();
		if (!transit_access.Num_Records (nlocation) || !transit_egress.Num_Records (nstop)) {
			Error ("Insufficient Memory for Access List Data");
		}
	}
	Access_Data *access_ptr;
	List_Data *list_ptr, *acc_ptr;
	Location_Data *location_ptr;

	for (access_ptr = access_data.First (); access_ptr; access_ptr = access_data.Next ()) {
		record = access_data.Record_Index ();

		acc_ptr = access_list [record];

		//---- update the access lists ---

		if (access_ptr->From_Type () == LOCATION_ID) {
			if (access_ptr->To_Type () == PARKING_ID) {
				list_ptr = parking_access [access_ptr->From_ID ()];

				acc_ptr->From_List (list_ptr->From_List ());
				list_ptr->From_List (record);

				if (walk_net || select_od) {
					list_ptr = parking_egress [access_ptr->To_ID ()];

					acc_ptr->To_List (list_ptr->To_List ());
					list_ptr->To_List (record);
				}
			} else if (transit_net && access_ptr->To_Type () == STOP_ID) {
				list_ptr = transit_access [access_ptr->From_ID ()];

				acc_ptr->From_List (list_ptr->From_List ());
				list_ptr->From_List (record);

				list_ptr = transit_egress [access_ptr->To_ID ()];

				acc_ptr->To_List (list_ptr->To_List ());
				list_ptr->To_List (record);

				//---- check walk access to transit ----

				location_ptr = location_data [access_ptr->From_ID ()];

				if (location_ptr->Link () == 0) {
					Warning ("No Walk Access to Transit Stop %d through Activity Location %d", 
						stop_data [access_ptr->To_ID ()]->ID (), location_ptr->ID ());
				}
			}
			
		} else if (access_ptr->To_Type () == LOCATION_ID) {
			if (access_ptr->From_Type () == PARKING_ID) {
				list_ptr = parking_access [access_ptr->To_ID ()];

				acc_ptr->To_List (list_ptr->To_List ());
				list_ptr->To_List (record);

				if (walk_net || select_od) {
					list_ptr = parking_egress [access_ptr->From_ID ()];

					acc_ptr->From_List (list_ptr->From_List ());
					list_ptr->From_List (record);
				}
			} else if (transit_net && access_ptr->From_Type () == STOP_ID) {
				list_ptr = transit_access [access_ptr->To_ID ()];

				acc_ptr->To_List (list_ptr->To_List ());
				list_ptr->To_List (record);

				list_ptr = transit_egress [access_ptr->From_ID ()];

				acc_ptr->From_List (list_ptr->From_List ());
				list_ptr->From_List (record);

				//---- check walk egress from transit ----

				location_ptr = location_data [access_ptr->To_ID ()];

				if (location_ptr->Link () == 0) {
					Warning ("No Walk Access to Transit Stop %d through Activity Location %d", 
						stop_data [access_ptr->From_ID ()]->ID (), location_ptr->ID ());
				}
			}
		}
	}

	//---- build the route stop list ----

	if (transit_net) {
		int i, stop, *link;
		Route_Stop_Data rstop_rec;
		Line_Data *line_ptr;

		if (!link_list.Num_Records (nlink) || !loc_list.Num_Records (nlocation) ||
			!stop_list.Num_Records (nstop)) {
			Error ("Insufficient Memory for Transit Lists");
		}
		fare_flag = (Num_Fare_Zones () > 0);

		if (fare_flag) {
			Print (2, "Number of Fare Zones = %d", Num_Fare_Zones ());
		}
		nroute = line_data.Num_Records () + 1;

		for (line_ptr = line_data.First (); line_ptr; line_ptr = line_data.Next ()) {
			rstop_rec.Route (line_data.Record_Index ());

			//if (line_ptr->Runs () == 0) {
			//	Error ("Route %d has no Scheduled Service", line_ptr->Route ());
			//}

			for (i=1; i <= line_ptr->Stops (); i++) {
				rstop_rec.Stop (i);

				stop = line_ptr->Stop (i);

				list = stop_list.Record (stop);

				rstop_rec.List (*list);

				if (!route_stop.Add (&rstop_rec)) {
					Error ("Adding Route-Stop data to the List");
				}
				*list = route_stop.Num_Records ();
			}
		}
		for (location_ptr = location_data.First (); location_ptr; location_ptr = location_data.Next ()) {
			record = location_data.Record_Index ();

			link = link_list.Record (location_ptr->Link ());
			list = loc_list.Record (record);

			*list = *link;
			*link = record;
		}

		//----- get the park-&-ride lots ----

		if (park_ride_flag) {
			Parking_Data *park_ptr;
			Link_Data *link_ptr;
			Node_Data *node_ptr;
			Park_Ride_Data park_rec;

			int ax, ay, bx, by;
			double factor;

			for (park_ptr = parking_data.First (); park_ptr; park_ptr = parking_data.Next ()) {
				if (park_ptr->Type () != PARKRIDE) continue;

				link_ptr = link_data [park_ptr->Link ()];

				node_ptr = node_data [link_ptr->Anode ()];

				ax = node_ptr->X ();
				ay = node_ptr->Y ();

				node_ptr = node_data [link_ptr->Bnode ()];

				bx = node_ptr->X ();
				by = node_ptr->Y ();

				factor = (double) park_ptr->Offset () / (double) link_ptr->Length ();

				park_rec.Parking (parking_data.Record_Index ());
				park_rec.X (ax + (int) ((bx - ax) * factor + 0.5));
				park_rec.Y (ay + (int) ((by - ay) * factor + 0.5));

				if (!park_ride.Add (&park_rec)) {
					Error ("Adding Park-&-Ride Lot to the List");
				}
			}
			Print (2, "Number of Park-&-Ride Lots = %d", park_ride.Num_Records ());
		}

		//----- get the kiss-&-ride lots ----

		if (kiss_ride_flag) {
			Parking_Data *park_ptr;
			Link_Data *link_ptr;
			Node_Data *node_ptr;
			Stop_Data *stop_ptr;
			Park_Ride_Data kiss_rec;

			int ax, ay, bx, by, distance;
			double factor;
			bool flag;

			for (park_ptr = parking_data.First (); park_ptr; park_ptr = parking_data.Next ()) {
				link_ptr = link_data [park_ptr->Link ()];

				node_ptr = node_data [link_ptr->Anode ()];

				ax = node_ptr->X ();
				ay = node_ptr->Y ();

				node_ptr = node_data [link_ptr->Bnode ()];

				bx = node_ptr->X ();
				by = node_ptr->Y ();

				factor = (double) park_ptr->Offset () / (double) link_ptr->Length ();

				kiss_rec.Parking (parking_data.Record_Index ());
				kiss_rec.X (ax + (int) ((bx - ax) * factor + 0.5));
				kiss_rec.Y (ay + (int) ((by - ay) * factor + 0.5));
				flag = false;

				for (stop_ptr = stop_data.First (); stop_ptr; stop_ptr = stop_data.Next ()) {
					if (!kissride_type [stop_ptr->Type ()]) continue;

					link_ptr = link_data [stop_ptr->Link ()];

					node_ptr = node_data [link_ptr->Anode ()];

					ax = node_ptr->X ();
					ay = node_ptr->Y ();

					node_ptr = node_data [link_ptr->Bnode ()];

					bx = node_ptr->X ();
					by = node_ptr->Y ();

					factor = (double) stop_ptr->Offset () / (double) link_ptr->Length ();

					ax += (int) ((bx - ax) * factor + 0.5) - kiss_rec.X ();
					ay += (int) ((by - ay) * factor + 0.5) - kiss_rec.Y ();

					distance = abs (ax) + abs (ay);
					if (Resolve (distance) < kissride_walk) {
						flag = true;
						break;
					}
				}
				if (flag) {
					if (!kiss_ride.Add (&kiss_rec)) {
						Error ("Adding Kiss-&-Ride Lot to the List");
					}
				}
			}
			Print (2, "Number of Kiss-&-Ride Lots = %d", kiss_ride.Num_Records ());
		}

		//---- stop penalty file ----

		if (stop_imp_flag) {
			Line_Data *line_ptr;

			int s, stop, imp, route, run, stop_field, imp_field, route_field, run_field, num_penalty, num_stops;
			
			stop_field = stop_imp_file.Required_Field ("STOP", "STOP_ID", "ID");
			imp_field = stop_imp_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
			route_field = stop_imp_file.Optional_Field ("ROUTE", "LINE", "ROUTE_ID", "LINE_ID");
			run_field = stop_imp_file.Optional_Field ("RUN", "TRIP");

			//---- process each stop penalty file record ----

			num_penalty = num_stops = 0;

			while (stop_imp_file.Read ()) {
				stop_imp_file.Get_Field (imp_field, &imp);
				if (imp <= 0) continue;
				imp = Round (imp);

				stop_imp_file.Get_Field (route_field, &route);
				stop_imp_file.Get_Field (run_field, &run);
				stop_imp_file.Get_Field (stop_field, &stop);

				if (stop > 0) {
					s = stop;
					stop = stop_data.Get_Index (stop);
					if (stop == 0) {
						Warning ("Transit Penalty Stop %d was Not Found", s);
						continue;
					}
				}
				if (route > 0) {
					line_ptr = line_data.Get (route);
					if (line_ptr == NULL) {
						Warning ("Transit Penalty Route %d was Not Found", route);
					}
					for (s=1; s <= line_ptr->Stops (); s++) {
						if (stop == 0 || line_ptr->Stop (s) == stop) {
							if (run <= 0) {
								for (int r=1; r <= line_ptr->Runs (); r++) {
									line_ptr->Penalty (r, s, imp);
									num_stops++;
								}
							} else {
								line_ptr->Penalty (run, s, imp);
								num_stops++;
							}
						}
					}
				} else {
					for (line_ptr = line_data.First (); line_ptr; line_ptr = line_data.Next ()) {
						for (s=1; s <= line_ptr->Stops (); s++) {
							if (stop == 0 || line_ptr->Stop (s) == stop) {
								if (run <= 0) {
									for (int r=1; r <= line_ptr->Runs (); r++) {
										line_ptr->Penalty (r, s, imp);
										num_stops++;
									}
								} else {
									line_ptr->Penalty (run, s, imp);
									num_stops++;
								}
							}
						}
					}
				}
				num_penalty++;
			}
			Print (2, "Number of Transit Penalty Records = %d", num_penalty);
			Print (1, "Number of Route-Run-Stop Penalties = %d", num_stops);
		}
	}

	//---- parking penalty file ----

	if (park_imp_flag && (drive_net || transit_net)) {
		Parking_Data *park_ptr;

		int parking, imp, park_field, imp_field, num_penalty;
		
		park_field = park_imp_file.Required_Field ("PARKING", "LOT", "ID", "NUMBER", "LOT_ID");
		imp_field = park_imp_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");

		//---- initialize the penalty field ----

		for (park_ptr = parking_data.First (); park_ptr; park_ptr = parking_data.Next ()) {
			park_ptr->Penalty (0);
		}

		//---- process each parking penalty file record ----

		num_penalty = 0;

		while (park_imp_file.Read ()) {
			park_imp_file.Get_Field (park_field, &parking);
			if (parking <= 0) continue;

			park_imp_file.Get_Field (imp_field, &imp);
			if (imp <= 0) continue;

			park_ptr = parking_data.Get (parking);
			if (park_ptr == NULL) {
				Warning ("Parking Penalty Lot %d was Not Found", parking);
				continue;
			}
			park_ptr->Penalty (Round (imp));
			num_penalty++;
		}
		Print (2, "Number of Parking Penalty Records = %d", num_penalty);
	}

	//---- path building data ----

	plan_file.Check_Size (nnode + 1);

	if (drive_net) {
		if (!link_path.Num_Records (ndir)) goto path_error;
	}

	if (transit_net) {
		for (int i=0; i <= max_paths; i++) {
			if (!node_path [i].Num_Records (nnode)) goto path_error;
			if (!loc_path [i].Num_Records (nlocation)) goto path_error;
			if (i < max_paths) {
				if (!board_path [i].Num_Records (nstop)) goto path_error;
				if (!alight_path [i].Num_Records (nstop)) goto path_error;
			}
		}
	} else if (walk_net || bike_net) {
		if (!node_path [0].Num_Records (nnode)) goto path_error;
	}
	return;

path_error:
	Error ("Insufficient Memory for Path Building data");

mem_error:
	Error ("Insufficient Memory for Time of Day data");

dir_error:
	Error ("Accessing Directional Link Records");
}
