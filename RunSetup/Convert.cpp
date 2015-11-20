//*********************************************************
//	Convert.cpp - Replace Keywords in Configuration File
//*********************************************************

#include "RunSetup.hpp"

#define KEY_SPACE	39

//---------------------------------------------------------
//	Convert_Codes
//---------------------------------------------------------

void RunSetup::Convert_Codes (Control_Group *group)
{
	Db_File control_file;

	int i, count, len, index;
	bool found, stat;

	char **code, *str_ptr, *at, string [STRING_BUFFER], buffer [STRING_BUFFER];

	char *code_list [] = {
		"@PROJECT@", "@BIN@", "@NETWORK@", "@DIR@", "@FORMAT@", "@ALT@", "@MODEL@", "@YEAR@", 
		"@RUN@", "@PREVIOUS@", "@GROUP@", "@PARAM@", "@PARAM1@", "@PARAM2@", "@PARAM3@", 
		"@COMMENT@", "@COMMENT1@", "@COMMENT2@", "@COMMENT3@", "@SPLITS@", NULL
	};

	Print (1);
	if (group->group_num == 0) {
		str_fmt (string, sizeof (string), "Default Control Group");
	} else {
		str_fmt (string, sizeof (string), "Control Group #%d", group->group_num);
	}
	str_fmt (buffer, sizeof (buffer), "%s/%s%s", control_dir, group->run_name, group->control);

	control_file.File_Type (string);
	control_file.File_Access (Db_File::CREATE);
	control_file.Open (buffer);

	Show_Message (2, "Converting %s... ", group->master_file->File_Type ());

	count = 0;
	index = 0;
	stat = true;

	while (stat) {
		stat = group->master_file->Read ();

		if (stat) {
			str_cpy (string, sizeof (string), group->master_file->Record ());
		} else {
			if (index > 3) break;
			stat = true;
			if (group->add_key [index++] == '\0') continue;
			str_cpy (string, sizeof (string), group->add_key [index-1]);
		}

		//---- remove end of line markers ----

		str_ptr = string + strlen (string);

		while (*str_ptr == '\0' || *str_ptr == '\n' || *str_ptr == '\r') {
			*str_ptr = '\0';
			str_ptr--;
			if (str_ptr < string) break;
		}

		//---- check for NULL previous run keys ----

		if (group->old_name [0] == '\0') {
			if (strstr (string, "@PREVIOUS@") != NULL) {
				Get_Token (string, buffer, sizeof (buffer));

				str_fmt (string, sizeof (string), "%s%*cNULL", 
					buffer, (KEY_SPACE - strlen (buffer)), BLANK);
			}
		}

		//---- replace codes ----

		while ((at = strchr (string, '@')) != NULL) {
			if (strchr (at+1, '@') == NULL) break;
			found = false;

			for (i=0, code=code_list; *code != NULL; code++, i++) {
                str_ptr = strstr (string, *code);
				if (str_ptr == NULL) continue;

				*str_ptr = '\0';
				str_ptr += (int) strlen (*code);

				switch (i) {
					case 0:		//---- project ----
						str_fmt (buffer, sizeof (buffer),  "%s%s%s", string, Project_Directory (), str_ptr);
						break;
					case 1:		//---- bin ----
						str_fmt (buffer, sizeof (buffer),  "%s%s%s", string, bin_dir, str_ptr);
						break;
					case 2:		//---- network ----
						str_fmt (buffer, sizeof (buffer),  "%s%s%s", string, network_dir, str_ptr);
						break;
					case 3:		//---- directory ----
						str_fmt (buffer, sizeof (buffer), "%s%s%s", string, group->directory, str_ptr);
						break;
					case 4:		//---- file format ----
						str_fmt (buffer, sizeof (buffer), "%s%s%s", string, file_format, str_ptr);
						break;
					case 5:		//---- alternative ----
						if (alt_name [0] != '\0') {
							str_fmt (buffer, sizeof (buffer), "%s%s%s", string, alt_name, str_ptr);
						} else {
							if (*str_ptr == '\0') {
								len = (int) strlen (string);
								if (len > 0) {
									if (string [len-1] == '.' || string [len-1] == '_') {
										string [len-1] = '\0';
									}
								}
							} else if (*str_ptr == '.' || *str_ptr == '_') {
								str_ptr++;
							}
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 6:		//---- model ----
						if (model_name [0] != '\0') {
							str_fmt (buffer, sizeof (buffer), "%s%s%s", string, model_name, str_ptr);
						} else {
							if (*str_ptr == '\0') {
								len = (int) strlen (string);
								if (len > 0) {
									if (string [len-1] == '.' || string [len-1] == '_') {
										string [len-1] = '\0';
									}
								}
							} else if (*str_ptr == '.' || *str_ptr == '_') {
								str_ptr++;
							}
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 7:		//---- year ----
						if (year_name [0] != '\0') {
							str_fmt (buffer, sizeof (buffer), "%s%s%s", string, year_name, str_ptr);
						} else {
							if (*str_ptr == '\0') {
								len = (int) strlen (string);
								if (len > 0) {
									if (string [len-1] == '.' || string [len-1] == '_') {
										string [len-1] = '\0';
									}
								}
							} else if (*str_ptr == '.' || *str_ptr == '_') {
								str_ptr++;
							}
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 8:		//---- run ---- 
						if (group->run_name [0] != '\0') {
							str_fmt (buffer, sizeof (buffer), "%s%s%s", string, group->run_name, str_ptr);
						} else {
							if (*str_ptr == '\0') {
								len = (int) strlen (string);
								if (len > 0) {
									if (string [len-1] == '.' || string [len-1] == '_') {
										string [len-1] = '\0';
									}
								}
							} else if (*str_ptr == '.' || *str_ptr == '_') {
								str_ptr++;
							}
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 9:		//---- previous ----
						str_fmt (buffer, sizeof (buffer),  "%s%s%s", string, group->old_name, str_ptr);
						break;
					case 10:		//---- group ----
						str_fmt (buffer, sizeof (buffer),  "%s%d%s", string, group->group_num, str_ptr);
						break;
					case 11:		//---- param ----
						str_fmt (buffer, sizeof (buffer), "%s%s%s", string, group->parameter [0], str_ptr);
						break;
					case 12:		//---- param1 ----
						str_fmt (buffer, sizeof (buffer), "%s%s%s", string, group->parameter [1], str_ptr);
						break;
					case 13:		//---- param2 ----
						str_fmt (buffer, sizeof (buffer), "%s%s%s", string, group->parameter [2], str_ptr);
						break;
					case 14:		//---- param3 ----
						str_fmt (buffer, sizeof (buffer), "%s%s%s", string, group->parameter [3], str_ptr);
						break;
					case 15:		//---- comment ----
						if (group->comment_flag [0]) {
							str_fmt (buffer, sizeof (buffer), "%s##%s", string, str_ptr);
						} else {
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 16:		//---- comment1 ----
						if (group->comment_flag [1]) {
							str_fmt (buffer, sizeof (buffer), "%s##%s", string, str_ptr);
						} else {
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 17:		//---- comment2 ----
						if (group->comment_flag [2]) {
							str_fmt (buffer, sizeof (buffer), "%s##%s", string, str_ptr);
						} else {
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 18:		//---- comment3 ----
						if (group->comment_flag [3]) {
							str_fmt (buffer, sizeof (buffer), "%s##%s", string, str_ptr);
						} else {
							str_fmt (buffer, sizeof (buffer), "%s%s", string, str_ptr);
						}
						break;
					case 19:		//---- splits ----
						str_fmt (buffer, sizeof (buffer),  "%s%s%s", string, num_splits, str_ptr);
						break;
					default:
						break;
				}
				str_cpy (string, sizeof (string), buffer);
				found = true;
			}
			if (!found) {
				at = strchr (string, '@');
				str_cpy (buffer, sizeof (buffer), at);
				at = strchr (buffer + 1, '@');
				*(at+1) = '\0';

				Error ("Command %s was Not Recognized", buffer);
			}
		}
		control_file.Write (string);
		count++;
	}

	Write (1, "Number of Control Records = %d", count);

	group->master_file->Rewind ();
	control_file.Close ();
}
