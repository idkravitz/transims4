//*********************************************************
//	Write_Households.cpp - write household data
//*********************************************************

#include "PopSyn.hpp"

//---------------------------------------------------------
//	Write_Households
//---------------------------------------------------------

void PopSyn::Write_Households (Household_Model *model_ptr)
{
	int cell, cells, num_hh, hholds, hh_id, location0, location;
	int pop, num_pop, pop_rec [100], veh, num_veh, age, model, field;
	double replicate, hh_reps, bucket, weight, total, share, dvalue;

	PUMA_Data *data_ptr;
	PUMA_Zone *zone_ptr;
	Zone_Location *zloc_ptr;
	Location_List *list_ptr = NULL;
	Household_Index *index_ptr;
	Population_Data *pop_ptr;
	Veh_Distribution *veh_ptr;

	//---- write household file ----

	cells = model_ptr->Num_Cells ();
	if (cells == 0) return;
	
	model = household_model.Record_Index ();
	data_ptr = model_ptr->Get_PUMA_Data ();
	zone_ptr = model_ptr->Get_PUMA_Zone ();

	zloc_ptr = zone_location.Get (zone_ptr->Zone ());
	if (zloc_ptr == NULL) {
		if (warning_flag) {
			Warning ("%.0lf Households in Zone %d cannot be Located",
				model_ptr->Zone_Total (), zone_ptr->Zone ());
		}
		return;
	}
	location0 = zloc_ptr->Location ();

	//---- process the households in each cell ----

	for (cell=1; cell <= cells; cell++) {

		//---- get the number of synthetic households ----

		hholds = (int) model_ptr->Zone_Share (cell);
		if (hholds == 0) continue;

		//---- count the number of pums households ----

		num_hh = 0;

		for (index_ptr = data_ptr->First_HH (cell); index_ptr; index_ptr = data_ptr->Next_HH ()) {
			if (index_ptr->cell != cell) break;
			num_hh++;
		}
		if (num_hh == 0) continue;

		//---- calculate the replication factor ----

		replicate = (double) hholds / num_hh;
		bucket = 0.5;

		//---- replicate and save the pums households ----

		for (index_ptr = data_ptr->First_HH (cell); index_ptr; index_ptr = data_ptr->Next_HH ()) {
			if (index_ptr->cell != cell) break;

			hh_reps = replicate + bucket;
			num_hh = (int) hh_reps;
			bucket = hh_reps - num_hh;

			if (num_hh == 0) continue;

			//---- get the database record ----

			if (!household_db.Read_Record (index_ptr->index)) {
				Error ("Reading Household Database Index %d", index_ptr->index);
			}

			//---- copy the data fields to the output file ----

			new_hh_file->Copy_Fields (&household_db);

			hh_id = new_hh_file->Household ();

			//---- gather the population record numbers ----

			num_pop = num_veh = 0;
			pop_ptr = population_data.Get_GE (hh_id);

			while (pop_ptr != NULL && pop_ptr->Household () == hh_id) {
				pop = population_data.Record_Index ();

				if (age_field > 0) {
					if (!population_db.Read_Record (pop)) {
						Error ("Reading Population Database Index %d", pop);
					}
					population_db.Get_Field (age_field, &age);
					if (age >= 16) num_veh++;
				}
				pop_rec [num_pop++] = pop;

				pop_ptr = population_data.Next_Key ();
			}

			//---- get the number of vehicles ----

			if (vehicle_field > 0) {
				household_db.Get_Field (vehicle_field, &num_veh);
			} else if (age_field == 0) {
				num_veh = num_pop;
			}
			new_hh_file->Vehicles (num_veh);

			//---- write records to the output file ----

			for (; num_hh > 0; num_hh--, household++) {
				new_hh_file->Household (household);

				//---- locate the household ----

				total = 0.0;
				weight = random.Probability ();

				for (location=location0; location > 0; location = list_ptr->List ()) {
					list_ptr = location_list [location];
					total += list_ptr->Weight (model);

					if (total >= weight) break;
				}
				new_hh_file->Location (list_ptr->Location ());

				if (!new_hh_file->Write ()) {
					Error ("Writing %s", new_hh_file->File_Type ());
				}

				//---- summarize the household data ----

				if (hh_sum_flag) {
					for (field=1; field <= hh_fields; field++) {
						new_hh_file->Get_Field (field, &dvalue);

						hh_total [field] += dvalue;
						if (dvalue < hh_minimum [field]) hh_minimum [field] = dvalue;
						if (dvalue > hh_maximum [field]) hh_maximum [field] = dvalue;
					}
				}

				//---- write the population records ----

				for (pop=0; pop < num_pop; pop++) {
					if (!population_db.Read_Record (pop_rec [pop])) {
						Error ("Reading Population Database Index %d", pop_rec [pop]);
					}

					//---- copy the data fields to the output file ----

					new_pop_file->Copy_Fields (&population_db);

					new_pop_file->Household (household);
					new_pop_file->Person (pop+1);

					if (!new_pop_file->Write ()) {
						Error ("Writing %s", new_pop_file->File_Type ());
					}
					population++;

					if (pop_sum_flag) {
						for (field=1; field <= pop_fields; field++) {
							new_pop_file->Get_Field (field, &dvalue);

							pop_total [field] += dvalue;
							if (dvalue < pop_minimum [field]) pop_minimum [field] = dvalue;
							if (dvalue > pop_maximum [field]) pop_maximum [field] = dvalue;
						}
					}
				}

				//---- write the vehicle records ----

				if (num_veh > 0 && list_ptr->Parking () == 0) {
					Warning ("No Parking for %d Vehicles at Location %d", 
						num_veh, list_ptr->Location ());
					continue;
				}

				for (veh=0; veh < num_veh; veh++) {
					new_veh_file->Vehicle (vehicle++);
					new_veh_file->Household (household);
					new_veh_file->Location (list_ptr->Parking ());
					new_veh_file->Type (1);
					new_veh_file->Sub_Type (0);

					//---- apply the vehicle type distribution ----

					if (veh_type_flag) {
						share = random_type.Probability ();

						for (veh_ptr = veh_distribution.First (); veh_ptr; veh_ptr = veh_distribution.Next ()) {
							if (veh_ptr->Share () >= share) {
								new_veh_file->Type (veh_ptr->Type ());
								new_veh_file->Sub_Type (veh_ptr->Sub_Type ());
								break;
							}
						}
					}
					if (!new_veh_file->Write ()) {
						Error ("Writing %s", new_veh_file->File_Type ());
					}
				}
			}
		}
	}
}
