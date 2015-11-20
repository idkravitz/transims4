//*********************************************************
//	Write_Factors.cpp - write the balancing factor file
//*********************************************************

#include "ActBalance.hpp"

//---------------------------------------------------------
//	Write_Factors
//---------------------------------------------------------

void ActBalance::Write_Factors (void)
{
	int zone, zone_field;
	double target, current, factor;

	Factor_Group *group;

	Show_Message ("Writing %s -- Record", factor_file.File_Type ());
	Set_Progress ();

	zone_field = factor_file.Field_Number ("ZONE");

	//---- total the zone data ----

	for (group = (Factor_Group *) factor_group.First (); group; group = (Factor_Group *) factor_group.Next ()) {
		target = current = 0.0;

		for (zone = 1; zone <= max_zone; zone++) {
			target += group->target [zone];
			current += group->current [zone];
		}
		group->target [0] = target;
		group->current [0] = current;
	}

	//---- store the zone factors ----

	for (zone = 1; zone <= max_zone; zone++) {
		Show_Progress ();

		factor_file.Put_Field (zone_field, zone);

		for (group = (Factor_Group *) factor_group.First (); group; group = (Factor_Group *) factor_group.Next ()) {
			if (group->current [zone] > 0.0 && group->target [0] > 0.0) {
				factor = (group->target [zone] * group->current [0]) / (group->current [zone] * group->target [0]);
			} else {
				factor = 1.0;
			}
			factor_file.Put_Field (group->factor_field, factor);
		}
		if (!factor_file.Write ()) {
			Error ("Writing Balancing Factors");
		}
	}
	End_Progress ();

	factor_file.Close ();
}
