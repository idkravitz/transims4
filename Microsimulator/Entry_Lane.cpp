//*********************************************************
//	Entry_Lane.cpp - entry lane on the next link
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Entry_Lane
//---------------------------------------------------------

int Microsimulator::Entry_Lane (int lane, Travel_Leg_Data *from_leg, Travel_Leg_Data *to_leg)
{
	int diff, in_low, in_high, out_low, out_high;

	//---- check the exit lane ----

	in_low = from_leg->Out_Best_Low ();
	in_high = from_leg->Out_Best_High ();

	if (lane < in_low) {
		in_low = from_leg->Out_Lane_Low ();
		out_low = to_leg->In_Lane_Low ();
		out_high = to_leg->In_Best_High ();
	} else if (lane > in_high) {
		in_high = from_leg->Out_Lane_High ();
		out_low = to_leg->In_Best_Low ();
		out_high = to_leg->In_Lane_High ();
	} else {
		out_low = to_leg->In_Best_Low ();
		out_high = to_leg->In_Best_High ();
	}

	diff = in_high - in_low + out_low - out_high;

	if (diff > 0) { 	//---- too many input lanes ----
		while (diff > 0) {
			if (lane > in_low) {
				in_low++;
				diff--;
			}
			if (diff > 0 && lane < in_high) {
				in_high--;
				diff--;
			}
		}
	} else if (diff < 0) {		//---- too many output lanes ----
		while (diff < 0) {
			out_low++;
			diff++;
			if (diff < 0) {
				out_high--;
				diff++;
			}
		}
	}
	return (lane + out_low - in_low);
}
