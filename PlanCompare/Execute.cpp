//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanCompare::Execute (void)
{
	int i, nfile, num, max_num;
	char *labels [] = { "TRIPS", "WALK", "DRIVE", "TRANSIT", "COST" };

	Travel_Diff *diff_ptr;
	Location_Diff *loc_ptr;
	Period_Diff *period_ptr;

	//---- read the network ----

	Demand_Service::Execute ();

	if (compare_loc_flag) {
		Read_Location ();
	}
	//---- read the household list ----

	if (hhlist_flag) {
		Read_HHList ();
	}

	//---- read the trip files ----

	if (input_trips.Is_Open ()) {
		Read_Trips (&input_trips, &input_trip_data);
	}
	if (compare_trips.Is_Open ()) {
		Read_Trips (&compare_trips, &compare_trip_data);
	}

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!input_plans.Open (nfile)) break;

		if (!compare_plans.Open (nfile)) {
			File_Error ("Compare Plan File was Not Found", compare_plans.Filename ());
		}

		//---- process the plan files ----

		Compare_Plans ();
	}

	if (nfile == 0) {
		File_Error ("No Plan Files were Found", input_plans.Filename ());
	}
	if (diff_file_flag) {
		diff_file.Close ();
	}
	if (start_flag) {
		Show_Message ("Writing Start Difference File -- Record");
		Set_Progress (10000);

		for (loc_ptr = (Location_Diff *) start_diff.First_Key (); loc_ptr; 
			loc_ptr = (Location_Diff *) start_diff.Next_Key ()) {
			Show_Progress ();

			fprintf (start_file.File (), "%d\t%d\t%d\t%.1lf\t%.1lf\t%.1lf\n",
				loc_ptr->location, loc_ptr->type, loc_ptr->count, 
				loc_ptr->input, loc_ptr->compare, loc_ptr->diff);
		}
		End_Progress ();
		start_file.Close ();
	}
	if (end_flag) {
		Show_Message ("Writing End Difference File -- Record");
		Set_Progress (10000);

		for (loc_ptr = (Location_Diff *) end_diff.First_Key (); loc_ptr; 
			loc_ptr = (Location_Diff *) end_diff.Next_Key ()) {
			Show_Progress ();

			fprintf (end_file.File (), "%d\t%d\t%d\t%.1lf\t%.1lf\t%.1lf\n",
				loc_ptr->location, loc_ptr->type, loc_ptr->count,
				loc_ptr->input, loc_ptr->compare, loc_ptr->diff);
		}
		End_Progress ();
		end_file.Close ();
	}
	if (period_diff_flag) {
		int count;
		double input, compare, diff;

		count = 0;
		input = compare = diff = 0;

		for (i=1; i <= num_inc; i++) {
			period_ptr = (Period_Diff *) period_diff.Record (i);

			count += period_ptr->count;
			input += period_ptr->input;
			compare += period_ptr->compare;
			diff += period_ptr->diff;

			fprintf (period_file.File (), "%d\t%d\t%.0lf\t%.0lf\t%.0lf\t%.4lf\n", i, period_ptr->count,
				period_ptr->input, period_ptr->compare, period_ptr->diff, 
				((period_ptr->input > 0) ? (-period_ptr->diff / period_ptr->input) : 0.0));
		}
		fprintf (period_file.File (), "Total\t%d\t%.0lf\t%.0lf\t%.0lf\t%.4lf\n", count,
			input, compare, diff, ((input > 0) ? (-diff / input) : 0.0));
	}

	//---- write summary statistics ----

	Break_Check (6);

	Print (2, "Number of Plan Files = %d", nfile);
	Print (1, "Number of Input Plans = %d", input_plans.Num_Plans ());
	Print (1, "Number of Input Records = %d", input_plans.Num_Records ());
	Print (1, "Number of Input Travelers = %d", input_plans.Num_Travelers ());
	Print (1, "Number of Input Trips = %d", input_plans.Num_Trips ());

	Break_Check (5);

	Print (2, "Number of Compare Plans = %d", compare_plans.Num_Plans ());
	Print (1, "Number of Compare Records = %d", compare_plans.Num_Records ());
	Print (1, "Number of Compare Travelers = %d", compare_plans.Num_Travelers ());
	Print (1, "Number of Compare Trips = %d", compare_plans.Num_Trips ());

	Break_Check (4);

	num = in_only + compare_only + num_matched;
	if (num == 0) num = 1;

	Print (2, "Number of Trips in Both Plan Files = %d (%.1lf%%)", 
		num_matched, (100.0 * num_matched / num));
	Print (1, "Number of Trips Only in the Input Plan File = %d (%.1lf%%)", 
		in_only, (100.0 * in_only / num));
	Print (1, "Number of Trips Only in the Compare Plan File = %d (%.1lf%%)", 
		compare_only, (100.0 * compare_only / num));

	Break_Check (5);

	if (num_matched == 0) num_matched = 1;

	Write (2, "Number of Trips Compared = %d (%.1lf%%)", 
		num_compared, (100.0 * num_compared / num_matched));

	if (num_compared == 0) num_compared = 1;

	Print (1, "Number of Trips Skipped as Incomplete = %d", num_bad);
	Print (1, "Number of Trips Compared using Complete Plans = %d (%.1lf%%)", 
		num_complete, (100.0 * num_complete / num_compared));
	Print (1, "Number of Trips Compared using Only Drive Legs = %d (%.1lf%%)", 
		num_drive, (100.0 * num_drive / num_compared));

	Print (1);
	if (sig_tot == 0.0) sig_tot = 1.0;

	Write (1, "Number of Trips with Nearly Equal Plans = %d (%.1lf%%)", 
		num_equal, (100.0 * num_equal / num_compared));
	Print (1, "Total Significant Time Difference = %.1lf hours (%.3lf%%)", 
		sig_diff / 3600.0, 100.0 * sig_diff / sig_tot);

	num_equal = num_compared - num_equal;
	if (num_equal <= 0) num_equal = 1;
	if (tot_base == 0.0) tot_base = 1.0;

	if (cost_flag) {
		Print (2, "Total Cost Difference for Non-Equal Plans = %.0lf units (%.2lf%%)", 
			abs_diff, 100.0 * abs_diff / tot_base);
		Print (1, "Average Cost Difference for Non-Equal Plans = %.1lf units", 
			abs_diff / num_equal);
	} else {
		Print (2, "Total Time Difference for Non-Equal Plans = %.1lf hours (%.2lf%%)", 
			abs_diff / 3600.0, 100.0 * abs_diff / tot_base);
		Print (1, "Average Time Difference for Non-Equal Plans = %.1lf minutes", 
			abs_diff / (60.0 * num_equal));
	}

	//---- write the distribution file ----

	if (distrib_flag) {
		time_diff.Write_Distribution ();
	}
	if (cost_distrib_flag) {
		cost_diff.Write_Distribution ();
	}
	if (mode_file_flag) {
		mode_diff.Write_Mode_Distribution (5, labels);
	}
	if (mode_data_flag) {
		mode_data.Write_Mode_Distribution (5, labels);
	}

	//---- write the trip time file ----

	if (trip_flag) {
		Write_Trip_Times ();
	}

	//---- print the comparison summary ----

	if (cost_flag) {
		cost_diff.Total_Summary ();
	} else {
		time_diff.Total_Summary ();
	}

	//---- write the output plan file ----

	if (nsel == 0) {
		Write (2, "No Trips were Selected for Output");
		Exit_Stat (DONE);
	}
	num = input_plans.Num_Trips ();
	if (num == 0) num = 1;

	max_num = compare_plans.Num_Trips ();
	if (max_num == 0) max_num = 1;
	if (num > max_num) max_num = num;

	Break_Check (3);

	Print (2, "Number of Trips Selected = %d (%.1lf%%)", 
		nsel, (100.0 * nsel / num));

	if (diff_flag || gap_flag) {
		select_flag = true;
		nsel = (int) (nsel * percent + 0.5);

		//---- check the maximum selection ----

		if (((double) nsel / max_num) > max_percent) {
			nsel = (int) (max_num * max_percent + 0.5);
		}

		//---- find the selection key ----

		min_key.percent = min_key.traveler = min_key.trip = 0;

		for (diff_ptr = (Travel_Diff *) travel_diff.Last_Key (); diff_ptr; 
			diff_ptr = (Travel_Diff *) travel_diff.Previous_Key ()) {

			if (--nsel == 0) {
				min_key.percent = diff_ptr->percent;
				min_key.traveler = diff_ptr->traveler;
				min_key.trip = diff_ptr->trip;
				break;
			}
		}
	} else {

		//---- check the maximum selection ----

		if (nsel > 1 && nsel * percent / max_num > max_percent) {
			select_flag = true;
			percent = max_percent * max_num / nsel;
		}
	}

	//---- output the plans ----

	if (output_flag) {
		Show_Message (1);

		for (nfile=0; ; nfile++) {
			if (!input_plans.Open (nfile)) break;

			if (!output_plans.Open (nfile)) {
				File_Error ("Output Plan File was Not Created", output_plans.Filename ());
			}

			//---- process the plan files ----

			Write_Plans ();
		}
		Print (1, "Number of Trips Written  = %d (%.1lf%%)", 
			nout, (100.0 * nout / max_num));
		Print (1, "Number of Plan Hours Written = %.2lf hours", tot_select / 3600.0);

		if (diff_flag) {
			Print (2, "Total Plan Time Difference   = %.1lf hours", tot_diff / 3600.0);
			Print (1, "Maximum Plan Time Difference = %.2lf minutes", max_diff / 60.0);
			Print (1, "Minimum Plan Time Difference = %.2lf minutes", min_diff / 60.0);
			if (nout == 0) nout = 1;
			Print (1, "Average Plan Time Difference = %.2lf minutes", tot_diff / (nout * 60.0));
		}

		Break_Check (5);

		Write (2, "Number of Output Plans = %d", output_plans.Num_Plans ());
		Write (1, "Number of Output Records = %d", output_plans.Num_Records ());
		Write (1, "Number of Output Travelers = %d", output_plans.Num_Travelers ());
		Write (1, "Number of Output Trips = %d", output_plans.Num_Trips ());
	}

	//---- print reports ----

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case PATH_CHANGE:		//---- percent path change ----
			case TIME_CHANGE:		//---- percent travel time change ---
			case COST_CHANGE:		//---- percent generalized cost change ----
				Change_Report (i);
				break;
			case TOTAL_DISTRIB:		//---- total difference distribution ----
			case TTIME_DISTRIB:		//---- time difference distributions ----
				time_diff.Time_Distribution (i);
				break;
			case PERIOD_SUM:		//---- time period summaries ----
				time_diff.Period_Summary ();
				break;
			case TCOST_DISTRIB:		//---- generalized cost distribution ----
			case COST_DISTRIB:		//---- cost difference distributions ----
				cost_diff.Time_Distribution (i);
				break;
			case COST_SUM:			//---- cost period summaries ----
				cost_diff.Period_Summary ();
				break;
			case TRIP_GAP:			//---- trip gap report ----
				Trip_Gap_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PlanCompare::Page_Header (void)
{
	switch (Header_Number ()) {
		case PATH_CHANGE:
		case TIME_CHANGE:
		case COST_CHANGE:
			Change_Header (Header_Number ());
			break;
		case TOTAL_DISTRIB:		//---- total difference distribution ----
		case TTIME_DISTRIB:		//---- time difference distributions ----
		case PERIOD_SUM:		//---- time period summary ----
			time_diff.Report_Header (Header_Number ());
			break;
		case TCOST_DISTRIB:		//---- generalized cost distribution ----
		case COST_DISTRIB:		//---- cost difference distributions ----
		case COST_SUM:			//---- cost period summary ----
			cost_diff.Report_Header (Header_Number ());
			break;
		case TRIP_GAP:			//---- trip gap report ----
			Trip_Gap_Header ();
			break;
		default:
			break;
	}
}
