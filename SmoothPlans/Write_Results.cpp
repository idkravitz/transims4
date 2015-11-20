//*********************************************************
//	Write_Results.cpp - write the results to the file
//*********************************************************

#include "SmoothPlans.hpp"

//---------------------------------------------------------
//	Write_Results
//---------------------------------------------------------

void SmoothPlans::Write_Results (void)
{
	int link, nlink, period;

	Travel_Data *travel_ptr;
	Link_Time *link_ptr;
	Period_Index *period_ptr;
	Dir_Data *dir_ptr;

	//---- add a header to the file ----

	fprintf (summary_file.File (), "LINK\tDIR\tTIME\tORIGINAL\tSMOOTHED\n");

	nlink = link_list.Num_Records ();

	for (link=1; link <= nlink; link++) {

		period_index.Zero_Data ();

		for (travel_ptr = travel_data.First (); travel_ptr; travel_ptr = travel_data.Next ()) {

			for (link_ptr = travel_ptr->First (); link_ptr; link_ptr = travel_ptr->Next ()) {

				if (link_ptr->Dir_Index () == link) {
					period = link_ptr->Time_Period ();

					if (period < 1) {
						period = 1;
					} else if (period > nperiod) {
						period = nperiod;
					}
					period_ptr = period_index [period];

					period_ptr->Add_Original ();

					period = link_ptr->Time_Period () + travel_ptr->Change ();

					if (period < 1) {
						period = 1;
					} else if (period > nperiod) {
						period = nperiod;
					}
					period_ptr = period_index [period];

					period_ptr->Add_Smoothed ();
				}
			}
		}
		dir_ptr = dir_data [link];

		for (period_ptr = period_index.First (); period_ptr; period_ptr = period_index.Next ()) {
			fprintf (summary_file.File (), "%d\t%d\t%s\t%d\t%d\n", dir_ptr->Link (), dir_ptr->Dir (),
				time_step.Format_Step (period_ptr->Period () * increment), 
				period_ptr->Original (), period_ptr->Smoothed ());
		}
	}
}
