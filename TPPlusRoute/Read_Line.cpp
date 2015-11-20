//*********************************************************
//	Read_Line.cpp - convert the transit line data
//*********************************************************

#include "TPPlusRoute.hpp"

#include "Utility.hpp"
	
//---------------------------------------------------------
//	Read_Line
//---------------------------------------------------------

void TPPlusRoute::Read_Line (void)
{
	int i, freq;
	char *str_ptr, *ptr, buffer [FIELD_BUFFER];

	File_Group *group_ptr;
	Node_Map *map_ptr;

	//---- read each line file ----

	for (group_ptr = (File_Group *) file_group.First (); group_ptr; group_ptr = (File_Group *) file_group.Next ()) {

		if (group_ptr->line_file == NULL) continue;

		Show_Message ("Reading %s -- Record", group_ptr->line_file->File_Type ());
		Set_Progress (100);

		flip = group_ptr->flip;
		period = 1;
		oneway = true;
		nnode = ttime = 0;
		offset = mode = -1;
		memset (headway, '\0', (num_periods+1) * sizeof (int));
		name [0] = '\0';
		str_fmt (notes, sizeof (notes), "Group %d", group_ptr->group);

		//---- process each record ----

		while (group_ptr->line_file->Read ()) {
			Show_Progress ();

			str_ptr = group_ptr->line_file->Record ();

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

				if (buffer [0] == '\0' || buffer [0] == ';') break;

				if (str_cmp (buffer, "LINE") == 0) {

					//---- process the existing line ----

					if (nnode > 1 && mode >= LOCAL_BUS) {
						Save_Route ();
					}
					period = 1;
					oneway = true;
					nnode = ttime = 0;
					offset = mode = -1;
					memset (headway, '\0', (num_periods+1) * sizeof (int));
					name [0] = '\0';
					str_fmt (notes, sizeof (notes), "Group %d", group_ptr->group);

				} else if (str_cmp (buffer, "NAME=", 5) == 0) {
					
					str_cpy (name, sizeof (name), buffer + 5);

				} else if (str_cmp (buffer, "ONEWAY=", 7) == 0) {

					oneway = (buffer [7] == 'Y' || buffer [7] == 'y' || buffer [7] == 'T' || buffer [7] == 't');

				} else if (str_cmp (buffer, "MODE=", 5) == 0) {

					mode = atoi (buffer + 5);

					if (mode > 0) {
						if (mode < num_modes) {
							mode = mode_map [mode];

							if (mode == -1) {
								mode = atoi (buffer + 5);
								Warning ("Mode %d was Not Converted", mode);
								mode_map [mode] = -2;
								mode = -1;
							}
						} else {
							mode = -1;
						}
					}

				} else if (str_cmp (buffer, "FREQ", 4) == 0) {

					if (buffer [4] == '[') {
						period = atoi (buffer + 5);
					} else {
						period = 1;
					}
					ptr = strchr (buffer, '=');

					if (ptr != NULL) {
						freq = atoi (ptr + 1) * 60;

						for (i=1; i <= num_periods; i++) {
							if (group_ptr->period_map [i] == period) {
								headway [i] = freq;
							}
						}
					}
				} else if (str_cmp (buffer, "RUNTIME=", 8) == 0) {

					ttime = atoi (buffer + 8) * 60;

				} else if (str_cmp (buffer, "RT=", 3) == 0) {

					ttime = atoi (buffer + 3) * 60;

				} else if (str_cmp (buffer, "OFFSET=", 7) == 0) {

					offset = atoi (buffer + 7);
					if (offset > 0) offset *= 60;

				} else if ((buffer [0] >= '0' && buffer [0] <= '9') || buffer [0] == '-') {

					i = atoi (buffer);

					if (map_flag) {
						if (i < 0) {
							i = -i;
							map_ptr = (Node_Map *) node_map.Get (&i);
							if (map_ptr != NULL) {
								i = map_ptr->new_node;
							}
							i = -i;
						} else {
							map_ptr = (Node_Map *) node_map.Get (&i);
							if (map_ptr != NULL) {
								i = map_ptr->new_node;
							}
						}
					}
					node [nnode++] = i;

				} else if (str_cmp (buffer, "NODES=", 6) == 0 || str_cmp (buffer, "N=", 2) == 0) {
					ptr = strchr (buffer, '=') + 1;

					if ((*ptr >= '0' && *ptr <= '9') || *ptr == '-') {
						i = atoi (ptr);

						if (map_flag) {
							if (i < 0) {
								i = -i;
								map_ptr = (Node_Map *) node_map.Get (&i);
								if (map_ptr != NULL) {
									i = map_ptr->new_node;
								}
								i = -i;
							} else {
								map_ptr = (Node_Map *) node_map.Get (&i);
								if (map_ptr != NULL) {
									i = map_ptr->new_node;
								}
							}
						}
						node [nnode++] = i;
					}
				}
			}
		}
		End_Progress ();

		if (nnode > 1 && mode >= LOCAL_BUS) {
			Save_Route ();
		}
		group_ptr->line_file->Close ();
	}
}
