//*********************************************************
//	Read_Plans.cpp - Read Each Plan File
//*********************************************************

#include "AdjustPlans.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void AdjustPlans::Read_Plans (void)
{
	int i, id, tod, time, *path, num_path, anode, length, dir, len_base, traveler, vehicle;
	int in_link_dir, link, lnk, group;

	Travel_Data *travel_ptr;
	Count_Data *count_ptr;
	AB_Data *ab_ptr;
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Parking_Data *parking_ptr;
	Connect_Time *connect_ptr;
	Integer_List *list;

	//---- set the filename and extension ----

	if (plan_file.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	len_base = 100000;
	travel_ptr = new Travel_Data ();

	//---- read each plan record ----

	while (plan_file.Read ()) {
		Show_Progress ();

		if (plan_file.Start_Type () != PARKING_ID || 
			plan_file.End_Type () != PARKING_ID || plan_file.Tokens () == 0) continue;

		traveler = plan_file.Traveler ();
		if (traveler > max_traveler) max_traveler = traveler;

		vehicle = plan_file.Vehicle ();
		if (vehicle > max_vehicle) max_vehicle = vehicle;
		
		tod = Round (plan_file.Time ());

		//--- get origin parking lot ----

		id = plan_file.Start_ID ();

		parking_ptr = parking_data.Get (id);
		in_link_dir = 0;

		if (parking_ptr == NULL) {
			Error ("Parking Lot %d was Not Found in the Parking File", id);
		}
			
		path = plan_file.Path (&num_path);

		if (type_flag) {

			//---- node-based plans ----

			anode = 0;

			for (i=1; i <= num_path+1; i++, path++) {
				length = len_base;

				if (i == 1) {
					link_ptr = link_data [parking_ptr->Link ()];

					id = node_data.Get_Index (*path);
					if (id == 0) {
						Error ("Traveler %d Node %d was Not Found", traveler, *path);
					}

					if (link_ptr->Bnode () == id) {
						dir = link_ptr->AB_Dir ();
						length = link_ptr->Length () - parking_ptr->Offset ();
					} else if (link_ptr->Anode () == id) {
						dir = link_ptr->BA_Dir ();
						length = parking_ptr->Offset ();
					} else {
						Error ("Traveler %d Node %d is not on Origin Link %d", 
							traveler, *path, link_ptr->Link ());
					}
					if (link_ptr->Length () > 0) {
						length = len_base * length / link_ptr->Length ();
					}
				} else if (i > num_path) {

					id = plan_file.End_ID ();

					parking_ptr = parking_data.Get (id);

					if (parking_ptr == NULL) {
						Error ("Parking Lot %d was Not Found in the Parking File", id);
					}
					link_ptr = link_data [parking_ptr->Link ()];

					id = node_data.Get_Index (anode);
					if (id == 0) {
						Error ("Traveler %d Node %d was Not Found", traveler, anode);
					}
					if (link_ptr->Anode () == id) {
						dir = link_ptr->AB_Dir ();
					} else if (link_ptr->Bnode () == id) {
						dir = link_ptr->BA_Dir ();
					} else {
						Error ("Traveler %d Node %d is not on Destination Link %d", 
							traveler, anode, link_ptr->Link ());
					}
				} else {
					ab_ptr = ab_key.Get (anode, *path);

					if (ab_ptr == NULL) {
						Error ("A Link between %d and %d was Not Found", anode, *path);
					}
					dir = ab_ptr->dir;
				}
				anode = *path;

				//---- get the link travel time ----

				ttime_ptr = ttime_data [dir];

				if (delay_flag) {

					//---- add the turning movement delay -----

					if (turn_flag) {
						if (in_link_dir > 0) {
							connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());

							if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
								time = connect_ptr->TTime (connect_time.Period (Resolve (tod)));
								tod += time;
							}
						}
						in_link_dir = ttime_ptr->Link_Dir ();
					}
					time = ttime_ptr->TTime (ttime_data.Period (Resolve (tod)));
				} else {
					time = ttime_ptr->Time0 ();
				}
				if (length < len_base) {
					time = time * length / len_base;
					if (time < 1) time = 1;
				}

				//---- look for a count ----

				if (equiv_flag) {
					link = ttime_ptr->Link ();
					if (ttime_ptr->Dir () > 0) link = -link;

					for (group=1; group <= link_equiv.Max_Group (); group++) {
						list = link_equiv.Group_List (group);
						if (list == NULL) continue;

						for (lnk = list->First (); lnk != 0; lnk = list->Next ()) {
							if (lnk == link) {
								count_ptr = count_data.Get_LE (group, tod);

								if (count_ptr != NULL) {
									if (count_ptr->Dir_Index () == group && count_ptr->End_Time () > tod) {
										count_ptr->Add_Volume ();
										if (!travel_ptr->Add_Count (count_data.Record_Index ())) goto count_error;
									}
								}
								break;
							}
						}
					}
				} else {

					count_ptr = count_data.Get_LE (dir, tod);

					if (count_ptr != NULL) {
						if (count_ptr->Dir_Index () == dir && count_ptr->End_Time () > tod) {
							count_ptr->Add_Volume ();
							if (!travel_ptr->Add_Count (count_data.Record_Index ())) goto count_error;
						}
					}
				}
				tod += time;
			}

		} else {

			//---- link-based plans ----

			for (i=1; i <= num_path; i++, path++) {
				id = *path;

				if (id < 0) {
					dir = 1;
					id = -id;
				} else {
					dir = 0;
				}
				link_ptr = link_data.Get (id);

				if (link_ptr == NULL) {
					Error ("Traveler %d Link %d is not in the Link File", traveler, id);
				}
				length = link_ptr->Length ();

				if (dir == 0) {
					dir = link_ptr->AB_Dir ();
					if (i == 1 && length > 0) {
						length = len_base * (length - parking_ptr->Offset ()) / length;
					} else {
						length = len_base;
					}
				} else {
					dir = link_ptr->BA_Dir ();
					if (i == 1 && length > 0) {
						length = len_base * parking_ptr->Offset () / length;
					} else {
						length = len_base;
					}
				}
				if (dir == 0) {
					Error ("Traffic is Not Permitted on Link %d in %s Direction", id, ((*path > 0) ? "AB" : "BA"));
				}

				//---- get the link travel time ----

				ttime_ptr = ttime_data [dir];

				if (delay_flag) {

					//---- add the turning movement delay -----

					if (turn_flag) {
						if (in_link_dir > 0) {
							connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());

							if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
								time = connect_ptr->TTime (connect_time.Period (Resolve (tod)));
								tod += time;
							}
						}
						in_link_dir = ttime_ptr->Link_Dir ();
					}
					time = ttime_ptr->TTime (ttime_data.Period (Resolve (tod)));
				} else {
					time = ttime_ptr->Time0 ();
				}
				if (length < len_base) {
					time = time * length / len_base;
					if (time < 1) time = 1;
  				}

				//---- look for a count ----

				if (equiv_flag) {
					link = ttime_ptr->Link ();
					if (ttime_ptr->Dir () > 0) link = -link;

					for (group=1; group <= link_equiv.Max_Group (); group++) {
						list = link_equiv.Group_List (group);
						if (list == NULL) continue;

						for (lnk = list->First (); lnk != 0; lnk = list->Next ()) {
							if (lnk == link) {
								count_ptr = count_data.Get_LE (group, tod);

								if (count_ptr != NULL) {
									if (count_ptr->Dir_Index () == group && count_ptr->End_Time () > tod) {
										count_ptr->Add_Volume ();
										if (!travel_ptr->Add_Count (count_data.Record_Index ())) goto count_error;
									}
								}
								break;
							}
						}
					}
				} else {
					count_ptr = count_data.Get_LE (dir, tod);

					if (count_ptr != NULL) {
						if (count_ptr->Dir_Index () == dir && count_ptr->End_Time () > tod) {
							count_ptr->Add_Volume ();
							if (!travel_ptr->Add_Count (count_data.Record_Index ())) goto count_error;
						}
					}
				}
				tod += time;
			}
		}

		//---- add the plan to the array ----

		if (travel_ptr->First () > 0) {
			travel_ptr->Traveler (traveler);
			travel_ptr->Weight (1);
			travel_ptr->Optimize ();

			if (!travel_data.Add (travel_ptr)) {
				Error ("Insufficient Memory for Travel Paths");
			}
			travel_ptr = new Travel_Data ();
		}
	}
	End_Progress ();

	delete travel_ptr;
	plan_file.Close ();
	return;

count_error:
	Error ("Adding Count to the Plan for Traveler %d", plan_file.Traveler ());
}
