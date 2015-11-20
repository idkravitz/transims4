//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "GISNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void GISNet::Program_Control (void)
{
	bool first;
	char *str_ptr;

	//---- get the projection data ----

	projection.Read_Control ();

	gis_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	gis_node.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

	//---- open the arcview node file ----
	
	str_ptr = Get_Control_String (GIS_NODE_FILE);

	if (str_ptr != NULL) {
		gis_node.File_Type ("GIS Node File");
		gis_node.File_ID ("GIS");

		Print (1);
		gis_node.Open (Project_Filename (str_ptr));
		node_flag = true;
	}

	//---- open the arcview link file ----
	
	str_ptr = Get_Control_String (GIS_LINK_FILE);

	if (str_ptr == NULL) goto control_error;

	gis_file.File_Type ("GIS Link File");
	gis_file.File_ID ("GIS");

	Print (1);
	gis_file.Open (Project_Filename (str_ptr));

	//---- open the conversion script ----

	str_ptr = Get_Control_String (CONVERSION_SCRIPT);

	if (str_ptr != NULL) {
		Print (1);
		script_file.File_Type ("Conversion Script");

		script_file.Open (Project_Filename (str_ptr));
		convert_flag = true;
	}

	//---- first link number ----

	first = true;
	str_ptr = Get_Control_String (FIRST_LINK_NUMBER);

	if (str_ptr != NULL) {
		first_link = atoi (str_ptr);

		if (first_link < 1) {
			Error ("First Link Number %d is Out of Range", first_link);
		}
		Print (2, "First Link Number = %d", first_link);
		first = false;
	}

	//---- first node number ----

	str_ptr = Get_Control_String (FIRST_NODE_NUMBER);

	if (str_ptr != NULL) {
		first_node = atoi (str_ptr);

		if (first_node < 1) {
			Error ("First Node Number %d is Out of Range", first_node);
		}
		if (first) Print (1);
		Print (1, "First Node Number = %d", first_node);
	}

	//---- maximum shape angle ----

	str_ptr = Get_Control_String (MAXIMUM_SHAPE_ANGLE);

	if (str_ptr != NULL) {
		max_angle = atoi (str_ptr);

		if (max_angle != 0 && (max_angle < 5 || max_angle > 120)) {
			Error ("Maximum Shape Angle %d is Out of Range (5..120)", max_angle);
		}
	}
	if (max_angle > 0) {
		Print (2, "Maximum Shape Angle = %d degrees", max_angle);
	} else {
		Print (2, "Shape Smoothing is Disabled");
	}

	//---- minimum shape length ----

	str_ptr = Get_Control_String (MINIMUM_SHAPE_LENGTH);

	if (str_ptr != NULL) {
		min_length = atoi (str_ptr);

		if (min_length != 0 && (min_length < 1 || min_length > 50)) {
			Error ("Minimum Shape Length %d is Out of Range (1..50)", min_length);
		}
	}
	if (min_length > 0) {
		Print (1, "Minimum Shape Length = %d meters", min_length);
	} else if (max_angle != 0) {
		Print (1, "Shape Smoothing is Disabled");
	}

	//---- update link lengths ----

	str_ptr = Get_Control_String (UPDATE_LINK_LENGTHS);

	if (str_ptr != NULL) {
		Print (2, "Update Link Lengths = %s", str_ptr);

		update_flag = Get_Control_Flag (UPDATE_LINK_LENGTHS);
	}

	//---- create the network files ----

	Network_Service::Program_Control ();

	link_file = (Link_File *) Network_Db_Base (NEW_LINK);
	node_file = (Node_File *) Network_Db_Base (NEW_NODE);
	shape_file = (Shape_File *) Network_Db_Base (NEW_SHAPE);

	if (gis_file.Z_Flag () && shape_file != NULL) {
		shape_file->Z_Flag (true);

		shape_file->Clear_Fields ();
		shape_file->Create_Fields ();
		shape_file->Write_Header ();
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
