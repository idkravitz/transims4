//*********************************************************
//	Vehicle_Types.cpp - read vehicle type distribution file
//*********************************************************

#include "PopSyn.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Vehicle_Types
//---------------------------------------------------------

void PopSyn::Vehicle_Types (void)
{
	int  field, lvalue;
	char *str_ptr, buffer [FIELD_BUFFER];
	double dvalue, share;
	bool flag;
	
	Veh_Distribution veh_rec, *veh_ptr;

	//---- read the vehicle type distribution file ----

	Show_Message ("Reading %s -- Record", veh_type_file.File_Type ());
	Set_Progress (100);

	flag = true;
	
	while (veh_type_file.Read ()) {
		Show_Progress ();

		str_ptr = veh_type_file.Record ();

		//---- check for a header record ----

		if (flag) {
			flag = false;
			if (*str_ptr < '0' || *str_ptr > '9') continue;
		}

		//---- process each field ----

		field = lvalue = 0;
		dvalue = 0.0;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] == '\0') break;

			field++;

			if (field == 1) {
				veh_rec.Type (atoi (buffer));
				if (veh_rec.Type () < 1) {
					Error ("Vehicle Type %d is Out of Range", veh_rec.Type ());
				}
			} else if (field == 2) {
				dvalue = atof (buffer);
				lvalue = (int) dvalue;
			} else {
				dvalue = atof (buffer);
			}
		}
		if (field < 3) {
			veh_rec.Sub_Type (0);
		} else {
			veh_rec.Sub_Type (lvalue);
		}
		veh_rec.Share (dvalue);

		if (!veh_distribution.Add (&veh_rec)) {
			Error ("Adding Vehicle Distribution Record");
		}
	}
	End_Progress ();
	veh_type_file.Close ();

	Print (2, "Number of %s Records = %d", veh_type_file.File_Type (), Progress_Count ());

	if (veh_distribution.Num_Records () == 0) {
		Error ("No Vehicle Distribution Records were Found");
	}

	//---- create a cumulative distribution ----

	share = 0.0;

	for (veh_ptr = veh_distribution.First (); veh_ptr; veh_ptr = veh_distribution.Next ()) {
		share += veh_ptr->Share ();
	}
	flag = (share != 0.0);
	if (!flag) share = 1.0 / veh_distribution.Num_Records ();

	dvalue = 0.0;

	for (veh_ptr = veh_distribution.First (); veh_ptr; veh_ptr = veh_distribution.Next ()) {
		if (flag) {
			dvalue += veh_ptr->Share () / share;
		} else {
			dvalue += share;
		}
		veh_ptr->Share (dvalue);
	}
}
