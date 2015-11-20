//*********************************************************
//	Zone_Report.cpp - Print the Zone Factor Report
//*********************************************************

#include "ActBalance.hpp"

//---------------------------------------------------------
//	Zone_Report
//---------------------------------------------------------

void ActBalance::Zone_Report (void)
{
	int i, zone, num_zone, num_group;
	double factor;

	Factor_Group *group;

	double average [MAX_PURPOSE], maximum [MAX_PURPOSE], minimum [MAX_PURPOSE];

	Show_Message ("Zone Factor Summary");

	num_zone = 0;
	num_group = factor_group.Num_Records ();

	for (i=1; i <= num_group; i++) {
		average [i] = 0.0;
		maximum [i] = 0.0;
		minimum [i] = 10000000.0;
	}

	//---- print the report ----

	Header_Number (ZONE_SUM);

	if (!Break_Check (max_zone + 7)) {
		Print (1);
		Zone_Header ();
	}		
	for (zone = 1; zone <= max_zone; zone++) {
		Print (1, "%5d ", zone);
		num_zone++;

		for (i=1; i <= num_group; i++) {
			group = (Factor_Group *) factor_group [i];

			if (group->current [zone] > 0.0 && group->target [0] > 0.0) {
				factor = (group->target [zone] * group->current [0]) / (group->current [zone] * group->target [0]);
			} else {
				factor = 1.0;
			}
			Print (0, "  %10.5lf", factor);

			average [i] += factor;
			if (factor > maximum [i]) maximum [i] = factor;
			if (factor < minimum [i]) minimum [i] = factor;
		}
	}
	if (num_zone > 0) {
		Break_Check (4);

		Print (2, "Average");
		for (i=1; i <= num_group; i++) {
			Print (0, " %10.5lf ", average [i] / num_zone);
		}
		Print (1, "Maximum");
		for (i=1; i <= num_group; i++) {
			Print (0, " %10.5lf ", maximum [i]);
		}
		Print (1, "Minimum");
		for (i=1; i <= num_group; i++) {
			Print (0, " %10.5lf ", minimum [i]);
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Zone_Header
//---------------------------------------------------------

void ActBalance::Zone_Header (void)
{
	Factor_Group *group;

	Print (1, "Zone Factor Summary Report");
	Print (2, " Zone ");

	for (group = (Factor_Group *) factor_group.First (); group; group = (Factor_Group *) factor_group.Next ()) {
		Print (0, "   Group #%-2d", group->group);
	}
	Print (1);
}
	 
/*********************************************|***********************************************

	Zone Factor Summary Report

	 Zone    Group #1    Group #2    Group #3    Group #4    Group #5    Group #6    Group #7

	ddddd   ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff
	
	Average ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff
	Maximum ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff
	Minimum ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff

**********************************************|***********************************************/ 
