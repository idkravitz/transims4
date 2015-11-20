//*********************************************************
//	Read_PUMS.cpp - main execution procedure
//*********************************************************

#include "PUMSPrep.hpp"

//---------------------------------------------------------
//	Read_PUMS
//---------------------------------------------------------

void PUMSPrep::Read_PUMS (Db_File *pums_file)
{
	int person, puma, type;
	char *rec_ptr, buffer [FIELD_BUFFER];
	bool hh_flag;

	PUMS_FIELD *fld_ptr;
	STATE_PUMA *sel_ptr;

	Show_Message ("Reading %s -- Record", pums_file->File_Type ());
	Set_Progress ();

	person = 0;
	hh_flag = false;

	while (pums_file->Read ()) {
		Show_Progress ();

		rec_ptr = pums_file->Record ();
		nrec++;

		if (*rec_ptr == 'H') {
			pums_hh++;

			if (hh_flag && person > 0) {
				new_hh_file.Put_Field (persons_field, person);

				if (!new_hh_file.Write ()) {
					Error ("Writing %s", new_hh_file.File_Type ());
				}
				hh_out++;
			}
			hh_flag = true;
			person = 0;

			//---- store the household data fields ----

			for (fld_ptr = (PUMS_FIELD *) hh_fields.First (); fld_ptr; fld_ptr = (PUMS_FIELD *) hh_fields.Next ()) {
				if (fld_ptr->start < 0) {
					memset (buffer, '\0', sizeof (buffer));
				} else {
					memcpy (buffer, rec_ptr + fld_ptr->start, fld_ptr->length);
					buffer [fld_ptr->length] = '\0';
				}
				new_hh_file.Put_Field (fld_ptr->field, buffer);
			}

			//---- check the household type ----

			if (type_flag) {
				new_hh_file.Get_Field (type_field, &type);

				hh_flag = select_type.In_Range (type);
			}

			//---- select households ----

			if (select_flag && hh_flag) {
				hh_flag = false;
				new_hh_file.Get_Field (state_field, buffer);
				new_hh_file.Get_Field (puma_field, &puma);

				for (sel_ptr = (STATE_PUMA *) state_puma.First (); sel_ptr; sel_ptr = (STATE_PUMA *) state_puma.Next ()) {
					if (sel_ptr->puma == puma) {
						if (str_cmp (sel_ptr->state, buffer) == 0) {
							hh_flag = true;
							break;
						}
					}
				}
			}

		} else if (*rec_ptr == 'P') {
			pums_pop++;
			if (hh_flag) {
				person++;

				//---- store the population data fields ----

				for (fld_ptr = (PUMS_FIELD *) pop_fields.First (); fld_ptr; fld_ptr = (PUMS_FIELD *) pop_fields.Next ()) {
					if (fld_ptr->start < 0) {
						memset (buffer, '\0', sizeof (buffer));
					} else {
						memcpy (buffer, rec_ptr + fld_ptr->start, fld_ptr->length);
						buffer [fld_ptr->length] = '\0';
					}
					new_pop_file.Put_Field (fld_ptr->field, buffer);
				}
				if (person_field > 0) {
					new_pop_file.Put_Field (person_field, person);
				}
				if (script_flag) {
					program.Execute ();
				}
				if (!new_pop_file.Write ()) {
					Error ("Writing %s", new_pop_file.File_Type ());
				}
				pop_out++;
			}
		}
	}
	if (hh_flag && person > 0) {
		new_hh_file.Put_Field (persons_field, person);

		if (!new_hh_file.Write ()) {
			Error ("Writing %s", new_hh_file.File_Type ());
		}
		hh_out++;
	}
	End_Progress ();

	pums_file->Close ();
}
