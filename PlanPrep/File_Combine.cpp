//*********************************************************
//	File_Combine.cpp - Combine Plan Files
//*********************************************************

#include "PlanPrep.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	File_Combine
//---------------------------------------------------------

void PlanPrep::File_Combine (void)
{
	int i, key1, key2, next;
	bool type_flag, binary_flag;
	char filename [FILE_NAME_SIZE];

	Plan_Data **plan_ptr, **ptr;

	if (nfile == 0) {
		type_flag = true;
		str_cpy (filename, sizeof (filename), input_plans.Pathname ());
		sort = input_plans.Plan_Sort ();
		extend = input_plans.Extend ();
		binary_flag = (input_plans.File_Format () == BINARY);

		for (; ; nfile++) {
			if (!input_plans.Open (nfile)) break;
		}
	} else {
		type_flag = false;
		str_cpy (filename, sizeof (filename), output_name);
		str_cat (filename, sizeof (filename), TEMP_LABEL);
		extend = input_plans.Extend ();
		binary_flag = true;
	}

	//---- allocate pointers for each file ----

	plan_file = new Plan_File * [nfile];
	plan_ptr = new Plan_Data * [nfile];

	if (plan_file == NULL || plan_ptr == NULL) {
		Error ("Insufficient Memory to Combine Files");	
	}

	//---- open a file handle for each file ----

	ptr = plan_ptr;

	for (i=0; i < nfile; i++, ptr++) {
		plan_file [i] = new Plan_File (filename, READ, PRIVATE_MEMORY, sort);

		if (plan_file [i] == NULL) {
			Error ("Insufficient Memory for Plan File");
		}
		plan_file [i]->Extend (extend);
		plan_file [i]->File_Format ((binary_flag) ? BINARY : UNFORMATED);

		if (!plan_file [i]->Open (i)) {
			File_Error ("Opening Plan File", plan_file [i]->Filename ());
		}

		//---- read the first plan in the file ----

		if (!plan_file [i]->Read ()) {
			File_Error ("Reading Plan File", plan_file [i]->Filename ());
		}
		*ptr = plan_file [i]->Plan ();
	}

	//---- interleaf plans based on sort keys ----

	Set_Progress (10000);

	for (;;) {
		key1 = MAX_INTEGER;
		key2 = MAX_INTEGER;
		next = 0;
		ptr = plan_ptr;

		//---- search for the lowest key value ----

		for (i=0; i < nfile; i++, ptr++) {
			if (*ptr == NULL) continue;

			if ((*ptr)->key1 < key1 || ((*ptr)->key1 == key1 && (*ptr)->key2 < key2)) {
				key1 = (*ptr)->key1;
				key2 = (*ptr)->key2;
				next = i;
			}
		}
		if (key1 == MAX_INTEGER && key2 == MAX_INTEGER) break;

		//---- output the plan ----

		Output_Plan (plan_ptr [next], true);

		Show_Progress ();

		//---- read the next plan in the file ----

		if (plan_file [next]->Read ()) {
			plan_ptr [next] = plan_file [next]->Plan ();
		} else {
			plan_ptr [next] = NULL;
		}
	}
	End_Progress ();

	//---- close the files and clear the memory -----

	output_plans.Close ();

	for (i=0; i < nfile; i++) {
		plan_file [i]->Close ();

		if (!type_flag) {
			str_cpy (filename, sizeof (filename), plan_file [i]->Filename ());
			str_cat (filename, sizeof (filename), ".def");

			remove (plan_file [i]->Filename ());
			remove (filename);
		} else {
			input_plans.Reset_Counters (input_plans.Num_Records () + plan_file [i]->Num_Records (),
										input_plans.Num_Plans () + plan_file [i]->Num_Plans (),
										input_plans.Num_Travelers () + plan_file [i]->Num_Travelers (),
										input_plans.Num_Trips () + plan_file [i]->Num_Trips ());
		}
		delete plan_file [i];
	}
	delete [] plan_file;
	delete [] plan_ptr;
}
