//*********************************************************
//	Set_Offset.cpp - Set the Signal Offsets
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Set_Offset
//---------------------------------------------------------

int Progression::Set_Offset (int group, bool report_flag, bool eval_flag)
{
	int i, ttime, offset, phase, green, thru_green, next, cycle, order;
	int time, tot_time, length, hours, delay, performance;
	double num_thru, max_thru, max_green, speed, percent, sum_thru;
	double weight, *wt, tot_weight;
	bool first;

	Group_Data *group_ptr, *last_ptr, *next_ptr;
	Group_Link *link_ptr;
	Group_Weight *wt_ptr;
	Signal_Offset *signal_ptr;

	//---- initialize the offsets ----

	for (signal_ptr = signal_offset.First (); signal_ptr; signal_ptr = signal_offset.Next ()) {
		if (signal_ptr->Period () == period) {
			if (eval_flag) {
				signal_ptr->Offset (signal_ptr->Input ());
			} else if (signal_ptr->Preset () > 0) {
				signal_ptr->Offset (signal_ptr->Preset ());
			} else {
				signal_ptr->Offset (-1);
			}
		} else if (signal_ptr->Period () == 0) {
			signal_ptr->Offset (signal_ptr->Preset ());
		}
	}

	//---- initialize the next group ----

	for (group_ptr = group_data.First (); group_ptr; group_ptr = group_data.Next ()) {
		if (group_data.Record_Index () == group) {
			last_ptr = group_ptr;
			group_ptr->Next_Group (0);
		} else {
			group_ptr->Next_Group (-1);
		}
	}
	num_thru = max_thru = weight = tot_weight = 0.0;
	order = 1;

	//---- process the signals in each group ----

	while (group) {
		group_ptr = group_data [group];

		if (group_ptr == NULL) break;

		//---- get the group period weight ----

		weight = 1.0;

		if (weight_flag) {
			wt_ptr = weight_data.Get (group_ptr->Group ());

			if (wt_ptr != NULL) {
				wt = wt_ptr->Best_Weight (period);
				if (wt != NULL) {
					weight = *wt;
				}
			}
		}

		if (report_flag) {
			Print (2, "**** Period %d, Group %d ", period, group_ptr->Group ());

			if (equiv_flag) {
				Print (0, "[%s] ", link_equiv.Group_Label (group_ptr->Group ()));
			}
			if (weight != 1.0) {
				Print (0, "Weight %.2lf ", weight);
			}
			Print (0, "****");
		}

		//---- find the first previously set offset ----

		offset = phase = 0;
		green = 1;

		for (link_ptr = group_ptr->First_Link (); link_ptr; link_ptr = group_ptr->Next_Link ()) {
			signal_ptr = signal_offset [link_ptr->Signal ()];

			offset += phase;
			phase = link_ptr->Offset ();
			ttime = link_ptr->TTime ();
			cycle = signal_ptr->Cycle ();

			if (ttime != 0) {
				offset += ttime - phase;
				if (offset < 0) offset += cycle;
				offset %= cycle;
			} else {
				green = link_ptr->Green ();
			}

			if (signal_ptr->Offset () >= 0) {
				offset = signal_ptr->Offset () - offset;
				if (offset < 0) offset += cycle;
				offset %= cycle;
				break;
			}	
		}
		if (link_ptr == NULL) {
			offset = 0;
		}
		phase = tot_time = length = hours = 0;
		percent = sum_thru = 0.0;

		//---- set the offset for each signal in the group ----
		
		for (link_ptr = group_ptr->First_Link (); link_ptr; link_ptr = group_ptr->Next_Link ()) {
			signal_ptr = signal_offset [link_ptr->Signal ()];

			offset += phase;
			phase = link_ptr->Offset ();
			ttime = link_ptr->TTime ();
			cycle = signal_ptr->Cycle ();

			if (ttime != 0) {
				offset += ttime - phase;
				if (offset < 0) offset += cycle;
				offset %= cycle;
			}
			delay = offset;

			if (link_ptr->Green () >= green) {
				max_green = 1.0;
			} else {
				max_green = (double) link_ptr->Green () / green;
			}
			max_thru += max_green;
			sum_thru += max_green;

			if (report_flag) Print (1, "Node=%8d, Arrival=%3d", link_ptr->Node (), offset);

			if (signal_ptr->Offset () < 0) {
                num_thru += max_green;
				signal_ptr->Offset (offset);
				if (report_flag) Print (0, ",  SET");
			} else if (offset == signal_ptr->Offset ()) {
				num_thru += max_green;
				if (report_flag) Print (0, ", THRU");
			} else {
				thru_green = 0;

				if ((offset + green) > cycle) {
					if ((offset + green) > (signal_ptr->Offset () + cycle) &&
						offset < (signal_ptr->Offset () + cycle + link_ptr->Green ())) {

						thru_green = MIN ((offset + green), 
							(signal_ptr->Offset () + cycle + link_ptr->Green ())) -
							MAX (offset, (signal_ptr->Offset () + cycle));
					}
				} else {
					if ((offset + green) > signal_ptr->Offset () && 
						offset < (signal_ptr->Offset () + link_ptr->Green ())) {
						
						thru_green = MIN ((offset + green), (signal_ptr->Offset () + link_ptr->Green ())) - 
							MAX (offset, signal_ptr->Offset ());
					}
				}

				if (thru_green > 0) {
					if (thru_green > green) {
						num_thru += max_green = 1.0;
					} else {
						num_thru += max_green = (double) thru_green / green;
					}
					if (report_flag) Print (0, ", %3d%%", (int) (max_green * 100 + 0.5));
				} else {
					if (report_flag) Print (0, ", STOP");
					max_green = 0.0;
				}
				offset = signal_ptr->Offset ();
			}
			green = link_ptr->Green ();

			delay -= signal_ptr->Offset ();
			if (delay < 0) delay += cycle;

			percent += max_green;

			tot_time += time = (int) (ttime + (1.0 - max_green) * delay + 0.5);
			length += link_ptr->Length ();

			if (ttime > 0) {
				hours += (int) ((double) link_ptr->VHT () * time / ttime + 0.5);
			} else {
				hours += link_ptr->VHT ();
			}
			if (report_flag) Print (0, ", Departure=%3d", signal_ptr->Offset ());

			//---- add crossing groups to the list ----

			first = true;

			for (next = signal_ptr->First_Group (); next != 0; next = signal_ptr->Next_Group ()) {
				if (next != group) {
					next_ptr = group_data [next];

					if (next_ptr->Next_Group () < 0) {
						last_ptr->Next_Group (next);
						next_ptr->Next_Group (0);
						last_ptr = next_ptr;
					}
                    if (report_flag) {
						if (first) {
							Print (0, ", Intersect= %d", next_ptr->Group ());
							first = false;
						} else {
							Print (0, ", %d", next_ptr->Group ());
						}
					}
				}
			}
		}

		//---- weight the performance measure ----

		if (method == PERCENT_THRU) {
			if (sum_thru == 0.0) sum_thru = 1.0;
			tot_weight += 1000.0 * weight * percent / sum_thru;
		} else if (method == MAX_SPEED) {
			tot_weight -= tot_time * weight;			
		} else {
			tot_weight -= hours * weight;
		}

		//---- save the shape record ----

		if (report_flag && arcview_flag && group_ptr->points.Num_Points () > 1) {
			arcview_file.Put_Field (period_fld, period);
			arcview_file.Put_Field (order_fld, order);
			arcview_file.Put_Field (group_fld, group_ptr->Group ());
		
			if (sum_thru == 0.0) sum_thru = 1.0;
			percent = percent * 100.0 / sum_thru;

			arcview_file.Put_Field (percent_fld, percent);
			arcview_file.Put_Field (time_fld, tot_time);
			arcview_file.Put_Field (length_fld, Resolve (length));

			if (tot_time > 0) {
				speed = UnRound (length / tot_time);
			} else {
				speed = 0.0;
			}
			arcview_file.Put_Field (speed_fld, speed);
			arcview_file.Put_Field (vht_fld, hours);

			arcview_file.parts.Reset ();
			arcview_file.points.Reset ();

			arcview_file.parts.Add (0);

			for (i=1; i <= group_ptr->points.Num_Points (); i++) {
				if (!arcview_file.points.Add (group_ptr->points [i])) {
					Error ("Insufficient Memory for Shape Points");
				}
			}
			if (!arcview_file.Write_Record ()) {
				Error ("Writing ArcView Progression File");
			}
		}

		//---- get the next group in the list ----

		order++;

		if (order_flag) {
			group++;
		} else {
			group = group_ptr->Next_Group ();

			if (!group) {
				for (group_ptr = group_data.First (); group_ptr; group_ptr = group_data.Next ()) {
					if (group_ptr->Next_Group () < 0) {
						group_ptr->Next_Group (0);
						last_ptr = group_ptr;
						group = group_data.Record_Index ();
						break;
					}
				}
			}
		}
	}

	//---- performance measure ----

	if (tot_weight > 0.0) {
		performance = (int) (tot_weight + 0.5);
	} else {
		performance = (int) (tot_weight - 0.5);
	}
	if (report_flag) {
		if (max_thru == 0.0) max_thru = 1.0;
		if (eval_flag) {
			old_thru += (int) (num_thru / max_thru * 1000.0  + 0.5);
		} else {
			tot_thru += (int) (num_thru / max_thru * 1000.0  + 0.5);
		}
		Print (2, "Period %d Performance Measure = %d", period, abs (performance));
	}
	return (performance);
}
