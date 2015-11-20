//*********************************************************
//	Trip_Report.cpp - print summary report
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Trip_Report
//---------------------------------------------------------

void SubareaPlans::Trip_Report (void)
{
	int i, j, p, num_zone, num_time, trip;
	char *in = "Internal";
	char *out = "External";

	if (zone_flag) {
		num_zone = zone_equiv.Num_Groups ();
	} else {
		num_zone = 2;
	}
	if (time_flag) {
		num_time = time_equiv.Num_Periods ();
	} else {
		num_time = 1;
	}

	Header_Number (TRIP_REPORT);

	if (!Break_Check (num_zone * num_zone * num_time)) {
		Print (1);
		Page_Header ();
	}

	for (p=1; p <= num_time; p++) {
		for (i=1; i <= num_zone; i++) {
			for (j=1; j <= num_zone; j++) {
				trip = trips [p] [i] [j];

				if (trip != 0) {
					if (zone_flag) {
						Print (1, "%5d   %8d   %8d   %8d", p, i, j, trip);
					} else {
						Print (1, "%5d   %8.8s   %8.8s   %8d", p, 
							((i == 1) ? in : out), ((j == 1) ? in : out), trip);
					}
				}
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void SubareaPlans::Page_Header (void)
{
	if (Header_Number () == TRIP_REPORT) {
		Print (1, "Trip Summary Report");
		Print (2, "Period     Start        End      Trips");
		Print (1);
	}
}

/*********************************************|***********************************************

	Internal-External Report

	Period     Start        End      Trips

	    1   Internal   External   dddddddd
	ddddd   ssssssss   ssssssss   dddddddd


**********************************************|***********************************************/ 
