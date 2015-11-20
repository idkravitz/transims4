//*********************************************************
//	Read_Households.cpp - read the household list
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Read_Households
//---------------------------------------------------------

void PlanTrips::Read_Households (void)
{
	int hhold, nfile, nhhold;

	//---- read the household list files ----

	nhhold = 0;

	for (nfile=0; ; nfile++) {
		if (!hhlist_file.Open (nfile)) break;

		//---- process the list files ----

		while (hhlist_file.Read ()) {

			Get_Integer (hhlist_file.Record (), &hhold);

			if (hhold > 0) {
				if (!hhlist_data.Add (hhold)) {
					Error ("Adding Household List");
				}
				nhhold++;
			}
		}
	}
	Print (2, "Number of Household List Records = %d", nhhold);
}
