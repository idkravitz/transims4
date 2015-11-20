//*********************************************************
//	Line_Stats.cpp - Line Group Statistics
//*********************************************************

#include "Validate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Line_Group_Stats
//---------------------------------------------------------

void Validate::Line_Group_Stats (void)
{
	int group, run, stop, stops, time, line, riders, group_field, count_field;
	int sum_runs, sum_lines, tot_runs, tot_lines;
	double volume, count, error;
	char *str_ptr, buffer [20];
	bool first;

	Group_Data *data, data_rec;
	Statistics stats;
	Rider_Data *rider_ptr;
	Integer_List *list;

	//---- read the count data ----

	group_field = line_count_file.Required_Field ("GROUP", "LINE", "LINEGROUP", "LINEGRP", "ID");
	count_field = line_count_file.Required_Field ("COUNT", "RIDERS", "PASSENGERS", "TOTAL", "VOLUME");

	while (line_count_file.Read ()) {

		line_count_file.Get_Field (group_field, &group);
		if (group <= 0) continue;

		line_count_file.Get_Field (count_field, &riders);

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
	
	Header_Number (LINE_GROUP);

	if (!Break_Check (group_data.Num_Records () + 11)) {
		Print (1);
		Line_Group_Header ();
	}

	//---- print each line group ----

	tot_runs = tot_lines = 0;

	for (data = (Group_Data *) group_data.First_Key (); data; data = (Group_Data *) group_data.Next_Key ()) {
	
		if (data->count == 0) continue;

		group = data->index;

		list = line_equiv.Group_List (group);
		if (list == NULL) continue;

		riders = sum_runs = sum_lines = 0;

		for (line = list->First (); line != 0; line = list->Next ()) {

			rider_ptr = rider_data.Get (line);
			if (rider_ptr == NULL) continue;

			stops = rider_ptr->Stops ();
			first = true;
			
			//---- process run ----

			for (run = 1; run <= rider_ptr->Runs (); run++) {

				//---- check time period ----

				time = (rider_ptr->Schedule (run, stops) + rider_ptr->Schedule (run, 1)) / 2;

				if (!time_periods.In_Range (Resolve (time))) continue;

				//---- sum boardings ----

				for (stop = 1; stop <= stops; stop++) {
					riders += rider_ptr->Board (run, stop);
				}
				sum_runs++;
				if (first) {
					sum_lines++;
					first = false;
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
		
		str_ptr = line_equiv.Group_Label (group);

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
//	Line_Group_Header
//---------------------------------------------------------

void Validate::Line_Group_Header (void)
{
	Page_Check (5);
	Print (0, "\n%30cSummary Statistics by Line Group\n", BLANK);
	Print (0, "\n%41cNum.    Num.  ------Riders------ ---Difference---  ", BLANK);
	Print (0, "\nGroup Description%23cLines    Runs  Estimate  Observed   Riders      %%\n", BLANK);
}

/**********************************************************|***********************************************************

                         Transit Summary Statistics by Line Group
									
                                   Num.    Num.  ------Riders------ ---Difference---
Line Group                        Lines    Runs  Estimate  Observed   Riders     %    

ssssssssssssssssssssssssssssssss dddddd ddddddd lllllllll lllllllll llllllll fffff.f

TOTAL                            dddddd ddddddd lllllllll lllllllll llllllll fffff.f

Avg.Abs.Error = ddddddd (fff.f%)   Std.Dev. = ddddddd   RMSE = ffff.f%   R Sq. = f.fff

***********************************************************|***********************************************************/ 
