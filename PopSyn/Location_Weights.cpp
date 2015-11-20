//*********************************************************
//	Location_Weights.cpp - Calculate Location Weights
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	Location_Weights
//---------------------------------------------------------

void PopSyn::Location_Weights (void)
{
	int location, location0, count, model;
	double total, weight;
	bool flag;

	Zone_Location *zone_ptr;
	Location_List *list_ptr;
	Household_Model *model_ptr;

	//---- normalize the weights in each zone ----

	for (zone_ptr = zone_location.First (); zone_ptr; zone_ptr = zone_location.Next ()) {

		//---- check for activity locations in each zone ----

		location0 = zone_ptr->Location ();

		if (location0 == 0) {
			Warning ("Zone %d has No Activity Locations", zone_ptr->Zone ());
			continue;
		}

		for (model_ptr = household_model.First (); model_ptr; model_ptr = household_model.Next ()) {

			count = 0;
			total = 0.0;
			model = household_model.Record_Index ();

			//---- sum the weight for each activity location within the zone ----

			for (location=location0; location != 0; location = list_ptr->List ()) {
				list_ptr = location_list [location];
				if (list_ptr == NULL) {
					Error ("Retrieving Location %d from the List", location);
				}
				total += list_ptr->Weight (model);
				count++;
			}
			if (count == 0) continue;

			weight = 1.0;
			flag = (total != 0.0);
			if (!flag) weight = 1.0 / count;

			//---- normalize the weights ----

			for (location=location0; location != 0; location = list_ptr->List ()) {
				list_ptr = location_list [location];

				if (flag) {
					weight = list_ptr->Weight (model);
					weight /= total;
				}
				list_ptr->Weight (model, weight);
			}
		}
	}
}
