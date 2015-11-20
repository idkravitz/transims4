//*********************************************************
//	Update_Times.cpp - Update the Link Travel Times
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Update_Travel_Times
//---------------------------------------------------------

void Router::Update_Travel_Times (void)
{
	int i, j, time0, vol, nperiod, type, lanes, list, period, tod1, tod, cap, tod_cap, len;

	TTime_Data *ttime_ptr;
	Link_Data *link_ptr;
	Link_Use_Data *use_ptr;

	nupdates++;
	period = ttime_data.Period_Size ();
	tod1 = period / 2;

	for (ttime_ptr = ttime_data.First (); ttime_ptr != NULL; ttime_ptr = ttime_data.Next ()) {

		link_ptr = link_data [ttime_ptr->Link ()];
		type = link_ptr->Type ();
		len = link_ptr->Length ();

		time0 = ttime_ptr->Time0 ();
		cap = ttime_ptr->Capacity ();
		nperiod = ttime_ptr->Periods ();
		lanes = ttime_ptr->Thru ();
		if (lanes < 1) lanes = 1;

		list = ttime_ptr->TOD_List ();

		for (i=1, tod = tod1; i <= nperiod; i++, tod += period) {
			vol = ttime_ptr->Volume (i);

			if (vol > 0) {
				//---- check for time of day restrictions ----

				tod_cap = cap;

				for (j = list; j; j = use_ptr->TOD_List ()) {
					use_ptr = link_use_data [j]; 

					if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
						tod_cap = (tod_cap * use_ptr->Lanes () + lanes / 2) / lanes;
						break;
					}
				}
				ttime_ptr->TTime (i, equation.Apply_Equation (type, time0, vol * vol_factor, tod_cap, len));
			}
		}
	}
}
