//*********************************************************
//	Link_Process.cpp - read the street names
//*********************************************************

#include "Validate.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool Validate::Link_Processing (Db_File *file)
{
	static int name;
	static bool name_flag;

	if (Network_Service::Link_Processing (file)) {
        
		//---- initialize the data processing ----

		if (First_Record ()) {
			name_flag = false;

			if (Report_Flag (GROUP_DETAILS) || Report_Flag (TURN_MOVEMENT)) {

				//---- optional fields ----

				name = ((Db_Base *) file)->Optional_Field ("STREET", "NAME");

				if (name > 0) {

					//---- reserve memory ----

					street_name = new char * [link_data.Max_Records ()];

					if (street_name != NULL) {
						memset (street_name, '\0', link_data.Max_Records () * sizeof (char *));
						name_flag = true;
					}
				}
			}

		} 

		//---- get the street name ----
		
		if (name_flag) {
			int link, len;
			char svalue [FIELD_BUFFER];

			link = link_data.Num_Records ();

			((Db_Base *) file)->Get_Field (name, svalue);

			if (svalue [0] == '\0') {
				return (true);
			} else if (svalue [0] >= '0' && svalue [0] <= '9') {
				if (strchr (svalue, '+') != NULL) return (true);
			} else if (str_cmp (svalue, "name") == 0) {
				return (true);
			} else if (str_cmp (svalue, "Noname") == 0) {
				return (true);
			}
			len = (int) strlen (svalue);
			if (len > 25) len = 25;

			street_name [link] = new char [len+1];

			if (street_name [link] != NULL) {
				str_cpy (street_name [link], len+1, svalue, len);
			}
		}
		return (true);
	}
	return (false);
}
