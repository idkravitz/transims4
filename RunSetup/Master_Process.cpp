//*********************************************************
//	Master_Processing.cpp - process master controls
//*********************************************************

#include "RunSetup.hpp"

//---------------------------------------------------------
//	Master_Processing
//---------------------------------------------------------

void RunSetup::Master_Processing (void)
{
	int i, len;
	char *str_ptr, buffer [STRING_BUFFER], *ptr;

	Control_Group *group_ptr;

	//---- process each iteration ----

	for (i=first_run; i <= last_run; i++) {
	
		//---- process the each group ----

		for (group_ptr = (Control_Group *) control_group.First (); group_ptr; 
			group_ptr = (Control_Group *) control_group.Next ()) {

			if (i > 0) {
				str_fmt (group_ptr->run_name, sizeof (group_ptr->run_name), "%d", i);
				if (i > 1) {
					str_fmt (group_ptr->old_name, sizeof (group_ptr->old_name), "%d", i-1);
				}
			}

			Convert_Codes (group_ptr);

			//---- write the description ----

			if (group_ptr->description [0] != '\0') {
				str_cpy (buffer, sizeof (buffer), group_ptr->description);

                str_ptr = strstr (buffer, "@RUN@");
				if (str_ptr != NULL) {
					*str_ptr = '\0';
					str_ptr += 5;

					str_fmt (record, size, "%s %s%d%s", message, buffer, i, str_ptr);
				} else {
					str_fmt (record, size, "%s %s", message, buffer);
				}
				batch_file.Write ();

				*record = '\0';
				batch_file.Write ();
			}

			//---- add a line for each program ----

			str_ptr = group_ptr->programs;

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

				if (buffer [0] != '\0') {
					len = (int) strlen (buffer);
					ptr = strchr (buffer + (len - 4), '.');
					if (ptr == NULL) {
						str_cat (buffer, sizeof (buffer), exe);
					}
					str_fmt (record, size, "%s %s %s/%s%s", buffer, program_flags,
						control_dir, group_ptr->run_name, group_ptr->control);
					batch_file.Write ();

					str_cpy (record, size, exit_check);
					batch_file.Write ();

					*record = '\0';
					batch_file.Write ();
				}
			}
		}
	}
}
