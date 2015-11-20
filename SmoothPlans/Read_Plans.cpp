//*********************************************************
//	Read_Plans.cpp - Read Each Plan File
//*********************************************************

#include "SmoothPlans.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void SmoothPlans::Read_Plans (void)
{
	int i, id, index, tod, time, *path, num_path, anode, length, dir, len_base, traveler;
	int start, in_link_dir, period;

	Travel_Data *travel_ptr;
	AB_Data *ab_ptr;
	Link_Time link_rec;
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Parking_Data *parking_ptr;
	Connect_Time *connect_ptr;

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
		
		start = tod = Round (plan_file.Time ());

		if (link_flag) {

			//--- get origin parking lot ----

			id = plan_file.Start_ID ();

			parking_ptr = parking_data.Get (id);

			if (parking_ptr == NULL) {
				Write (1, "Traveler %d Parking Lot %d was Not Found", traveler, id);
				continue;
			}

			if (type_flag) {

				//---- node-based plans ----

				path = plan_file.Path (&num_path);
				anode = in_link_dir = 0;

				for (i=1; i <= num_path; i++, path++) {
					length = len_base;

					if (i == 1) {
						link_ptr = link_data [parking_ptr->Link ()];

						id = node_data.Get_Index (*path);
						if (id == 0) {
							Write (1, "Traveler %d Node %d was Not Found", traveler, *path);
							continue;
						}

						if (link_ptr->Bnode () == id) {
							dir = link_ptr->AB_Dir ();
							length = link_ptr->Length () - parking_ptr->Offset ();
						} else if (link_ptr->Anode () == id) {
							dir = link_ptr->BA_Dir ();
							length = parking_ptr->Offset ();
						} else {
							Write (1, "Traveler %d Node %d is not on Origin Link %d", 
								traveler, *path, link_ptr->Link ());
							continue;
						}
						if (link_ptr->Length () > 0) {
							length = len_base * length / link_ptr->Length ();
						}
					} else {
						ab_ptr = ab_key.Get (anode, *path);

						if (ab_ptr == NULL) {
							Write (1, "Traveler %d No Link between %d and %d", traveler, anode, *path);
							continue;
						}
						dir = ab_ptr->dir;
					}
					anode = *path;

					ttime_ptr = ttime_data [dir];

					if (delay_flag) {
						period = ttime_data.Period (Resolve (tod));
						time = ttime_ptr->TTime (period);
					} else {
						time = ttime_ptr->Time0 ();
					}
					if (length < len_base) {
						time = time * length / len_base;
						if (time < 1) time = 1;
					}

					//---- add the turn delay ----

					if (delay_flag && turn_flag) {
						if (in_link_dir > 0) {
							connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());

							if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
								time += connect_ptr->TTime (period);
								if (time < 1) time = 1;
							}
						}
						in_link_dir = ttime_ptr->Link_Dir ();
					}

					//---- look for a link ----

					index = link_list.Get_Index (dir);

					if (index != 0) {
						link_rec.Dir_Index (index);
						link_rec.Time_Period (Resolve (tod) / increment + 1);

						if (!travel_ptr->Add_Dir (&link_rec)) goto dir_error;
					}
					tod += time;
				}

			} else {

				//---- link-based plans ----

				path = plan_file.Path (&num_path);

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
						Write (1, "Traveler %d Link %d was Not Found", traveler, id);
						continue;
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
						Write (1, "Traveler %d Link %d is Available in %s Direction", traveler, id, ((*path > 0) ? "AB" : "BA"));
					}
					ttime_ptr = ttime_data [dir];

					if (delay_flag) {
						period = ttime_data.Period (Resolve (tod));
						time = ttime_ptr->TTime (period);
					} else {
						time = ttime_ptr->Time0 ();
					}
					if (length < len_base) {
						time = time * length / len_base;
						if (time < 1) time = 1;
  					}

					//---- add the turn delay ----

					if (delay_flag && turn_flag) {
						if (in_link_dir > 0) {
							connect_ptr = connect_time.Get (in_link_dir, ttime_ptr->Link_Dir ());

							if (connect_ptr != NULL && connect_ptr->Periods () > 0) {
								time += connect_ptr->TTime (period);
								if (time < 1) time = 1;
							}
						}
						in_link_dir = ttime_ptr->Link_Dir ();
					}

					//---- look for a link ----

					index = link_list.Get_Index (dir);

					if (index != 0) {
						link_rec.Dir_Index (index);
						link_rec.Time_Period (Resolve (tod) / increment + 1);

						if (!travel_ptr->Add_Dir (&link_rec)) goto dir_error;
					}
					tod += time;
				}
			}

		} else {

			link_rec.Dir_Index (1);
			link_rec.Time_Period (Resolve (tod) / increment + 1);

			if (!travel_ptr->Add_Dir (&link_rec)) goto dir_error;
		}

		//---- add the plan to the array ----

		if (travel_ptr->First () > 0) {
			travel_ptr->Traveler (traveler);
			travel_ptr->Change (0);
			travel_ptr->Optimize ();

			tod = Resolve (start) / increment;
			if (tod > max_change) tod = max_change;

			travel_ptr->Min_Change (-tod);

			tod = (MIDNIGHT - Resolve (start)) / increment;
			if (tod > max_change) tod = max_change;
			if (tod < 0) tod = 0;

			travel_ptr->Max_Change (tod);

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

dir_error:
	Error ("Adding Link to the Plan for Traveler %d", plan_file.Traveler ());
}
