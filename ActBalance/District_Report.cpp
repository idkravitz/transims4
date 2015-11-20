//*********************************************************
//	District_Report.cpp - Print the District Factor Report
//*********************************************************

#include "ActBalance.hpp"

//---------------------------------------------------------
//	District_Report
//---------------------------------------------------------

void ActBalance::District_Report (void)
{
	int i, zone, num_zone, num_group, district, num_dist, max_dist;
	double factor;

	Factor_Group *group;

	double target [MAX_PURPOSE], current [MAX_PURPOSE];
	double average [MAX_PURPOSE], maximum [MAX_PURPOSE], minimum [MAX_PURPOSE];

	Show_Message ("District Factor Summary");

	num_dist = 0;
	max_dist = zone_equiv.Num_Groups ();
	num_group = factor_group.Num_Records ();

	for (i=1; i <= num_group; i++) {
		average [i] = 0.0;
		maximum [i] = 0.0;
		minimum [i] = 10000000.0;
	}

	//---- print the report ----

	Header_Number (DISTRICT_SUM);

	if (!Break_Check (max_dist + 7)) {
		Print (1);
		District_Header ();
	}
	for (district = 1; district <= max_dist; district++) {
		num_zone = 0;
		memset (target, '\0', sizeof (target));
		memset (current, '\0', sizeof (current));

		for (zone = 1; zone <= max_zone; zone++) {
			if (zone_equiv.Zone_Group (zone) != district) continue;
			num_zone++;

			for (i=1; i <= num_group; i++) {
				group = (Factor_Group *) factor_group [i];

				if (group->target [0] > 0.0) {
					target [i] += group->target [zone] / group->target [0];
				}
				if (group->current [0] > 0.0) {
					current [i] += group->current [zone] / group->current [0];
				}
			}
		}
		if (num_zone == 0) continue;

		Print (1, "%5d ", district);
		num_dist++;

		for (i=1; i <= num_group; i++) {
			if (current [i] > 0.0) {
				factor = target [i] / current [i];
			} else {
				factor = 1.0;
			}
			Print (0, "  %10.5lf", factor);

			average [i] += factor;
			if (factor > maximum [i]) maximum [i] = factor;
			if (factor < minimum [i]) minimum [i] = factor;
		}
	}
	if (num_dist > 0) {
		Break_Check (4);

		Print (2, "Average");
		for (i=1; i <= num_group; i++) {
			Print (0, " %10.5lf ", average [i] / num_dist);
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
//	District_Header
//---------------------------------------------------------

void ActBalance::District_Header (void)
{
	Factor_Group *group;

	Print (1, "District Factor Summary Report");
	Print (2, "District");

	for (group = (Factor_Group *) factor_group.First (); group; group = (Factor_Group *) factor_group.Next ()) {
		Print (0, " Group #%-2d  ", group->group);
	}
	Print (1);
}
	 
/*********************************************|***********************************************

	District Factor Summary Report

	District Group #1    Group #2    Group #3    Group #4    Group #5    Group #6    Group #7

	ddddd   ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff
	
	Average ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff
	Maximum ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff
	Minimum ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff  ffff.fffff

**********************************************|***********************************************/ 
