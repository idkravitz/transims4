//*********************************************************
//	Smooth_Delay.cpp - smooth the link delay data
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Smooth_Delay
//---------------------------------------------------------

void LinkDelay::Smooth_Delay (TTime_Data *ttime_ptr, int num, Connect_Time **turn_ptr)
{
	int i, n, nperiod, lvalue;
	double bucket, dvalue;

	Connect_Time *connect_ptr;

	//---- smooth the link travel time ----

	nperiod = ttime_ptr->Periods ();

	if (nperiod > smooth_data.Num_Input ()) {
		smooth_data.Num_Input (nperiod);
	}
	for (i=1; i <= nperiod; i++) {
		smooth_data.Input (i, UnRound (ttime_ptr->TTime (i)));
	}
	smooth_data.Smooth ();

	for (i=1; i <= nperiod; i++) {
		ttime_ptr->TTime (i, Round (smooth_data.Output (i)));
	}

	//---- smooth the link volumes ----

	for (i=1; i <= nperiod; i++) {
		smooth_data.Input (i, ttime_ptr->Volume (i));
	}
	smooth_data.Smooth ();

	bucket = 0.45;

	for (i=1; i <= nperiod; i++) {
		dvalue = smooth_data.Output (i) + bucket;
		lvalue = (int) dvalue;
		bucket = dvalue - lvalue;

		ttime_ptr->Volume (i, lvalue);
	}
	if (num == 0) return;

	//---- process each turn movement ----

	for (n=0; n < num; n++) {
		connect_ptr = turn_ptr [n];

		nperiod = connect_ptr->Periods ();
		if (nperiod == 0) continue;

		for (i=1; i <= nperiod; i++) {
			smooth_data.Input (i, UnRound (connect_ptr->TTime (i)));
		}
		smooth_data.Smooth ();

		for (i=1; i <= nperiod; i++) {
			connect_ptr->TTime (i, Round (smooth_data.Output (i)));
		}

		//---- smooth the link volumes ----

		for (i=1; i <= nperiod; i++) {
			smooth_data.Input (i, connect_ptr->Volume (i));
		}
		smooth_data.Smooth ();

		bucket = 0.45;

		for (i=1; i <= nperiod; i++) {
			dvalue = smooth_data.Output (i) + bucket;
			lvalue = (int) dvalue;
			bucket = dvalue - lvalue;

			connect_ptr->Volume (i, lvalue);
		}
	}
}
