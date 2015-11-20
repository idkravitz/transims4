//*********************************************************
//	Read_Plan.cpp - Read Each Plan File
//*********************************************************

#include "VISSIMPlan.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void VISSIMPlan::Read_Plan (void)
{
	int current_traveler, traveler, origin, destination, *path, num_path, period;
	bool time_flag, drive_flag, stat;

	Link_Data *link_ptr;
	Parking_Data *parking_ptr;

	//---- set the filename and extension ----

	if (plan_file.Extend ()) {
		Show_Message ("Reading Plan File %s -- Plan", plan_file.Extension ());
	} else {
		Show_Message ("Reading Plan File -- Plan");
	}
	Set_Progress (10000);

	//---- read each plan record ----

	time_flag = (time_range.Num_Ranges () > 0);
	current_traveler = origin = destination = period = 0;

	for (;;) {
		stat = plan_file.Read ();

		//---- get the traveler id ----

		traveler = plan_file.Traveler ();

		if (current_traveler != traveler || !stat) {

			if (current_traveler > 0 && drive_flag && period > 0 && random.Probability () <= percent) {
				num_out++;

				fprintf (file, "\n<ROUTE FROMZONENO=\"%d\" TOZONENO=\"%d\" INDEX=\"%d\">", origin, destination, num_out);

				fprintf (file, "\n<ITEMS>");

				path = drive_plan.Path (&num_path);

				if (type_flag) {
					parking_ptr = parking_data.Get (drive_plan.Start_ID ());

					if (parking_ptr != NULL) {
						link_ptr = link_data.Get (parking_ptr->Link ());

						if (link_ptr != NULL) {
							if (link_ptr->Anode () == *path) {
								fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Bnode ());
							} else if (link_ptr->Bnode () == *path) {
								fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Anode ());
							}
						}
					}
				} else {
					link_ptr = link_data.Get (abs (*path));

					if (link_ptr != NULL) {
						if (*path > 0) {
							fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Anode ());
						} else {
							fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Bnode ());
						}
					}
				}
				while (num_path-- > 0) {
					if (type_flag) {
						fprintf (file, "\n<ITEM NODE=\"%d\"/>", *path++);
					} else {
						link_ptr = link_data.Get (abs (*path));

						if (link_ptr != NULL) {
							if (*path > 0) {
								fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Bnode ());
							} else {
								fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Anode ());
							}
						}
						path++;
					}
				}
				if (type_flag) {
					parking_ptr = parking_data.Get (drive_plan.End_ID ());

					if (parking_ptr != NULL) {
						link_ptr = link_data.Get (parking_ptr->Link ());

						if (link_ptr != NULL) {
							path--;
							if (link_ptr->Anode () == *path) {
								fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Bnode ());
							} else if (link_ptr->Bnode () == *path) {
								fprintf (file, "\n<ITEM NODE=\"%d\"/>", link_ptr->Anode ());
							}
						}
					}
				}
				fprintf (file, "\n</ITEMS>");
				fprintf (file, "\n<DEMANDS>");
				fprintf (file, "\n<DEMAND VTI=\"%d\" VOLUME=\"1\"/>", period);
				fprintf (file, "\n</DEMANDS>");
				fprintf (file, "\n</ROUTE>");
			}
			if (!stat) break;

			current_traveler = traveler;
			drive_flag = false;
		}
		Show_Progress ();

		if (plan_file.Start_Type () == LOCATION_ID && plan_file.End_Type () == PARKING_ID) {
			origin = plan_file.Start_ID ();
		} else if (plan_file.Start_Type () == PARKING_ID && plan_file.End_Type () == LOCATION_ID) {
			destination = plan_file.End_ID ();
		} else if (plan_file.Start_Type () == PARKING_ID && plan_file.End_Type () == PARKING_ID) {
			if (time_flag) {
				period = time_range.Period (plan_file.Time ());
				if (period == 0) continue;
			} else {
				period = 1;
			}
			drive_plan.Plan (plan_file.Plan ());
			drive_flag = true;
		}
	}
	End_Progress ();

	plan_file.Close ();
}
