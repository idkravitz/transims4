//*********************************************************
//	Household_Match.cpp - match household to survey
//*********************************************************

#include "ActGen.hpp"

#define MAX_MATCH	24

//---------------------------------------------------------
//	Household_Match
//---------------------------------------------------------

void ActGen::Household_Match (HHold_Data *household_ptr)
{
	int i, hh, hhold, age, group, group1, group2, work, gender, match, ndata;
	double total, probability;
	bool flag;

	typedef struct {
		int person;
		int age;
		int group;
		int work;
		int gender;
		int used;
		int index;
	} Match_Data;

	Match_Data data [MAX_MATCH], *data_ptr;

	Type_Data *type_ptr;
	Survey_HHold *survey_hh_ptr;
	Population_Data *pop_ptr;
	Activity_Data act_rec;

	nhhold++;

	hhold_act_data.Reset ();
	survey_hhold = 0;

	type_ptr = type_data [household_ptr->Type ()];

	//---- select the survey household ----

	probability = random.Probability () * type_ptr->Cum_Weight ();
	if (max_variance > 0) {
		time_variance = (int) (2.0 * random_time.Probability () * max_variance + 0.5) - max_variance;
	}
	total = 0.0;
	survey_hh_ptr = 0;

	for (hhold = type_ptr->Household (); hhold; hhold = survey_hh_ptr->Type ()) {
		survey_hh_ptr = survey_hh_data [hhold];

		total += survey_hh_ptr->Weight ();

		if (total > probability) break;
	}
	survey_hhold = hhold = survey_hh_ptr->Household ();

	//--- gather the household members ----

	ndata = 0;
	data_ptr = data;

	for (pop_ptr = survey_pop_data.Get_GE (hhold); pop_ptr; pop_ptr = survey_pop_data.Next_Key ()) {
		if (pop_ptr->Household () != hhold) break;

		if (ndata == MAX_MATCH) break;

		//---- define age groups ----

		group = age = pop_ptr->Age ();

		if (age > 0) {
			if (age < 5) {
				group = 1;
			} else if (age < 12) {
				group = 2;
			} else if (age < 16) {
				group = 3;
			} else if (age < 21) {
				group = 4;
			} else if (age < 65) {
				group = 5;
			} else {
				group = 6;
			}
		}
		data_ptr->age = age;
		data_ptr->group = group;
		data_ptr->person = pop_ptr->Person ();
		data_ptr->work = pop_ptr->Work ();
		data_ptr->gender = pop_ptr->Gender ();
		data_ptr->used = 0;
		data_ptr->index = survey_pop_data.Record_Index ();

		ndata++;
		data_ptr++;
	}

	//---- match each person to the survey population ----

	hh = household_data.Record_Index ();
	flag = false;

	for (pop_ptr = population_data.Get_GE (hh); pop_ptr; pop_ptr = population_data.Next_Key ()) {
		if (pop_ptr->Household () != hh) break;
		nperson++;

		//---- define age groups ----

		group = age = pop_ptr->Age ();

		if (age > 0) {
			if (age < 5) {
				group = 1;
			} else if (age < 12) {
				group = 2;
			} else if (age < 16) {
				group = 3;
			} else if (age < 21) {
				group = 4;
			} else if (age < 65) {
				group = 5;
			} else {
				group = 6;
			}
		}
		work = pop_ptr->Work ();
		gender = pop_ptr->Gender ();

		if (work == 1) nwork++;

		//---- match to the survey data ----

		match = -1;

		if (group > 0) {

			//---- try to match to a new person ----

			for (i=0, data_ptr=data; i < ndata; i++, data_ptr++) {
				if (data_ptr->used == 0) {
					if (data_ptr->group == group) {
						if (match < 0) {
							match = i;
						} else {
							if (work == data_ptr->work) {
								if (work == data [match].work) {
									if (gender == data_ptr->gender && gender != data [match].gender) {
										match = i;
									}
								} else {
									match = i;
								}
							} else if (work != data [match].work) {
								if (gender == data_ptr->gender && gender != data [match].gender) {
									match = i;
								}
							}
						}
					}
				}
			}

			//---- match by age group ----

			if (match < 0) {
				for (i=0, data_ptr=data; i < ndata; i++, data_ptr++) {
					if (data_ptr->group == group) {
						if (match < 0) {
							match = i;
						} else {
							if (work == data_ptr->work) {
								if (work == data [match].work) {
									if (gender == data_ptr->gender) {
										if (gender != data [match].gender) {
											match = i;
										} else if (data_ptr->used < data [match].used) {
											match = i;
										}
									}
								} else {
									match = i;
								}
							} else if (work != data [match].work) {
								if (gender == data_ptr->gender) {
									if (gender != data [match].gender) {
										match = i;
									} else if (data_ptr->used < data [match].used) {
										match = i;
									}
								}
							}
						}
					}
				}
			}

			//---- relax the match criteria ----

			if (match < 0) {
				if (group == 2 || group == 3) {				//---- match to school children ----
					group1 = 2;
					group2 = 3;
				} else if (group == 5 || group == 6) {		//---- match to adults ----
					group1 = 5;
					group2 = 6;
				} else if (group == 4) {					//---- match to adult worker or child -----
					if (work) {
						group1 = 4;
						group2 = 5;
					} else {
						group1 = 3;
						group2 = 4;
					}
				} else {								//---- match to young children ----
					group1 = 1;
					group2 = 2;
				}

				//---- retry ----

				for (i=0, data_ptr=data; i < ndata; i++, data_ptr++) {
					if (data_ptr->group == group1 || data_ptr->group == group2) {
						if (match < 0) {
							match = i;
						} else {
							if (work == data_ptr->work) {
								if (work == data [match].work) {
									if (gender == data_ptr->gender) {
										if (gender != data [match].gender) {
											match = i;
										} else if (data_ptr->used < data [match].used) {
											match = i;
										}
									}
								} else {
									match = i;
								}
							} else if (work != data [match].work) {
								if (gender == data_ptr->gender) {
									if (gender != data [match].gender) {
										match = i;
									} else if (data_ptr->used < data [match].used) {
										match = i;
									}
								}
							}
						}
					}
				}
			}

		} else {

			//---- try to match to a new person ----

			for (i=0, data_ptr=data; i < ndata; i++, data_ptr++) {
				if (data_ptr->used == 0) {
					if (work == data_ptr->work) {
						if (match < 0) {
							match = i;
						} else {
							if (gender == data_ptr->gender && gender != data [match].gender) {
								match = i;
							}
						}
					}
				}
			}

			//---- match to worker and gender ----

			if (match < 0) {
				for (i=0, data_ptr=data; i < ndata; i++, data_ptr++) {
					if (work == data_ptr->work) {
						if (match < 0) {
							match = i;
						} else {
							if (gender == data_ptr->gender) {
								if (gender != data [match].gender) {
									match = i;
								} else if (data_ptr->used < data [match].used) {
									match = i;
								}
							}
						}
					}
				}
			}
		}

		//---- match problem ----

		if (match < 0) {
			flag = true;
			per_match++;

			//---- household match report ----

			if (Report_Flag (HOUSEHOLD_MATCH)) {
				Print (1, "%4d  %8d    %3d  %3d   %2d    %2d     %2d     %8d", 
					type_ptr->ID (), household_ptr->Household (), pop_ptr->Person (), 
					pop_ptr->Age (), group, pop_ptr->Work (), pop_ptr->Gender (), hhold);
			}

			if (match_flag) {
				char buffer [STRING_BUFFER];

				str_fmt (buffer, sizeof (buffer), "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
					type_ptr->ID (), household_ptr->Household (), pop_ptr->Person (), 
					pop_ptr->Age (), group, pop_ptr->Work (), pop_ptr->Gender (), hhold);

				if (!match_file.Write (buffer)) {
					Error ("Writing %s", match_file.File_Type ());
				}
			}

			//---- generate a problem message ----

			memset (&act_rec, '\0', sizeof (act_rec));

			act_rec.Household (household_ptr->Household ());
			act_rec.Person (pop_ptr->Person ());

			Activity_Problem (MATCH_PROBLEM, &act_rec);

		} else {

			if (group != data [match].group) age_match++;
			if (work != data [match].work) work_match++;
			if (gender != data [match].gender) gender_match++;

			data [match].used++;

			//---- household match report ----

			if (Report_Flag (HOUSEHOLD_MATCH)) {
				Print (1, "%4d  %8d    %3d  %3d   %2d    %2d     %2d     %8d   %3d  %3d   %2d    %2d     %2d", 
					type_ptr->ID (), household_ptr->Household (), pop_ptr->Person (), 
					pop_ptr->Age (), group, pop_ptr->Work (), pop_ptr->Gender (),
					hhold, data [match].person, data [match].age, 
					data [match].group, data [match].work, data [match].gender);
			}

			if (match_flag) {
				char buffer [STRING_BUFFER];

				str_fmt (buffer, sizeof (buffer), "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
					type_ptr->ID (), household_ptr->Household (), pop_ptr->Person (), 
					pop_ptr->Age (), group, pop_ptr->Work (), pop_ptr->Gender (),
					hhold, data [match].person, data [match].age, 
					data [match].group, data [match].work, data [match].gender);

				if (!match_file.Write (buffer)) {
					Error ("Writing %s", match_file.File_Type ());
				}
			}

			if (count_flag) {
				survey_pop_data [data [match].index]->Add_Count ();
			}

			//---- save activities ----

			Save_Activity (household_ptr, pop_ptr, hhold, data [match].person);
		}
	}
	if (flag) {
		hh_match++;
	}
	num_act_gen += hhold_act_data.Num_Records ();
}
