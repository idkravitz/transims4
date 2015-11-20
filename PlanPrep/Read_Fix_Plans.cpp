//*********************************************************
//	Read_Fix_Plans.cpp - Read the fix plans file
//*********************************************************

#include "PlanPrep.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Fix_Plans
//---------------------------------------------------------

void PlanPrep::Read_Fix_Plans (void)
{
	int field, count;
	char *record, buffer [FIELD_BUFFER];

	Fix_List fix_rec;

	Show_Message ("Reading %s -- Record", fix_file.File_Type ());
	Set_Progress (100000);

	//---- read the fix plan file ----

	field = 1;

	while (fix_file.Read ()) {
		Show_Progress ();

		record = fix_file.Record ();

		for (; record != NULL;) {
			record = Get_Token (record, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			switch (field) {
				case 1:
					fix_rec.number = atoi (buffer);
					field++;
					break;
				case 2:
					fix_rec.share = atof (buffer);

					if (fix_rec.share < 1.0 || fix_rec.share > 100.0) {
						Error ("Fix Plan Percentage %.1lf is Out of Range (1.0..100.0)", fix_rec.share);
					}
					fix_rec.share /= 100.0;
					field++;
					break;
				case 3:
					fix_rec.num_old = atoi (buffer);

					if (fix_rec.num_old < 1 || fix_rec.num_old > 1000) {
						Error ("Old List Size %d is Out of Range (1..1000)", fix_rec.num_old);
					}
					fix_rec.old_list = new int [fix_rec.num_old];
					if (fix_rec.old_list == NULL) goto mem_error;
					field++;
					break;
				case 4:
					fix_rec.num_new = atoi (buffer);

					if (fix_rec.num_new < 1 || fix_rec.num_new > 1000) {
						Error ("New List Size %d is Out of Range (1..1000)", fix_rec.num_new);
					}
					fix_rec.new_list = new int [fix_rec.num_new];
					if (fix_rec.new_list == NULL) goto mem_error;
					field++;
					count = 0;
					break;
				case 5:
					fix_rec.old_list [count] = atoi (buffer);

					if (++count == fix_rec.num_old) {
						field++;
						count = 0;
					}
					break;
				case 6:
					fix_rec.new_list [count] = atoi (buffer);

					if (++count == fix_rec.num_new) {
						field = 1;
						fix_rec.num_in = 0;
						fix_rec.num_out = 0;
						if (!fix_list.Add (&fix_rec)) goto mem_error;
					}
					break;
			}
		}
	}
	End_Progress ();

	fix_file.Close ();

	Print (1, "Number of Fix Plan Lists = %d", fix_list.Num_Records ());
	return;

mem_error:
	Error ("Insufficient Memory for Fix Plan Lists");
}
	


