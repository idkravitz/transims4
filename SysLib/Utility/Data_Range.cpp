//*********************************************************
//	Data_Range.cpp - data range functions
//*********************************************************

#include "Data_Range.hpp"
#include "Utility.hpp"

#include "string.h"

//---------------------------------------------------------
//	Data_Range constructor
//---------------------------------------------------------

Data_Range::Data_Range (int factor, int minimum, int maximum, int increment) : 
	Range_Array (), Static_Service ()
{
	Setup (factor, minimum, maximum, increment);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Data_Range::Setup (int fac, int min, int max, int inc)
{
	if (fac < 1 || min >= max || max < 0 || inc < 0) {
		if (Send_Messages ()) {
			exe->Error ("Range Setup %d-%d-%d-%d is Out of Range", fac, min, max, inc);
		}
	} else {
		factor = fac;
		minimum = min * fac;
		maximum = max * fac;
		increment = inc;
	}
	return;
}

//---------------------------------------------------------
//	Add_Ranges
//---------------------------------------------------------

bool Data_Range::Add_Ranges (char *ranges)
{
	int low, high;
	double d1, d2;

	//---- check for special conditions ----

	if (ranges == NULL || *ranges == '\0' || *ranges == '\n') return (true);
	
	if (str_cmp (ranges, "All") == 0) {
		return (Add_Range (minimum, maximum, increment));
	}

	//---- unpack the range string ----	

	while (ranges != NULL && *ranges != '\0' && *ranges != '\n') {

		ranges = Get_Range (ranges, &d1, &d2);

		if (d1 == 0.0 && d2 == 0.0) continue;

		low = (int) (d1 * factor);
		high = (int) (d2 * factor + 0.5);

		if (low > high || low < minimum || high > maximum) {
			if (Send_Messages ()) {
				exe->Error ("Range %g-%g is Out of Range", d1, d2);
			}
		}
		if (!Add_Range (low, high, increment)) return (false);
	}
	return (Num_Ranges () > 0);
}

//---------------------------------------------------------
//	Add_Breaks
//---------------------------------------------------------

bool Data_Range::Add_Breaks (char *breaks)
{
	int low, high;
	double d1, d2;

	//---- check for special conditions ----

	if (breaks == NULL || *breaks == '\0' || *breaks == '\n') return (true);

	if (str_cmp (breaks, "None") == 0) {
		return (Add_Range (minimum, maximum));
	}

	//---- unpack the break string ----	

	low = minimum;

	while (breaks != NULL && *breaks != '\0' && *breaks != '\n') {

		breaks = Get_Range (breaks, &d1, &d2);

		if (d1 == 0.0 && d2 == 0.0) continue;
		if (d2 > d1) goto break_error;

		high = (int) (d2 * factor + 0.5);

		if (low > high || low < minimum || high > maximum) goto break_error;

		if (low < high) {
			if (!Add_Range (low, high)) return (false);
		}
		low = high;
	}
	high = maximum;

	if (low < high) {
		return (Add_Range (low, high));
	}
	return (Num_Ranges () > 0);

break_error:
	if (Send_Messages ()) {
		exe->Error ("Range Breaks %s are Illogical", breaks);
	}
	return (false);
}
