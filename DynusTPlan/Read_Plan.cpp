//*********************************************************
//	Read_Plan.cpp - Read Each Plan File
//*********************************************************

#include "DynusTPlan.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void DynusTPlan::Read_Plan (void)
{
	int i, *path, num_path, anode, bnode;
	int type, occ, nodes, stops, start, info, origin, destination [10], duration [10];
	double band, rate;
	bool time_flag, veh_type_flag;

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Parking_Data *parking_ptr;
	Vehicle_Data *veh_ptr;
	Veh_Type_Data *type_ptr;

	//---- set the filename and extension ----

	if (plan_file.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	//---- read each plan record ----

	time_flag = (time_range.Num_Ranges () > 0);
	path = NULL;
	num_path = 0;

	band = rate = 0.0;
	stops = 1;
	origin = 0;
	duration [0] = 0;
	type = 1;
	occ = 1;
	info = 0;

	veh_type_flag = (Demand_File_Flag (VEHICLE) && Demand_File_Flag (VEHICLE_TYPE));

	while (plan_file.Read ()) {
		Show_Progress ();

		if (plan_file.Start_Type () != PARKING_ID || plan_file.End_Type () != PARKING_ID) continue;

		if (time_flag) {
			if (!time_range.Period (plan_file.Time ())) continue;
		}
		if (random.Probability () > percent) continue;

		//---- get the vehicle type ----

		if (veh_type_flag) {
			veh_ptr = vehicle_data.Get (plan_file.Vehicle ());

			if (veh_ptr != NULL) {
				type_ptr = veh_type_data.Get (veh_ptr->Type (), veh_ptr->Sub_Type ());

				if (type_ptr != NULL) {
					type = type_ptr->Use ();

					if (type == TRUCK || type == LIGHTTRUCK || type == HEAVYTRUCK) {
						type = 2;
					} else if (type == HOV2 || type == HOV3 || type == HOV4 || type == TAXI) {
						type = 3;
					} else {
						type = 1;
					}
				}
			}
		}
		num_out++;

		path = plan_file.Path (&num_path);

		//---- get the destination zone ----

		destination [0] = 0;

		if (num_path > 0) {
			anode = path [num_path-1];

			if (type_flag) {
				if (num_path > 0) {
					parking_ptr = parking_data.Get (plan_file.End_ID ());

					if (parking_ptr != NULL) {
						link_ptr = link_data.Get (parking_ptr->Link ());

						if (link_ptr != NULL) {
							if (link_ptr->Anode () == anode) {
								bnode = link_ptr->Bnode ();
						} else if (link_ptr->Bnode () == anode) {
								bnode = link_ptr->Anode ();
							}
							node_ptr = node_data.Get (bnode);
							destination [0] = node_zone [node_data.Record_Index ()];
						}
					}
				}
			} else {
				link_ptr = link_data.Get (abs (anode));

				if (link_ptr != NULL) {
					if (anode > 0) {
						bnode = link_ptr->Bnode ();
					} else {
						bnode = link_ptr->Anode ();
					}
				}
				node_ptr = node_data.Get (bnode);
				destination [0] = node_zone [node_data.Record_Index ()];
			}
		}

		//---- get the origin ----

		origin = 0;

		if (type_flag) {
			parking_ptr = parking_data.Get (plan_file.Start_ID ());

			if (parking_ptr != NULL) {
				link_ptr = link_data.Get (parking_ptr->Link ());

				if (link_ptr != NULL) {
					if (link_ptr->Anode () == *path) {
						anode = link_ptr->Bnode ();
						bnode = link_ptr->Anode ();
					} else if (link_ptr->Bnode () == *path) {
						anode = link_ptr->Anode ();
						bnode = link_ptr->Bnode ();
					}
					node_ptr = node_data.Get (anode);
					origin = node_zone [node_data.Record_Index ()];
				}
			}
			nodes = num_path + 2;
		} else {
			link_ptr = link_data.Get (abs (*path));

			if (link_ptr != NULL) {
				if (*path > 0) {
					anode = link_ptr->Anode ();
					bnode = link_ptr->Bnode ();
				} else {
					anode = link_ptr->Bnode ();
					bnode = link_ptr->Anode ();
				}
				node_ptr = node_data.Get (anode);
				origin = node_zone [node_data.Record_Index ()];
			}
			nodes = num_path + 1;
		}
		start = plan_file.Time () - start_time;

		node_ptr = node_data.Get (anode);
		origin = node_zone [node_data.Record_Index ()];
		
		//---- print the vehicle record (limit start times to 1 decimal place) ----

		fprintf (veh_file.File (), "%7d%7d%7d%7.1lf0%6d%6d%6d%6d%6d%6d%8.4lf%8.4lf%5d\n",
			num_out, anode, bnode, start / 60.0, user_class, type, occ, nodes, stops, info, band, rate, origin);

		for (i=0; i < stops; i++) {
			fprintf (veh_file.File (), "%12d%7.2lf\n", destination [i], duration [i] / 60.0);
		}
		fprintf (path_file.File (), "%7d", anode);

		while (num_path-- > 0) {
			if (type_flag) {
				bnode = *path++;

			} else {
				link_ptr = link_data.Get (abs (*path));

				if (link_ptr != NULL) {
					if (*path > 0) {
						bnode = link_ptr->Bnode ();
					} else {
						bnode = link_ptr->Anode ();
					}
				}
				path++;
			}
			fprintf (path_file.File (), "%7d", bnode);
		}
		if (type_flag) {
			parking_ptr = parking_data.Get (plan_file.End_ID ());

			if (parking_ptr != NULL) {
				link_ptr = link_data.Get (parking_ptr->Link ());

				if (link_ptr != NULL) {
					path--;
					if (link_ptr->Anode () == *path) {
						bnode = link_ptr->Bnode ();
					} else if (link_ptr->Bnode () == *path) {
						bnode = link_ptr->Anode ();
					}
				}
				fprintf (path_file.File (), "%7d\n", bnode);
			}
		} else {
			fprintf (path_file.File (), "\n");
		}
	}
	End_Progress ();

	plan_file.Close ();
}
