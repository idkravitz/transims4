//*********************************************************
//	Write_Plans.cpp - Write the Subarea Plan File
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Write_Plans
//---------------------------------------------------------

bool SubareaPlans::Write_Plans (int num_sub, Vehicle_Data *veh_ptr, bool *save_flag)
{
	int duration, old_duration, vehicle;
	bool update_flag = false;

	if (veh_ptr != NULL) {
		if (veh_ptr->Location () == 0) {
			vehicle = veh_ptr->Vehicle ();
			veh_ptr->Location (subarea_plans.Start_ID ());
		} else {
			Vehicle_Data veh_rec;

			vehicle = ++max_vehicle;

			veh_rec.Vehicle (vehicle);
			veh_rec.Household (veh_ptr->Household ());
			veh_rec.Location (subarea_plans.Start_ID ());
			veh_rec.Type (veh_ptr->Type ());
			veh_rec.Sub_Type (veh_ptr->Sub_Type ());

			if (!vehicle_data.Add (&veh_rec)) {
				Error ("Adding Subarea Vehicle to Vehicle Data");
			}
			update_flag = true;
		}
		subarea_plans.Vehicle (vehicle);
	}
	subarea_plans.Num_Path (num_sub);

	duration = subarea_plans.Stop_Time () - subarea_plans.Time ();
	if (duration < 0) {
		duration = 1;
		subarea_plans.Stop_Time (subarea_plans.Time () + duration);
	}
	old_duration = subarea_plans.Duration ();
	subarea_plans.Duration (duration);

	if (old_duration > 0) {
		subarea_plans.GCF ((subarea_plans.GCF () * duration + old_duration / 2) / old_duration);
	}

	//---- write the household list file ----

	*save_flag = Save_Plans ();

	if (*save_flag && hhlist_flag) {
		char buffer [FIELD_BUFFER];

		int hhold = subarea_plans.Traveler () / Traveler_Scale ();

		str_fmt (buffer, sizeof (buffer), "%d", hhold);

		if (!hhlist_file.Write (buffer)) {
			Error ("Writing %s", hhlist_file.File_Type ());
		}
	}
	return (update_flag);
}
