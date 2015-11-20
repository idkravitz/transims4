//*********************************************************
//	Smooth_Link.cpp - smooth link by time of day
//*********************************************************

#include "SmoothPlans.hpp"

//---------------------------------------------------------
//	Smooth_Link
//---------------------------------------------------------

void SmoothPlans::Smooth_Link (int iter, int link)
{
	int i, p, change, period, index, count, num1, num2, count1, count2;
	double prob;
	bool flag;

	Travel_Data *travel_ptr;
	Link_Time *link_ptr;
	Period_Index *period_ptr;

	//---- population the period index ----

	period_index.Zero_Data ();

	for (travel_ptr = travel_data.First (); travel_ptr; travel_ptr = travel_data.Next ()) {

		for (link_ptr = travel_ptr->First (); link_ptr; link_ptr = travel_ptr->Next ()) {

			if (link_ptr->Dir_Index () == link) {
				period = link_ptr->Time_Period () + travel_ptr->Change ();

				if (period < 1) {
					period = 1;
				} else if (period > nperiod) {
					period = nperiod;
				}
				period_ptr = period_index [period];

				travel_ptr->Next_Index (period_ptr->Traveler ());
				period_ptr->Add_Traveler (travel_data.Record_Index ());

				travel_ptr->Flag (false);
			}
		}
	}

	//---- distribute each period ----

	for (period_ptr = period_index.First (); period_ptr; period_ptr = period_index.Next ()) {

		period = period_ptr->Period ();
		count = period_ptr->Smoothed ();

		if (iter == 1) {
			period_ptr->Original (count);
		}

		count1 = (int) (count * 0.20 + 0.5);
		count2 = (int) (count * 0.20 + 0.5);
		
		num1 = num2 = 0;
		flag = true;

retry:
		for (index = period_ptr->Traveler (); index > 0; index = travel_ptr->Next_Index ()) {

			travel_ptr = travel_data [index];

			if (travel_ptr->Flag ()) continue;

			//---- apply the probability distribution ----

			//prob = random.Probability () * num / count;
			prob = random.Probability ();

			for (i=0; i <= 2 * naverage; i++) {
				if (prob < distribution [i]) break;
			}
			if (i == naverage) continue;

			//---- check the constraints ----

			p = period - naverage + i;
			if (p < 1 || p > nperiod) continue;

			change = travel_ptr->Change () + i - naverage;
			if (change < travel_ptr->Min_Change () || change > travel_ptr->Max_Change ()) continue;

			if (i > naverage) {
				if (num2 >= count2) continue;
				num2++;
			} else if (i < naverage) {
				if (num1 >= count1) continue;
				num1++;
			}

			//---- set the time period change ----

			travel_ptr->Change (change);
			travel_ptr->Flag (true);

			if (num1 == count1 && num2 == count2) break;
		}

		//---- check the results ----

		if (flag && (num1 < count1 || num2 < count2)) {
			flag = false;
			goto retry;
		}
	}

	if (summary_flag && iter == niter) {

		period_index.Zero_Data ();

		for (travel_ptr = travel_data.First (); travel_ptr; travel_ptr = travel_data.Next ()) {

			for (link_ptr = travel_ptr->First (); link_ptr; link_ptr = travel_ptr->Next ()) {

				if (link_ptr->Dir_Index () == link) {
					period = link_ptr->Time_Period () + travel_ptr->Change ();

					if (period < 1) {
						period = 1;
					} else if (period > nperiod) {
						period = nperiod;
					}
					period_ptr = period_index [period];

					travel_ptr->Next_Index (period_ptr->Traveler ());
					period_ptr->Add_Traveler (travel_data.Record_Index ());

					travel_ptr->Flag (false);
				}
			}
		}
	}
}
