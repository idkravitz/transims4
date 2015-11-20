//*********************************************************
//	Population.cpp - Read Population File
//*********************************************************

#include "PlanSelect.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Population
//---------------------------------------------------------

void PlanSelect::Read_Population (void)
{
	Person_Data person_rec;

	int nperson, per_id, hh_id, num;
	char code [10], *str_ptr;
	
	person_data.Data_Size (sizeof (Person_Data));

	Show_Message ("Reading %s -- Record", pop_file.File_Type ());
	Set_Progress (10000);

	while (pop_file.Read ()) {
		Show_Progress ();

		if (Progress_Count () <= 2) continue;
		
		str_ptr = pop_file.Record ();

		str_ptr = Get_Integer (str_ptr, &num);
		str_ptr = Get_Integer (str_ptr, &num);
		str_ptr = Get_Token (str_ptr, code);
		
		if (str_cmp (code, "H") != 0) goto pop_error;

		str_ptr = Get_Integer (str_ptr, &hh_id);
		str_ptr = Get_Integer (str_ptr, &nperson);

		while (nperson-- > 0) {
			if (!pop_file.Read ()) goto pop_error;

			Show_Progress ();

			str_ptr = pop_file.Record ();

			str_ptr = Get_Integer (str_ptr, &num);
			str_ptr = Get_Token (str_ptr, code);

			if (num != hh_id || str_cmp (code, "P") != 0) goto pop_error;

			str_ptr = Get_Integer (str_ptr, &per_id);

			person_rec.person = per_id;
			person_rec.hhold = hh_id;

			if (!person_data.Add (&person_rec)) {
				Error ("Adding a Person to the Person List");
			}
		}
	}
	End_Progress ();

	person_data.Max_Records (person_data.Num_Records ());

	Print (2, "Number of Population Records Read = %d", Progress_Count ());
	return;

pop_error:
	Error ("The Population File is Not in TRANSIMS Format");
}
