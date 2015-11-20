//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SF3Dictionary.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SF3Dictionary::Execute (void)
{
	int num_in, num_out, num, segment, offset, last_segment;
	char *rec_ptr, buffer [FIELD_BUFFER], name [FIELD_BUFFER], *str_ptr, *ptr;

	Show_Message ("Reading %s -- Record", input_file.File_Type ());
	Set_Progress ();

	num_in = num_out = 0;

	name [0] = '\0';
	num = segment = last_segment = 0;
	offset = num_fields;

	while (input_file.Read ()) {
		Show_Progress ();

		rec_ptr = input_file.Record ();
		num_in++;

		if (strlen (rec_ptr) < 87) continue;

		str_ptr = Get_Token (rec_ptr + 73, buffer, sizeof (buffer));

		if (buffer [0] == '\0') continue;
		if (buffer [0] != 'H' && buffer [0] != 'P') continue;

		ptr = buffer + strlen (buffer) - 1;

		if (*ptr < '0' || *ptr > '9') continue;

		ptr -= 2;
		num = atoi (ptr);
		*ptr = '\0';

		if (str_cmp (buffer, name) != 0) {
			if (name [0] != '\0') {
				output_file.Write ();
				num_out++;
			}
			str_cpy (name, sizeof (name), buffer);

			//---- get the segment number ----

			Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] == '\0') {
				Error ("Reading Segment Number");
			}
			segment = atoi (buffer);
			if (segment == 0) {
				Error ("Reading Segment Number");
			}
			if (segment != last_segment) {
				last_segment = segment;
				offset = num_fields + 1;
			} else {
				offset++;
			}
			str_fmt (buffer, sizeof (buffer), "\"%s\"", name);
			output_file.Put_Field (1, buffer);
			output_file.Put_Field (2, segment);
			output_file.Put_Field (3, offset);
			output_file.Put_Field (4, num);
		} else {
			output_file.Put_Field (4, num);
			offset++;
		}
	}
	End_Progress ();

	if (name [0] != '\0') {
		output_file.Write ();
		num_out++;
	}
	output_file.Close ();

	Write (2, "Number of %s Records = %d", input_file.File_Type (), num_in);
	Write (1, "Number of %s Records = %d", output_file.File_Type (), num_out);

	Exit_Stat (DONE);
}
