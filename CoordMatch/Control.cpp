//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "CoordMatch.hpp"

#include "Time_Range.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void CoordMatch::Program_Control (void)
{
	char *str_ptr;

	//---- open network files ----

	Network_Service::Program_Control ();

	new_node_flag = Network_File_Flag (NEW_NODE);

	if (new_node_flag) {
		replace_flag = Get_Control_Flag (REPLACE_NODE_NUMBERS);
	}

	//---- reference node file ----

	str_ptr = Get_Control_String (REFERENCE_NODE_FILE);

	if (str_ptr != NULL) {
		Print (1);
		ref_node_file.File_Type ("Reference Node File"); 

		ref_node_file.Open (Project_Filename (str_ptr));
		ref_node_flag = true;
	}

	//---- node map ----

	str_ptr = Get_Control_String (NODE_MAP_FILE);

	if (str_ptr != NULL) {
		Print (1);
		node_map_file.File_Type ("Node Map File"); 

		node_map_file.Open (Project_Filename (str_ptr));
		node_map_flag = true;

		node_map_file.Required_Field ("NODE", "CURRENT", "IN", "IN_NODE", "OLD");

		if (ref_node_flag || replace_flag) {
			node_map_file.Required_Field ("NEW_NODE", "REFERENCE", "NEW", "OUT_NODE", "OUT");
		}
		if (!ref_node_flag) {
			node_map_file.Required_Field ("X_COORD", "EASTING", "X");
			node_map_file.Required_Field ("Y_COORD", "NORTHING", "Y");
		}
	}

	if (!ref_node_flag && !node_map_flag) {
		Error ("A Reference Node File and/or a Node Map File are Required");
	}

	//---- open the output node map file ----

	str_ptr = Get_Control_String (NEW_NODE_MAP_FILE);

	if (str_ptr != NULL) {
		if (!ref_node_flag) {
			Error ("A New Node Map requires a Reference Node File");
		}
		Print (1);
		new_map_file.File_Type ("New Node Map File");
		new_map_file.File_Access (CREATE);

		new_map_file.Open (Project_Filename (str_ptr));
		new_map_flag = true;

		new_map_file.Add_Field ("NODE", INTEGER, 10);
		new_map_file.Add_Field ("NEW_NODE", INTEGER, 10);

		new_map_file.Write_Fields ();
		new_map_file.Write_Header ();
	}

	//---- get the maximum snap distance ----

	str_ptr = Get_Control_String (MAXIMUM_SNAP_DISTANCE);

	if (str_ptr != NULL) {
		max_distance = atoi (str_ptr);

		if (max_distance < 0 || max_distance > 2000) {
			Error ("Maximum Snap Distance %d is Out of Range (0..2000)", max_distance);
		}
	}
	Print (2, "Maximum Snap Distance = %d meters", max_distance);
	
	//---- replace node numbers ----

	if (new_node_flag) {
		str_ptr = Get_Control_String (REPLACE_NODE_NUMBERS);

		if (str_ptr != NULL) {
			Print (2, "Replace Node Numbers = %s", str_ptr);

			replace_flag = Get_Control_Flag (REPLACE_NODE_NUMBERS);
		}
	}
} 
