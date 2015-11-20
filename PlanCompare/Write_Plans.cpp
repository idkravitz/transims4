//*********************************************************
//	Write_Plans.cpp - Write the Output Plan Files
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Write_Plans
//---------------------------------------------------------

void PlanCompare::Write_Plans (void)
{
	int traveler, trip, index;
	bool keep;

	Travel_Diff *diff_ptr;
	Plan_Data *plan_ptr;
	Select_Trips sel_rec;

	if (input_plans.Extend ()) {
		Show_Message ("Writing Plan File %s -- Plan ", input_plans.Extension ());
	} else {
		Show_Message ("Writing Plan File -- Plan ");
	}
	Set_Progress (10000);

	traveler = trip = 0;

	while (input_plans.Read ()) {
		Show_Progress ();

		plan_ptr = input_plans.Plan ();

		if (plan_ptr->key1 != traveler || (plan_ptr->key1 == traveler && plan_ptr->trip != trip)) {
			traveler = plan_ptr->key1;
			trip = plan_ptr->trip;

			sel_rec.traveler = traveler;
			sel_rec.trip = trip;

			index = select_trips.Get_Index (&sel_rec);

			if (!index) {
				keep = false;
			} else if (select_flag) {
				if (diff_flag || gap_flag) {
					diff_ptr = (Travel_Diff *) travel_diff [index];

					if (diff_ptr != NULL) {
						keep = (diff_ptr->percent > min_key.percent || 
							(diff_ptr->percent == min_key.percent && (traveler >= min_key.traveler ||
							(traveler == min_key.traveler && trip >= min_key.trip))));

						if (max_diff == 0 || diff_ptr->time_diff > max_diff) {
							max_diff = diff_ptr->time_diff;
						}
						if (min_diff == 0 || diff_ptr->time_diff < min_diff) {
							min_diff = diff_ptr->time_diff;
						}
						tot_diff += diff_ptr->time_diff;
					}

				} else {
					keep = (random.Probability () <= percent);
				}
			} else {
				keep = true;
			}
			if (keep) nout++;
		}
		if (keep) {
			tot_select += plan_ptr->stop_time - plan_ptr->key2;

			if (!output_plans.Write (plan_ptr)) {
				Error ("Writing Output Plan File");
			}
		}
	}
	End_Progress ();

	input_plans.Close ();
	output_plans.Close ();
}
