//*********************************************************
//	Write_Intersection.cpp - write the arcview intersection file
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Write_Intersection
//---------------------------------------------------------

void ArcDelay::Write_Intersection (void)
{
	int i, dir, end, period, nperiods, vol, volume, from, to, enter, exit, max_queue, cycle_failure;
	double ttime, speed, density, delay, max_density, time_ratio, queue, vmt, vht, max;
	char start_time [FIELD_BUFFER], end_time [FIELD_BUFFER];
	char buffer [STRING_BUFFER];
	bool version3_flag;

	Node_List *node_ptr;
	List_Data *list_ptr;
	Link_Data *link_ptr;
	Performance_Data *perf_ptr;
	Range_Data *range_ptr;

	XYZ_Point *point = arcview_node.points [1];

	version3_flag = (delay_file->Dbase_Format () == VERSION3);

	//---- initialize the list data arrays ----

	dir_list.Num_Records (performance_data.Num_Records ());

	for (list_ptr = dir_list.First (); list_ptr; list_ptr = dir_list.Next ()) {
		list_ptr->From_List (0);
		list_ptr->To_List (0);

	}
	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		node_ptr->From_Dir (0);
		node_ptr->To_Dir (0);

	}

	//---- build the list data arrays ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		for (i=0; i < 2; i++) {
			if (i == 1) {
				dir = link_ptr->BA_Dir ();
				from = link_ptr->Bnode ();
				to = link_ptr->Anode ();
			} else {
				dir = link_ptr->AB_Dir ();
				from = link_ptr->Anode ();
				to = link_ptr->Bnode ();
			}
			if (dir == 0) continue;

			list_ptr = dir_list [dir];
			if (list_ptr == NULL) continue;

			//---- process the from node -----

			node_ptr = node_data.Get (from);
			if (node_ptr == NULL) continue;

			list_ptr->From_List (node_ptr->From_Dir ());
			node_ptr->From_Dir (dir);

			//---- process the to node ----

			node_ptr = node_data.Get (to);
			if (node_ptr == NULL) continue;

			list_ptr->To_List (node_ptr->To_Dir ());
			node_ptr->To_Dir (dir);
		}
	}

	//---- process each time period ----

	nperiods = time_period.Num_Ranges ();

	for (period = 1; period <= nperiods; period++) {

		range_ptr = time_period [period];
		end = range_ptr->High () + 1;

		str_cpy (start_time, sizeof (start_time), time_period.Format_Step (range_ptr->Low ()));
		str_cpy (end_time, sizeof (end_time), time_period.Format_Step (end));

		//---- create a new shape file ----

		str_fmt (buffer, sizeof (buffer), "%s_%s.shp", nodename, time_period.Range_Label (period));
		Print (1);

		if (!arcview_node.Open (buffer)) {
			File_Error ("Opening ArcView Intersection File", arcview_node.Shape_Filename ());
		}
		arcview_node.Write_Header ();

		Show_Message ("Writing %s %s -- Node", arcview_node.File_Type (), end_time);
		Set_Progress (1000);

		//---- process each node ----

		for (node_ptr = node_data.First_Key (); node_ptr; node_ptr = node_data.Next_Key ()) {
			Show_Progress ();

			point->x = UnRound (node_ptr->X ());
			point->y = UnRound (node_ptr->Y ());
			point->z = UnRound (node_ptr->Z ());

			arcview_node.points.Set ();

			volume = enter = exit = max_queue = cycle_failure = from = to = 0;
			ttime = speed = density = delay = max_density = time_ratio = queue = vmt = vht = 0.0;

			//---- progress the departure links ----

			for (dir = node_ptr->From_Dir (); dir; dir = list_ptr->From_List ()) {
				list_ptr = dir_list [dir];
				from++;

				perf_ptr = performance_data [dir];
				if (perf_ptr == NULL || perf_ptr->Periods () == 0) continue;

				exit += perf_ptr->Exit (period);
			}

			//---- process the arrival links ----

			for (dir = node_ptr->To_Dir (); dir; dir = list_ptr->To_List ()) {
				list_ptr = dir_list [dir];
				to++;

				perf_ptr = performance_data [dir];
				if (perf_ptr == NULL || perf_ptr->Periods () == 0) continue;

				//---- calculate the performance ----

				vol = perf_ptr->Volume (period);
				volume += vol;
				ttime += vol * perf_ptr->TTime (period);
				speed += vol * perf_ptr->Avg_Speed (period);
				density += perf_ptr->Density (period);

				enter += perf_ptr->Enter (period);
				delay += perf_ptr->Delay (period);

				max = perf_ptr->Max_Density (period);
				if (max > max_density) max_density = max;

				time_ratio += vol * perf_ptr->Time_Ratio (period);
				queue += perf_ptr->Queue (period);
				max_queue += perf_ptr->Max_Queue (period);
				cycle_failure += perf_ptr->Failure (period);

				vmt += perf_ptr->Distance (period);
				vht += perf_ptr->Time (period);
			}

			if (to == 0 && from == 0) continue;

			//---- normalize the data ----

			if (volume > 0) {
				ttime = ttime / volume;
				speed = speed / volume;
				density = density / to;
				time_ratio = time_ratio / volume;
			}
			volume = (enter + exit + 1) / 2;

			//---- save the Version 3 fields ----

			if (version3_flag) {
				delay_file->Time (end);
				delay_file->Lane (1);
				delay_file->Turn (0);

				delay_file->Volume (volume);
				delay_file->TTime (volume * ttime);
				delay_file->TTime2 (volume * ttime * ttime);

				delay_file->Vehicles ((int) (density + 0.5));
				delay_file->Speed (density * speed);
				delay_file->Speed2 (density * speed * speed);

			} else {

				delay_file->Start_Time (start_time);
				delay_file->End_Time (end_time);

				delay_file->Volume (volume);
				delay_file->Enter (enter);
				delay_file->Exit (exit);

				delay_file->Speed (speed);
				delay_file->TTime (ttime);
				delay_file->Delay (delay);

				delay_file->Density (density);
				delay_file->Max_Density (max_density);

				delay_file->Time_Ratio (time_ratio);
				delay_file->Avg_Queue (queue);
				delay_file->Max_Queue (max_queue);
				delay_file->Cycle_Failure (cycle_failure);

				delay_file->VMT (vmt);
				delay_file->VHT (vht);
			}
			delay_file->Num_Connect ((from + to + 1) / 2);

			arcview_node.Copy_Fields (delay_file);
			arcview_node.Put_Field (1, node_ptr->Node ());

			//---- save the link record ----

			if (!arcview_node.Write_Record ()) {
				Error ("Writing ArcView Delay File");
			}
		}
		End_Progress ();
		arcview_node.Close ();
	}
}
