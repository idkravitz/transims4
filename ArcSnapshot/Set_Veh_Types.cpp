//*********************************************************
//	Set_Veh_Types.cpp - set the vehicle type length
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Set_Veh_Types
//---------------------------------------------------------

void ArcSnapshot::Set_Veh_Types (void)
{
	int type, fld;
	double size;
	char buffer [FIELD_BUFFER];

	Veh_Type_Data *veh_type_ptr;

	//---- process vehicle type data ----

	max_type = 1;

	for (veh_type_ptr = veh_type_data.First (); veh_type_ptr; veh_type_ptr = veh_type_data.Next ()) {
		type = veh_type_ptr->Type ();
		if (type > max_type) max_type = type;
	}

	//---- initialize the vehicle length array ----

	veh_length = new double [++max_type];
	if (veh_length == NULL) goto mem_error;

	for (type=1; type < max_type; type++) {
		veh_length [type] = cell_size;
	}

	//---- set vehicle length and cell size ----

	size = 1000;

	for (veh_type_ptr = veh_type_data.First (); veh_type_ptr; veh_type_ptr = veh_type_data.Next ()) {
		type = veh_type_ptr->Type ();
		veh_length [type] = UnRound (veh_type_ptr->Length ());

		if (veh_length [type] < size) {
			size = veh_length [type];
		}
	}
	if (size < cell_size) cell_size = size;

	//---- allocate link arrays ----

	if (sum_flag) {
		num_links = link_data.Num_Records () + 1;

		vehicles = new int * [max_type];
		persons = new int * [max_type];
		veh_fld = new int [max_type];
		per_fld = new int [max_type];

		if (vehicles == NULL || persons == NULL || veh_fld == NULL || per_fld == NULL) goto mem_error;

		memset (vehicles, '\0', max_type * sizeof (int *));
		memset (persons, '\0', max_type * sizeof (int *));
		memset (veh_fld, '\0', max_type * sizeof (int));
		memset (per_fld, '\0', max_type * sizeof (int));

		summary_file.Add_Field ("LINK", INTEGER, 10);
		summary_file.Add_Field ("START", STRING, TIME_BUFFER);
		summary_file.Add_Field ("END", STRING, TIME_BUFFER);
		summary_file.Add_Field ("VEHICLES", INTEGER, 10);
		summary_file.Add_Field ("PERSONS", INTEGER, 10);

		link_fld = 1;
		start_fld = 2;
		end_fld = 3;

		fld = 4;
		veh_fld [0] = fld++;
		per_fld [0] = fld++;

		for (type=0; type < max_type; type++) {
			if (type > 0 && veh_type_flag && !veh_types.In_Range (type)) continue;

			if (type > 0) {
				str_fmt (buffer, sizeof (buffer), "VEH%d", type);
				summary_file.Add_Field (buffer, INTEGER, 10);
				veh_fld [type] = fld++;

				str_fmt (buffer, sizeof (buffer), "PER%d", type);
				summary_file.Add_Field (buffer, INTEGER, 10);
				per_fld [type] = fld++;
			}
			vehicles [type] = new int [num_links];
			persons [type] = new int [num_links];

			if (vehicles [type] == NULL || persons [type] == NULL) goto mem_error;
		}
		summary_file.Write_Header ();
	}
	return;

mem_error:
	Error ("Insufficient Memory for %d Vehicle Types", max_type);
}
