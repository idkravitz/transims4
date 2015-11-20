//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TPPlusRoute.hpp"

#include "Time_Range.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TPPlusRoute::Program_Control (void)
{
	int i, j, num;
	char *str_ptr, buffer [40];

	File_Group group_rec;
	Time_Range time_period;

	//---- open network files ----

	Network_Service::Program_Control ();

	node_flag = Network_File_Flag (NODE);
	link_flag = Network_File_Flag (LINK);

	if (Network_File_Flag (ROUTE_HEADER) && Network_File_Flag (ROUTE_NODES)) {

		input_flag = true;
		Network_Copy_Header (ROUTE_HEADER, NEW_ROUTE_HEADER);
		Network_Copy_Header (ROUTE_NODES, NEW_ROUTE_NODES);

		header_file = (Route_Header_File *) Network_Db_File (NEW_ROUTE_HEADER);
		nodes_file = (Route_Nodes_File *) Network_Db_File (NEW_ROUTE_NODES);

		num_periods = header_file->Periods ();
		time_flag = header_file->Time_Flag ();

	} else if (Network_File_Flag (ROUTE_HEADER) || Network_File_Flag (ROUTE_NODES)) {

		Error ("Route Header and Nodes files must both be provided");

	} else {

		//---- get the time periods ----

		str_ptr = Get_Control_String (TRANSIT_TIME_PERIODS);

		if (str_ptr == NULL) {
			str_ptr = "None";
		}
		if (!time_period.Add_Breaks (str_ptr)) {
			Error ("Adding Transit Time Period Breaks");
		}
		num_periods = time_period.Num_Ranges ();

		Print (2, "Transit Time Period Breaks = %s", str_ptr);

		//---- get the period travel time flag ----

		str_ptr = Get_Control_String (PERIOD_TRAVEL_TIMES);

		if (str_ptr != NULL) {
			Print (1, "Period Travel Times = %s", str_ptr);

			time_flag = Get_Control_Flag (PERIOD_TRAVEL_TIMES);
		}

		//---- update the file header ----

		header_file = (Route_Header_File *) Network_Db_File (NEW_ROUTE_HEADER);

		header_file->Create_Fields (num_periods, time_flag);
		header_file->Write_Header ();

		nodes_file = (Route_Nodes_File *) Network_Db_File (NEW_ROUTE_NODES);

		nodes_file->Dwell_Flag (false);
		nodes_file->Time_Flag (false);
		nodes_file->Speed_Flag (false);

		nodes_file->Write_Header ();
	}

	//---- allocate headway space ----

	headway = new int [num_periods+1];

	//---- first route number ----

	str_ptr = Get_Control_String (FIRST_ROUTE_NUMBER);

	if (str_ptr != NULL) {
		route_id = atoi (str_ptr);

		if (route_id < 1) {
			Error ("First Route Number %d is Out of Range (>0)", route_id);
		}
		Print (2, "First Route Number = %d", route_id);
	}

	//---- determine the number of file groups ----

	num = Highest_Control_Group (TPPLUS_LINE_FILE_x, 0);

	if (num == 0) {
		Error ("Missing Control Key = %s", TPPLUS_LINE_FILE_x);
	}

	for (i=1; i <= num; i++) {

		//---- open the TP+ link file ----
		
		str_ptr = Get_Control_String (TPPLUS_LINE_FILE_x, i);

		if (str_ptr == NULL) continue;

		group_rec.group = i;
		group_rec.line_file = new Db_File ();
		group_rec.period_map = new int [num_periods+1];
		group_rec.flip = false;

		if (group_rec.line_file == NULL || group_rec.period_map == NULL) {
			Error ("Creating a Line Group");
		}
		Print (1);
		str_fmt (buffer, sizeof (buffer), "TPPlus Line File %d", i); 

		group_rec.line_file->File_Type (buffer);

		group_rec.line_file->Open (Project_Filename (str_ptr));

		//---- process the period map ----

		str_ptr = Get_Control_String (TPPLUS_PERIOD_MAP_x, i);

		memset (group_rec.period_map, '\0', (num_periods+1) * sizeof (int));

		if (str_ptr != NULL) {
			for (j=1; j <= num_periods && str_ptr; j++) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

				if (buffer [0] == '\0') break;

				group_rec.period_map [j] = atoi (buffer);
			}
		} else {
			group_rec.period_map [1] = 1;
		}
		Print (1, "TPPlus Period Map %d = ", i);

		for (j=1; j <= num_periods; j++) {
			if (j == 1) {
				Print (0, "%d", group_rec.period_map [j]);
			} else {
				Print (0, ", %d", group_rec.period_map [j]);
			}
		}

		//---- check the flip flag ----

		str_ptr = Get_Control_String (TPPLUS_FLIP_ROUTE_x, i);

		if (str_ptr != NULL) {
			Print (1, "TPPlus Flip Route %d = %s", i, str_ptr);

			group_rec.flip = Get_Control_Flag (TPPLUS_FLIP_ROUTE_x, i);
		}

		//---- save the record ----

		if (!file_group.Add (&group_rec)) {
			Error ("Adding File Group");
		}
	}

	//---- tp plus mode map ----

	num = Highest_Control_Group (TPPLUS_MODE_MAP_x, 0);

	if (num == 0) {
		Error ("Missing Control Key = %s", TPPLUS_MODE_MAP_x);
	}

	//--- count the number of modes ----

	for (i=1; i <= num; i++) {
		str_ptr = Get_Control_String (TPPLUS_MODE_MAP_x, i);
		j = i;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] == '\0') break;

			j++;
		}
		if (j > num_modes) num_modes = j;
	}

	//---- allocate map space ----

	mode_map = new int [num_modes];

	if (mode_map == NULL) {
		Error ("Insufficient Memory for %d Mode Maps", num_modes);
	}
	for (i=0; i < num_modes; i++) {
		mode_map [i] = -1;
	}
	Print (1);

	//---- fill the mode map ----

	for (i=1; i <= num; i++) {
		str_ptr = Get_Control_String (TPPLUS_MODE_MAP_x, i);

		if (str_ptr != NULL) {
			Print (1, "TPPlus Mode Map %d = %s", i, str_ptr);
			j = i;

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

				if (buffer [0] == '\0') break;

				mode_map [j++] = Transit_Code (buffer);
			}
		} 
	}

	//---- node map ----

	str_ptr = Get_Control_String (TPPLUS_NODE_MAP_FILE);

	if (str_ptr != NULL) {
		Print (1);
		map_flag = true;
		node_map_file.File_Type ("TPPlus Node Map File"); 

		node_map_file.Open (Project_Filename (str_ptr));

		node_map_file.Required_Field ("NODE", "TPPLUS", "OLD", "OLD_NODE", "IN");
		node_map_file.Required_Field ("NEW_NODE", "TRANSIMS", "NEW", "NEWNODE", "OUT");
	}
} 
