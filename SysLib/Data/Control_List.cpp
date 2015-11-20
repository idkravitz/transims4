//*********************************************************
//	Control_List.cpp - list of control keys
//*********************************************************

#include "Control_List.hpp"

#include "Db_File.hpp"
#include "Utility.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//---------------------------------------------------------
//	Control_List - constructor
//---------------------------------------------------------

Control_List::Control_List (void) : Data_Array (sizeof (Control), 4)
{
	skip_offset = 0;
}

//---------------------------------------------------------
//	Control List - destructor
//---------------------------------------------------------

Control_List::~Control_List (void)
{
	Control *rec_ptr;

	for (rec_ptr = (Control *) First (); rec_ptr; rec_ptr = (Control *) Next ()) {
		if (rec_ptr->nest) {
			delete rec_ptr->group;
		} else if (rec_ptr->line != NULL) {
			delete [] rec_ptr->line;
			rec_ptr->line = NULL;
		}
		if (rec_ptr->key != NULL) {
			delete [] rec_ptr->key;
			rec_ptr->key = NULL;
		}
	}
}

//---------------------------------------------------------
//	Clean
//---------------------------------------------------------

void Control_List::Clean (void)
{
	Control *rec_ptr;

	for (rec_ptr = (Control *) First (); rec_ptr; rec_ptr = (Control *) Next ()) {
		if (rec_ptr->nest) {
			rec_ptr->group->Clean ();
		} else if (rec_ptr->line != NULL) {
			delete [] rec_ptr->line;
			rec_ptr->line = NULL;
		}
	}
}

//---------------------------------------------------------
//	Read_Control_File
//---------------------------------------------------------

bool Control_List::Read_Control_File (char *control_file) 
{
	int len;
	bool found;
	char *line, *ptr, *key_ptr, key [FIELD_BUFFER], level [16];
	Control *rec_ptr;

	Db_File file;
	bool message_flag = file.Send_Messages ();

	//---- open the control file ----

	file.Delimiters (" \t=");
	file.Send_Messages (false);

	if (!file.Open (control_file)) return (false);

	file.Send_Messages (message_flag);
	Clean ();

	//---- read the control file ----

	skip_offset = 0;

	while (file.Read ()) {

		//---- remove comments ----

		line = file.Clean_Record ();

		if (line == NULL || *line == '\0') continue;

		while (*line == ' ' || *line == '\t') {
			line++;
		}

		//---- get the key and key value offset ----

		line = file.Parse_String (line, key, sizeof (key));

		if (strlen (key) == 0) continue;

		if (line == NULL) {
			line = "";
		} else {
			while (*line == ' ' || *line == '\t' || *line == '=') {
				line++;
			}
			if (str_cmp (line, "NULL") == 0) {
				line = "";
			}
		}

		//---- get the key value ----

		found = false;

		for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {
			if (!rec_ptr->active) continue;

			if (!rec_ptr->nest) {

				if (str_cmp (rec_ptr->key, key) == 0) {
					found = true;
					if (rec_ptr->line != NULL) {
						delete [] rec_ptr->line;
						rec_ptr->line = NULL;
					}
					len = (int) strlen (line);
					if (len++ > 0) {
						rec_ptr->line = new char [len];
						if (rec_ptr->line == NULL) return (false);
						str_cpy (rec_ptr->line, len, line);
					}
					break;
				}
				continue;
			}

			//---- check for key groups ----

			len = (int) strlen (rec_ptr->key);

			if (str_cmp (rec_ptr->key, key, len - 1) == 0) {
				found = true;
				key_ptr = key + len - 1;

				for (;;) {
					ptr = level;
					while (*key_ptr != '_' && *key_ptr != '\0') *ptr++ = *key_ptr++;

					if (*key_ptr == '_') {
						*ptr++ = *key_ptr++;
						*ptr++ = '*';
					}
					*ptr = '\0';
					if (strlen (level) == 0) break;

					Control_List *list = rec_ptr->group;

					list->Add_Control_Key (level);

					for (rec_ptr = (Control *) list->First (); rec_ptr != NULL; rec_ptr = (Control *) list->Next ()) {
						if (str_cmp (rec_ptr->key, level) == 0) {
							if (!rec_ptr->nest) {
								if (rec_ptr->line != NULL) {
									delete [] rec_ptr->line;
									rec_ptr->line = NULL;
								}
								len = (int) strlen (line);
								if (len++ > 0) {
									rec_ptr->line = new char [len];
									if (rec_ptr->line == NULL) return (false);
									str_cpy (rec_ptr->line, len, line);
								}
							}
							break;
						}
					}
				}
			}
		}
		
		//---- add the key to the list of unrecognized keys ----

		if (!found && key [0] != '#' && strlen (line) != 0) {
			len = (int) strlen (key);

			rec_ptr = (Control *) New_Record (true);
			if (rec_ptr == NULL) return (false);

			rec_ptr->key = new char [len + 1];
			if (rec_ptr->key == NULL) return (false);

			str_cpy (rec_ptr->key, len + 1, key);

			rec_ptr->nest = false;
			rec_ptr->active = false;
			rec_ptr->line = NULL;

			if (!Add ()) return (false);

			if (skip_offset == 0) {
				skip_offset = Num_Records ();
			}
		}
	}
	file.Close ();

	return (true);
}

//-----------------------------------------------------------
//	Add_Control_Key - add a key to the list 
//-----------------------------------------------------------

void Control_List::Add_Control_Key (char *key) 
{
	int len;
	Control *rec_ptr;

	if (key == NULL) return;

	for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {
		if (str_cmp (rec_ptr->key, key) == 0) return;
	}
	len = (int) strlen (key);

	rec_ptr = (Control *) New_Record (true);
	if (rec_ptr == NULL) return;

	rec_ptr->key = new char [len + 1];
	if (rec_ptr->key == NULL) return;
	str_cpy (rec_ptr->key, len + 1, key);

	rec_ptr->active = true;
	rec_ptr->nest = (key [len - 1] == '*');

	if (rec_ptr->nest) {
		rec_ptr->group = new Control_List ();
	} else {
		rec_ptr->line = NULL;
	}
	Add ();
}

//-----------------------------------------------------------
//	Get_Control_String - search for a specified record 
//-----------------------------------------------------------

char * Control_List::Get_Control_String (char *key, ...) 
{
	Control *rec_ptr;

	if (key == NULL) return (NULL);
	Current_Key (key);

	for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {

		if (!rec_ptr->active) continue;

		if (str_cmp (rec_ptr->key, key) == 0) {

			if (rec_ptr->nest) {

				int index;
				char level [16];
				va_list args;

				//--- search for each index ----

				va_start (args, key);

				while (rec_ptr->nest) {
					index = va_arg (args, int);

					str_fmt (level, sizeof (level), "%d", index);

					Control_List *list = rec_ptr->group;

					rec_ptr = (Control *) list->First ();

					if (rec_ptr == NULL) break;

					if (rec_ptr->nest) {
						str_cat (level, sizeof (level), "_*");
					}
					for (; rec_ptr != NULL; rec_ptr = (Control *) list->Next ()) {
						if (str_cmp (rec_ptr->key, level) == 0) break;
					}
					if (rec_ptr == NULL) break;
				}
				va_end (args);
			}
			if (rec_ptr == NULL) break;
			return (rec_ptr->line);
		}
	}
	return (NULL);
}

//-----------------------------------------------------------
//	Get_Control_Flag - search for a flag parameter 
//-----------------------------------------------------------

bool Control_List::Get_Control_Flag (char *key, ...) 
{
	Control *rec_ptr;

	if (key == NULL) return (false);

	Current_Key (key);

	for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {

		if (!rec_ptr->active) continue;

		if (str_cmp (rec_ptr->key, key) == 0) {

			if (rec_ptr->nest) {

				int index;
				char level [16];
				va_list args;

				//--- search for each index ----

				va_start (args, key);

				while (rec_ptr->nest) {
					index = va_arg (args, int);

					str_fmt (level, sizeof (level), "%d", index);

					Control_List *list = rec_ptr->group;

					rec_ptr = (Control *) list->First ();

					if (rec_ptr == NULL) break;

					if (rec_ptr->nest) {
						str_cat (level, sizeof (level), "_*");
					}
					for (; rec_ptr != NULL; rec_ptr = (Control *) list->Next ()) {
						if (str_cmp (rec_ptr->key, level) == 0) break;
					}
					if (rec_ptr == NULL) break;
				}
				va_end (args);
			}	
			if (rec_ptr == NULL || rec_ptr->line == NULL) break;

            char flag = rec_ptr->line [0];
			return (flag == 'Y' || flag == 'y' || flag == 't' || flag == 'T' || flag == '1');
		}
	}
	return (false);
}

//-----------------------------------------------------------
//	Number_Control_Group - the number of records in a group 
//-----------------------------------------------------------

int Control_List::Number_Control_Group (char *key, ...) 
{
	Control *rec_ptr;

	if (key == NULL) return (0);

	for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {
		
		if (!rec_ptr->active) continue;

		if (str_cmp (rec_ptr->key, key) == 0) {
			if (key [strlen (key) - 1] == '*') {
				int i1, i2;
				char level [16];
				va_list args;

				//--- search for each index ----

				va_start(args, key);

				for (i1 = va_arg(args, int); i1 >= 0; i1 = i2) {
					Control_List *list = rec_ptr->group;

					if (i1 == 0) {
						return (list->Num_Records ());
					}
					i2 = va_arg(args, int);

					str_fmt (level, sizeof (level), "%d", i1);
					if (i2 >= 0) {
						str_cat (level, sizeof (level), "_*");
					}
					for (rec_ptr = (Control *) list->First (); rec_ptr != NULL; rec_ptr = (Control *) list->Next ()) {
						if (str_cmp (rec_ptr->key, level) == 0) break;
					}
					if (rec_ptr == NULL) break;
				}
				va_end(args);
			} else {
				return (1);
			}
		}
	}
	return (0);
}

//-----------------------------------------------------------
//	Highest_Control_Group - the highest group number 
//-----------------------------------------------------------

int Control_List::Highest_Control_Group (char *key, ...) 
{
	Control *rec_ptr;

	if (key == NULL) return (0);

	for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {

		if (!rec_ptr->active) continue;

		if (str_cmp (rec_ptr->key, key) == 0) {
			if (key [strlen (key) - 1] == '*') {
				int i1, i2;
				char level [16];
				va_list args;

				//--- search for each index ----

				va_start(args, key);

				for (i1 = va_arg(args, int); i1 >= 0; i1 = i2) {
					Control_List *list = rec_ptr->group;

					if (i1 == 0) {
						int high = 0;
						for (rec_ptr = (Control *) list->First (); rec_ptr != NULL; rec_ptr = (Control *) list->Next ()) {
							int code = atoi (rec_ptr->key);
							if (code > high) high = code;
						}
						return (high);
					}
					i2 = va_arg(args, int);

					str_fmt (level, sizeof (level), "%d", i1);
					if (i2 >= 0) {
						str_cat (level, sizeof (level), "_*");
					}
					for (rec_ptr = (Control *) list->First (); rec_ptr != NULL; rec_ptr = (Control *) list->Next ()) {
						if (str_cmp (rec_ptr->key, level) == 0) break;
					}
					if (rec_ptr == NULL) break;
				}
				va_end(args);
			} else {
				return (1);
			}
		}
	}
	return (0);
}

//-----------------------------------------------------------
//	First_Key - show the first program key 
//-----------------------------------------------------------

char * Control_List::First_Key (void) 
{
	Control *rec_ptr;
	
	for (rec_ptr = (Control *) First (); rec_ptr; rec_ptr = (Control *) Next ()) {
		if (rec_ptr->active) break;
	}
	if (rec_ptr != NULL) {
		return (rec_ptr->key);
	} else {
		return (NULL);
	}
}

//-----------------------------------------------------------
//	Next_Key - show the other program keys 
//-----------------------------------------------------------

char * Control_List::Next_Key (void) 
{
	Control *rec_ptr;
	
	while ((rec_ptr = (Control *) Next ()) != NULL) {
		if (rec_ptr->active) break;
	}
	if (rec_ptr != NULL) {
		return (rec_ptr->key);
	} else {
		return (NULL);
	}
}

//-----------------------------------------------------------
//	First_Skip_Key - return the first skipped control key 
//-----------------------------------------------------------

char * Control_List::First_Skip_Key (void) 
{
	Control *rec_ptr = (Control *) Record (skip_offset);

	if (rec_ptr != NULL) {
		return (rec_ptr->key);
	} else {
		return (NULL);
	}
}

//-----------------------------------------------------------
//	Next_Skip_Key - return the other skipped control keys 
//-----------------------------------------------------------

char * Control_List::Next_Skip_Key (void) 
{
	Control *rec_ptr = (Control *) Next ();

	if (rec_ptr != NULL) {
		return (rec_ptr->key);
	} else {
		return (NULL);
	}
}

//-----------------------------------------------------------
//	Disable_Control_Key - make key inactive 
//-----------------------------------------------------------

void Control_List::Disable_Control_Key (char *key) 
{
	Control *rec_ptr;

	if (key == NULL) return;

	for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {
		if (str_cmp (rec_ptr->key, key) == 0) {
			rec_ptr->active = false;
		}
	}
}

//-----------------------------------------------------------
//	Activate_Control_Key - make key active 
//-----------------------------------------------------------

void Control_List::Activate_Control_Key (char *key) 
{
	Control *rec_ptr;

	if (key == NULL) return;

	for (rec_ptr = (Control *) First (); rec_ptr != NULL; rec_ptr = (Control *) Next ()) {
		if (str_cmp (rec_ptr->key, key) == 0) {
			rec_ptr->active = true;
		}
	}
}
