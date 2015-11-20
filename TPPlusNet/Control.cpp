//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TPPlusNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TPPlusNet::Program_Control (void)
{
	int len;
	char *str_ptr, *format_ptr;

	//---- create the network files ----

	Network_Service::Program_Control ();

	//---- get the projection data ----

	proj_service.Read_Control ();

	projection.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

	//---- open the TP+ link file ----
	
	str_ptr = Get_Control_String (TPPLUS_LINK_FILE);

	if (str_ptr == NULL) goto control_error;

	len = (int) strlen (str_ptr);
	len = (len > 4) ? len - 4 : 0;

	if ((format_ptr = strchr (str_ptr + len, '.')) != NULL) {
		link_shape_flag = (str_cmp (format_ptr, ".shp") == 0);
	}

	if (link_shape_flag) {

		//---- set the projection data ----

		link_shape_file.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

		link_shape_file.File_Type ("TPPlus Link File");
		link_shape_file.File_ID ("TPPLUS");

		Print (1);
		link_shape_file.Open (Project_Filename (str_ptr));

		link_shape_file.File_Access (MODIFY);
	} else {
		link_file.File_Type ("TPPlus Link File");
		link_file.File_ID ("TPPLUS");

		format_ptr = Get_Control_String (TPPLUS_LINK_FORMAT);

		if (format_ptr != NULL) {
			link_file.Dbase_Format (format_ptr);
		}
		Print (1);
		link_file.Open (Project_Filename (str_ptr));

		link_file.File_Access (MODIFY);
	}

	//---- open the TP+ node file ----
	
	str_ptr = Get_Control_String (TPPLUS_NODE_FILE);

	if (str_ptr == NULL) goto control_error;

	len = (int) strlen (str_ptr);
	len = (len > 4) ? len - 4 : 0;

	if ((format_ptr = strchr (str_ptr + len, '.')) != NULL) {
		node_shape_flag = (str_cmp (format_ptr, ".shp") == 0);
	}

	if (node_shape_flag) {

		//---- set the projection data ----

		node_shape_file.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

		node_shape_file.File_Type ("TPPlus Node File");
		node_shape_file.File_ID ("TPPLUS");

		Print (1);
		node_shape_file.Open (Project_Filename (str_ptr));
	} else {
		node_file.File_Type ("TPPlus Node File");
		node_file.File_ID ("TPPLUS");

		format_ptr = Get_Control_String (TPPLUS_NODE_FORMAT);

		if (format_ptr != NULL) {
			node_file.Dbase_Format (format_ptr);
		}
		Print (1);
		node_file.Open (Project_Filename (str_ptr));
	}

	//---- open the speed capacity table ----
	
	str_ptr = Get_Control_String (TPPLUS_SPDCAP_FILE);

	if (str_ptr != NULL) {
		spdcap_file.File_Type ("TPPlus Speed-Capacity File");
		spdcap_file.File_ID ("TPPLUS");

		format_ptr = Get_Control_String (TPPLUS_SPDCAP_FORMAT);

		if (format_ptr != NULL) {
			spdcap_file.Dbase_Format (format_ptr);
		} else {
			spdcap_file.Dbase_Format (FIXED_COLUMN);
		}
		Print (1);
		spdcap_file.Open (Project_Filename (str_ptr));
		spdcap_flag = true;

		//---- convert units ----

		str_ptr = Get_Control_String (CONVERT_MPH_TO_MPS);

		if (str_ptr != NULL) {
			Print (1, "Convert MPH to MPS = %s", str_ptr);

			units_flag = Get_Control_Flag (CONVERT_MPH_TO_MPS);
		}

		//---- facility index field ----

		str_ptr = Get_Control_String (FACILITY_INDEX_FIELD);

		if (str_ptr != NULL) {
			Print (1, "Facility Index Field = %s", str_ptr);

			if (link_shape_flag) {
				fac_fld = link_shape_file.Required_Field (str_ptr);
			} else {
				fac_fld = link_file.Required_Field (str_ptr);
			}
			Print (0, " (Number = %d)", fac_fld);
		}

		//---- area type index field ----

		str_ptr = Get_Control_String (AREA_TYPE_INDEX_FIELD);

		if (str_ptr != NULL) {
			Print (1, "Area Type Index Field = %s", str_ptr);

			if (link_shape_flag) {
				at_fld = link_shape_file.Required_Field (str_ptr);
			} else {
				at_fld = link_file.Required_Field (str_ptr);
			}
			Print (0, " (Number = %d)", at_fld);
		}
		if ((fac_fld == 0 && at_fld > 0) || (fac_fld > 0 && at_fld == 0)) {
			Error ("Facility and Area Types Index Fields are Required");
		}
	}

	//---- open the conversion script ----

	str_ptr = Get_Control_String (CONVERSION_SCRIPT);

	if (str_ptr != NULL) {
		Print (1);
		script_file.File_Type ("Conversion Script");

		script_file.Open (Project_Filename (str_ptr));
		convert_flag = true;
	}

	//---- external zone number ----

	str_ptr = Get_Control_String (EXTERNAL_ZONE_NUMBER);

	if (str_ptr != NULL) {
		ext_zone = atoi (str_ptr);

		if (ext_zone < 0 || ext_zone > 10000) {
			Error ("External Zone Number %d is Out of Range (0..10000)", ext_zone);
		}
		Print (2, "First External Zone Number = %d", ext_zone);
	}

	//---- open the link detail file ----

	str_ptr = Get_Control_String (NEW_LINK_DETAIL_FILE);

	if (str_ptr != NULL) {
		Print (1);
		detail_file.Notes_Flag (Notes_Name_Flag ());

		detail_file.Create (Project_Filename (str_ptr));
		detail_flag = true;
	}

	if (link_shape_flag) {
		shape_file = (Shape_File *) Network_Db_Base (NEW_SHAPE);

		if (link_shape_file.Z_Flag () && shape_file != NULL) {
			shape_file->Z_Flag (true);

			shape_file->Clear_Fields ();
			shape_file->Create_Fields ();
			shape_file->Write_Header ();
		}
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
