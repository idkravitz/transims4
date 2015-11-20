//*********************************************************
//	Stop_Stats.cpp - Stop Group Statistics
//*********************************************************

#include "Validate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Stop_Group_Stats
//---------------------------------------------------------

void Validate::Stop_Group_Stats (int type)
{
	int i, group, run, stop, stops, time, s, riders, on, off, group_field, count_field;
	int sum_lines, sum_runs, tot_lines, tot_runs;
	double volume, count, error;
	char *str_ptr, buffer [20];
	bool first;

	Group_Data *data, data_rec;
	Statistics stats;
	Rider_Data *rider_ptr;
	Integer_List *list;

	//---- read the count data ----

	group_field = stop_count_file.Required_Field ("GROUP", "STOP", "STOPGROUP", "STOPGRP", "ID");

	if (type == STOP_GROUP) {
		count_field = stop_count_file.Required_Field ("TOTAL",  "ONOFF", "ON_OFF", "PASSENGERS", "COUNT");
	} else if (type == BOARD_GROUP) {
		count_field = stop_count_file.Required_Field ("BOARDINGS", "BOARD", "ON", "PASSENGERS", "COUNT");
	} else if (type == ALIGHT_GROUP) {
		count_field = stop_count_file.Required_Field ("ALIGHTINGS", "ALIGHT", "OFF", "PASSENGERS", "COUNT");
	}
	stop_count_file.Rewind ();

	while (stop_count_file.Read ()) {

		stop_count_file.Get_Field (group_field, &group);
		if (group <= 0) continue;

		stop_count_file.Get_Field (count_field, &riders);

		data = (Group_Data *) group_data.Get (&group);

		if (data == NULL) {
			memset (&data_rec, '\0', sizeof (Group_Data));

			data_rec.index = group;
			if (!group_data.Add (&data_rec)) {
				Error ("Adding Group %d to Group Data", group);
			}
			data = (Group_Data *) group_data.Record ();
		}
		data->count += riders;
	}

	//---- print the page header ----
	
	Header_Number (type);

	if (!Break_Check (group_data.Num_Records () + 11)) {
		Print (1);
		Stop_Group_Header (type);
	}
	tot_runs = tot_lines = 0;

	//---- print each line group ----

	for (data = (Group_Data *) group_data.First_Key (); data; data = (Group_Data *) group_data.Next_Key ()) {

		if (data->count == 0) continue;	

		group = data->index;

		list = stop_equiv.Group_List (group);
		if (list == NULL) continue;

		riders = sum_runs = sum_lines = 0;

		//---- check routes for stops ----

		for (rider_ptr = rider_data.First (); rider_ptr; rider_ptr = rider_data.Next ()) {

			stops = rider_ptr->Stops ();
			first = true;

			//---- check the stop ----

			for (i=1; i <= stops; i++) {

				stop = rider_ptr->Stop (i);

				//---- check for stops in the stop group ----

				for (s = list->First (); s != 0; s = list->Next ()) {
					if (s == stop) break;
				}
				if (s == 0) continue;
				
				//---- process run ----

				for (run = 1; run <= rider_ptr->Runs (); run++) {

					//---- check time period ----

					time = rider_ptr->Schedule (run, i);

					if (!time_periods.In_Range (Resolve (time))) continue;

					//---- sum stop ons/offs ----

					on = rider_ptr->Board (run, i);
					off = rider_ptr->Alight (run, i);

					if (type == STOP_GROUP) {
						riders += on + off;
					} else if (type == BOARD_GROUP) {
						riders += on;
					} else {
						riders += off;
					}
					sum_runs++;
					if (first) {
						sum_lines++;
						first = false;
					}
				}
			}
		}
		volume = riders;
		count = data->count;
		error = fabs (volume - count);

		data->count = 0;

		//---- sum to category ----

		data->number++;

		data->volume += volume;
		data->volume_sq += volume * volume;

		data->count += count;
		data->count_sq += count * count;

		data->error += error;
		data->error_sq += error * error;
		
		data->count_volume += count * volume;

		//---- add to total as well ----

		total.number++;
		tot_runs += sum_runs;
		tot_lines += sum_lines;

		total.volume += volume;
		total.volume_sq += volume * volume;

		total.count += count;
		total.count_sq += count * count;

		total.error += error;
		total.error_sq += error * error;
		
		total.count_volume += count * volume;

		//---- print the group data ----

		Calc_Stats (data, &stats);
		
		str_ptr = stop_equiv.Group_Label (group);

		if (str_ptr == NULL) {
			str_ptr = str_fmt (buffer, sizeof (buffer), "Line Group %d", group);
		}

		Page_Check (1);
		Print (0, "\n%-5d %-32.32s ", data->index, str_ptr);
		Print (0, "%6d%8d%10.0lf%10.0lf%9.0lf %7.1lf",
			sum_lines, sum_runs, data->volume, data->count, 
			(data->volume - data->count), stats.per_diff);
	}
		
	//---- calculate the total statistics ----
	
	if (total.number > 0) {
		Calc_Stats (&total, &stats);
		
		Page_Check (5);
		Print (0, "\n\nTOTAL%34c", BLANK);
		Print (0, "%6d%8d%10.0lf%10.0lf%9.0lf %7.1lf\n",
			tot_lines, tot_runs, total.volume, total.count, 
			(total.volume - total.count), stats.per_diff);

		Print (0, "\n      Avg.Abs.Error = %.0lf (%.1lf%%)   Std.Dev. = %.0lf   RMSE = %.1lf%%   R Sq. = %.3lf\n",
			stats.avg_error, stats.per_error, stats.std_dev, stats.rmse, stats.r_sq);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Group_Header
//---------------------------------------------------------

void Validate::Stop_Group_Header (int type)
{
	Page_Check (5);
	Print (0, "\n%30cTransit Summary Statistics by ", BLANK);
	if (type == STOP_GROUP) {
		Print (0, "Stop Group\n");
	} else if (type == BOARD_GROUP) {
		Print (0, "Boarding Group\n");
	} else {
		Print (0, "Alighting Group\n");
	}
	Print (0, "\n%41cNum.    Num.  ------Riders------ ---Difference---  ", BLANK);
	Print (0, "\nGroup Description%23cLines    Runs  Estimate  Observed   Riders      %%\n", BLANK);
}

/**********************************************************|***********************************************************

                         Transit Summary Statistics by Stop Group
									
                                         Num.    Num.  ------Riders------ ---Difference---
Group Description                       Lines    Runs  Estimate  Observed   Riders     %    

ddddd ssssssssssssssssssssssssssssssss dddddd ddddddd lllllllll lllllllll llllllll fffff.f

TOTAL                                  dddddd ddddddd lllllllll lllllllll llllllll fffff.f

Avg.Abs.Error = ddddd (fff.f%)   Std.Dev. = dddddd   RMSE = ffff.f%   R Sq. = f.fff

***********************************************************|***********************************************************/ 
