//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TransimsNet.hpp"

#include "Link_File.hpp"
#include "Zone_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TransimsNet::Program_Control (void)
{
	int i, num, lvalue;
	double dvalue;
	char *str_ptr, buffer [FIELD_BUFFER];

	Area_Type *type_ptr;

	//---- open network files ----

	Network_Service::Program_Control ();

	//---- set the file flags ----

	shape_flag = Network_File_Flag (NEW_SHAPE);
	zone_flag = Network_File_Flag (ZONE);
	zout_flag = Network_File_Flag (NEW_ZONE);

	name_flag = (((Link_File *) Network_Db_Base (LINK))->Name_Flag () && 
				((Link_File *) Network_Db_Base (NEW_LINK))->Name_Flag ());

	turn_flag = Network_File_Flag (TURN_PROHIBITION);

	//---- set the zone fields ----

	if (zout_flag) {
		if (!zone_flag) {
			Print (2, "Zone Centroids Copied from Node File");
		} else {
			Zone_File *file = (Zone_File *) Network_Db_File (ZONE);
			Zone_File *new_file = (Zone_File *) Network_Db_File (NEW_ZONE);

			new_file->Clear_Fields ();
			new_file->Replicate_Fields (file);
			new_file->Write_Header ();
		}
	}

	//---- keep node list ----
	
	str_ptr = Get_Control_String (KEEP_NODE_LIST);

	if (str_ptr != NULL) {
		Print (1);
		keep_nodes.File_Type ("Keep Node List File");

		keep_nodes.Open (Network_Filename (str_ptr));
	}

	//---- link node equivalance ----
	
	str_ptr = Get_Control_String (LINK_NODE_EQUIVALENCE);

	if (str_ptr != NULL) {
		Print (1);
		node_string.File_Type ("Link Node Equivalence File");

		node_string.Create (New_Filename (str_ptr));
	}

	//---- get the pocket lane lengths ----

	Print (1);

	num = Highest_Control_Group (POCKET_LENGTHS_FOR_FACILITY_x, 0);

	if (num >= EXTERNAL) num = EXTERNAL - 1;

	for (i=1; i <= num; i++) {
		str_ptr = Get_Control_String (POCKET_LENGTHS_FOR_FACILITY_x, i);
	
		if (str_ptr != NULL) {
			Print (1, "Pocket Lengths for Facility Type %d = %s meters", i, str_ptr);

			while (str_ptr != NULL) {
				str_ptr = Get_Double (str_ptr, &dvalue);

				if (dvalue < 0.0 || dvalue > 2000.0) {
					Error ("Pocket Length %.1lf for Facility %d is Out of Range (0..2000)", dvalue, i);
				}
				if (!pocket_len [i].Add (Round (dvalue))) {
					Error ("Adding Pocket Lane Length");
				}
			}
		}
	}

	//---- get the pocket to facility ----

	str_ptr = Get_Control_String (POCKET_TO_FACILITY_BY_AREA_TYPE);

	if (str_ptr != NULL) {
		pocket_to_flag = true;

		Print (1, "Pocket To Facility Types by Area Type = %s", str_ptr);

		while (str_ptr != NULL) {
			str_ptr = Get_Integer (str_ptr, &lvalue);

			if (lvalue < 0 || lvalue > EXTERNAL) {
				Error ("Pocket To Facility %d is Out of Range (0..%d)", lvalue, EXTERNAL);
			}
			if (!pocket_to.Add (lvalue)) {
				Error ("Adding Pocket To Facility");
			}
		}
	}

	//---- get the signal warrants ----

	Print (1);

	num = Highest_Control_Group (SIGNAL_WARRANT_FOR_AREA_TYPE_x, 0);

	for (i=1; i <= num; i++) {
		str_ptr = Get_Control_String (SIGNAL_WARRANT_FOR_AREA_TYPE_x, i);

		if (str_ptr != NULL) {
			Area_Type type_rec (i);

			Print (1, "Signal Warrant for Area Type %d = %s", i, str_ptr);

            str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			type_rec.Primary (Facility_Code (buffer));

			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			type_rec.Secondary (Facility_Code (buffer));

			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (*buffer == '\0') {
				str_cpy (buffer, sizeof (buffer), "ACTUATED");
			}

			type_rec.Signal_Type (Signal_Code (buffer));

			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (*buffer == '\0') {
				str_cpy (buffer, sizeof (buffer), "SINGLE_RING");
			}

			type_rec.Ring_Type (Ring_Code (buffer));

			if (!area_type.Add (&type_rec)) goto type_error;
		}
	}

	//---- get the sign warrants ----

	Print (1);

	num = Highest_Control_Group (STOP_WARRANT_FOR_AREA_TYPE_x, 0);

	for (i=1; i <= num; i++) {
		str_ptr = Get_Control_String (STOP_WARRANT_FOR_AREA_TYPE_x, i);

		if (str_ptr != NULL) {
			type_ptr = area_type.Get (i);

			lvalue = Facility_Code (str_ptr);

			if (type_ptr == NULL) {
				Area_Type type_rec (i);
				type_rec.All_Stop (lvalue);

				if (!area_type.Add (&type_rec)) goto type_error;
			} else {
				type_ptr->All_Stop (lvalue);
			}
			Print (1, "Stop Warrant for Area Type %d = %s", i, str_ptr);
		}
	}

	//---- activity location side offset ----

	str_ptr = Get_Control_String (ACTIVITY_LOCATION_SIDE_OFFSET);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &location_side);

		if (location_side < 0.0 || location_side > 100.0) {
			Error ("Activity Location Side Offset %.2lf is Out of Range (0..100)", location_side);
		}
	}
	Print (2, "Activity Location Side Offset = %.2lf meters", location_side);

	//---- get the facility type access flags ----

	str_ptr = Get_Control_String (FACILITY_TYPE_ACCESS_FLAGS);

	if (str_ptr != NULL) {
		for (i=1; str_ptr != NULL; i++) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (*buffer == '\0') break;

			access_flag [i] = (*buffer == '1' || *buffer == 't' || *buffer == 'T' || 
				*buffer == 'y' || *buffer == 'Y');
		}
	}
	Print (2, "Facility Type Access Flags = ");

	for (i=1; i <= EXTERNAL; i++) {
		if (i > 1) {
			Print (0, ", %d", access_flag [i]);
		} else {
			Print (0, "%d", access_flag [i]);
		}
	}

	//---- get the number of access points ----

	str_ptr = Get_Control_String (MAXIMUM_ACCESS_POINTS);

	if (str_ptr == NULL) goto control_error;

	Get_Integer (str_ptr, &max_splits);

	if (max_splits < 1 || max_splits > 20) {
		Error ("Maximum Number of Access Points %d is Out of Range", max_splits);
	}
	Print (2, "Maximum Number of Access Points Per Link = %d", max_splits);

	//---- get the min split length ----

	str_ptr = Get_Control_String (MINIMUM_SPLIT_LENGTHS);

	if (str_ptr == NULL) goto control_error;

	Print (2, "Minimum Split Lengths = %s meters", str_ptr);

	for (i=1; str_ptr != NULL; i++) {
		str_ptr = Get_Integer (str_ptr, &lvalue);

		if (lvalue == 0) continue;

		if (lvalue < 40 || lvalue > 4000) {
			Error ("Minimum Split Length %d is Out of Range (40-4000)", lvalue);
		}
		lvalue = Round (lvalue);

		type_ptr = area_type.Get (i);

		if (type_ptr == NULL) {
			Area_Type type_rec (i);
			type_rec.Split_Len (lvalue);

			if (!area_type.Add (&type_rec)) goto type_error;
		} else {
			type_ptr->Split_Len (lvalue);
		}
	}

	//---- get the cell size ----

	str_ptr = Get_Control_String (CELL_SIZE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &cell_size);

		if (cell_size < 4.0 || cell_size > 9.0) {
			Error ("Cell Size %.2lf is Out of Range (4..9)", cell_size);
		}
	}
	Print (2, "Cell Size = %.2lf meters", cell_size);

	//---- get the minimum link length ----

	str_ptr = Get_Control_String (MINIMUM_LINK_LENGTH);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);

		if (dvalue < cell_size || dvalue > 100.0) {
			Error ("Minimum Link Length %.1lf is Out of Range (%.1lf..100.0)", dvalue, cell_size);
		}
	} else {
		dvalue = cell_size * 5.0;
	}
	min_length = Round (dvalue);
	Print (2, "Minimum Link Length = %.1lf meters", UnRound (min_length));

	//---- get the maximum length factor ----

	str_ptr = Get_Control_String (MAXIMUM_LENGTH_TO_XY_RATIO);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &max_length_factor);

		if (max_length_factor < 1.0 || max_length_factor > 2.0) {
			Error ("Maximum Length to XY Ratio %.1lf is Out of Range (1.0..2.0)", max_length_factor);
		}
	}
	Print (2, "Maximum Length to XY Ratio = %.2lf", max_length_factor);

	//---- get the maximum connection angle ----

	str_ptr = Get_Control_String (MAXIMUM_CONNECTION_ANGLE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);

		if (lvalue < 90 || lvalue > 180) {
			Error ("Maximum Connection Angle %d is Out of Range (90..180)", lvalue);
		}
		max_angle = compass.Num_Points () * lvalue / 360;
	}
	Print (2, "Maximum Connection Angle = %d degrees", max_angle * 360 / compass.Num_Points ());

	//---- uturn flag ----

	uturn_flag = Get_Control_Flag (ADD_UTURN_TO_DEADEND_LINKS);

	if (uturn_flag) {
		Print (1, "U-Turns will be added to Dead-End Links");
	} else {
		Print (1, "U-Turns will not be added to Dead-End Links");
	}

	//---- get the setback distance ----

	str_ptr = Get_Control_String (INTERSECTION_SETBACK_DISTANCE);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);

		if (dvalue < 0.0 || dvalue > 20.0) {
			Error ("Intersection Setback Distance %.1lf is Out of Range (0.0..20.0)", dvalue);
		}
	} else {
		dvalue = 0.0;
	}
	setback = Round (dvalue);
	Print (2, "Intersection Setback Distance = %.1lf meters", UnRound (setback));

	//---- get the external zone number ----

	str_ptr = Get_Control_String (FIRST_EXTERNAL_ZONE_NUMBER);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &ext_zone);

		if (ext_zone < 0 || ext_zone > 10000) {
			Error ("External Zone Number %d is Out of Range (0..10000)", ext_zone);
		}
	} else {
		ext_zone = 0;
	}
	Print (2, "First External Zone Number = %d", ext_zone);

	//---- get the zone connector flag ----

	str_ptr = Get_Control_String (KEEP_ZONE_CONNECTORS_AS_LOCALS);

	if (str_ptr != NULL) {
		Print (1, "Keep Zone Connectors as Locals = %s", str_ptr);

		connect_flag = Get_Control_Flag (KEEP_ZONE_CONNECTORS_AS_LOCALS);
	}

	//---- get the replicate MPO network ----

	str_ptr = Get_Control_String (REPLICATE_MPO_NETWORK);

	if (str_ptr != NULL) {
		Print (1, "Replication MPO Network = %s", str_ptr);

		replicate_flag = Get_Control_Flag (REPLICATE_MPO_NETWORK);
	}

	//---- collapse nodes flag ----

	if (Get_Control_String (COLLAPSE_NODES_FLAG) != NULL) {
		collapse_flag = Get_Control_Flag (COLLAPSE_NODES_FLAG);
	}
	if (collapse_flag) {
		Print (2, "Extra Nodes will be Removed from the Network");
	} else {
		Print (2, "All Nodes in the Network will be Kept");
	}

	//---- update node range ----

	str_ptr = Get_Control_String (UPDATE_NODE_RANGE);

	if (str_ptr != NULL) {
		update_flag = true;
		Print (2, "Update Node Range = %s", str_ptr);

		if (!update_nodes.Add_Ranges (str_ptr)) {
			Error ("Adding Update Nodes");
		}
	}

	//---- update node file ----

	str_ptr = Get_Control_String (UPDATE_NODE_FILE);

	if (str_ptr != NULL) {
		update_flag = update_node_flag = true;
		Print (1);
		update_node_file.File_Type ("Update Node File");

		if (!update_node_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", update_node_file.File_Type ());
		}
	}

	//---- update link range ----

	str_ptr = Get_Control_String (UPDATE_LINK_RANGE);

	if (str_ptr != NULL) {
		update_flag = true;
		Print (2, "Update Link Range = %s", str_ptr);

		if (!update_links.Add_Ranges (str_ptr)) {
			Error ("Adding Update Links");
		}
	}

	//---- update link file ----

	str_ptr = Get_Control_String (UPDATE_LINK_FILE);

	if (str_ptr != NULL) {
		update_flag = update_link_flag = true;
		Print (1);
		update_link_file.File_Type ("Update Link File");

		if (!update_link_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", update_link_file.File_Type ());
		}
	}

	//---- delete node range ----

	str_ptr = Get_Control_String (DELETE_NODE_RANGE);

	if (str_ptr != NULL) {
		delete_flag = true;
		Print (2, "Delete Node Range = %s", str_ptr);

		if (!delete_nodes.Add_Ranges (str_ptr)) {
			Error ("Adding Delete Nodes");
		}
	}

	//---- delete node file ----

	str_ptr = Get_Control_String (DELETE_NODE_FILE);

	if (str_ptr != NULL) {
		delete_flag = delete_node_flag = true;
		Print (1);
		delete_node_file.File_Type ("Delete Node File");

		if (!delete_node_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", delete_node_file.File_Type ());
		}
	}

	//---- delete link range ----

	str_ptr = Get_Control_String (DELETE_LINK_RANGE);

	if (str_ptr != NULL) {
		delete_flag = true;
		Print (2, "Delete Link Range = %s", str_ptr);

		if (!delete_links.Add_Ranges (str_ptr)) {
			Error ("Adding Delete Links");
		}
	}

	//---- delete link file ----

	str_ptr = Get_Control_String (DELETE_LINK_FILE);

	if (str_ptr != NULL) {
		delete_flag = delete_link_flag = true;
		Print (1);
		delete_link_file.File_Type ("Delete Link File");

		if (!delete_link_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", delete_link_file.File_Type ());
		}
	}

	//---- link detail file ----

	str_ptr = Get_Control_String (LINK_DETAIL_FILE);

	if (str_ptr != NULL) {
		Print (1);
		detail_flag = true;

		if (!detail_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", detail_file.File_Type ());
		}
		lane_use_flag = Network_File_Flag (NEW_LANE_USE);
	}

	//---- get the projection data ----

	projection.Read_Control ();

	coord.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

	Set_Projection (&coord);

	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

type_error:
	Error ("Adding Area Type Record");
} 
