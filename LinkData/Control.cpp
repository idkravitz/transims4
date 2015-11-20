//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LinkData::Program_Control (void)
{
	char *str_ptr;

	//---- open control file ----

	Execution_Service::Program_Control ();

	//---- open the link node equivalence ----

	str_ptr = Get_Control_String (LINK_NODE_EQUIVALENCE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	link_node.File_Type ("Link Node Equivalence");
	link_node.Open (Project_Filename (str_ptr));

	//---- open the directional data file ----

	str_ptr = Get_Control_String (DIRECTIONAL_DATA_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	dir_file.File_Type ("Directional Data File");
	dir_file.Dbase_Format ("CSV_DELIMITED");
	dir_file.Open (Project_Filename (str_ptr));

	//---- get the from node field name ----

	str_ptr = Get_Control_String ("FROM_NODE_FIELD_NAME");

	if (str_ptr == NULL) {
		str_ptr = "A";
	}
	Print (2, "From Node Field Name = %s", str_ptr);

	from_field = dir_file.Required_Field (str_ptr);

	//---- get the to node field name ----

	str_ptr = Get_Control_String ("TO_NODE_FIELD_NAME");

	if (str_ptr == NULL) {
		str_ptr = "B";
	}
	Print (1, "To Node Field Name = %s", str_ptr);

	to_field = dir_file.Required_Field (str_ptr);

	//---- get the volume field name ----

	str_ptr = Get_Control_String ("VOLUME_DATA_FIELD_NAME");

	if (str_ptr == NULL) {
		str_ptr = "VOL";
	}
	Print (1, "Volume Data Field Name = %s", str_ptr);

	vol_field = dir_file.Required_Field (str_ptr);

	//---- get the speed field name ----

	str_ptr = Get_Control_String ("SPEED_DATA_FIELD_NAME");

	if (str_ptr == NULL) {
		speed_flag = false;
	} else {
		Print (1, "Speed Data Field Name = %s", str_ptr);

		spd_field = dir_file.Required_Field (str_ptr);
		speed_flag = true;
	}

	//---- open the new link data file ----

	str_ptr = Get_Control_String (NEW_LINK_DATA_FILE);

	if (str_ptr == NULL) goto control_error;
	
	Print (1);
	data_file.File_Type ("New Link Data File");
	data_file.File_Access (Db_File::CREATE);
	data_file.Dbase_Format ("TAB_DELIMITED");
	data_file.Open (Project_Filename (str_ptr));

	data_file.Add_Field ("LINK", Db_Code::INTEGER, 10);
	data_file.Add_Field ("ANODE", Db_Code::INTEGER, 10);
	data_file.Add_Field ("BNODE", Db_Code::INTEGER, 10);

	//---- get the field names ----

	str_ptr = Get_Control_String (NEW_AB_VOLUME_FIELD_NAME);

	if (str_ptr == NULL) {
		str_ptr = "AB_VOLUME";
	}
	Print (2, "New AB Volume Field Name = %s", str_ptr);

	data_file.Add_Field (str_ptr, Db_Code::INTEGER, 10);
	ab_vol_field = data_file.Num_Fields ();

	str_ptr = Get_Control_String (NEW_BA_VOLUME_FIELD_NAME);

	if (str_ptr == NULL) {
		str_ptr = "BA_VOLUME";
	}
	Print (1, "New BA Volume Field Name = %s", str_ptr);

	data_file.Add_Field (str_ptr, Db_Code::INTEGER, 10);
	ba_vol_field = data_file.Num_Fields ();

	//---- speed field names ----

	if (speed_flag) {
		str_ptr = Get_Control_String (NEW_AB_SPEED_FIELD_NAME);

		if (str_ptr == NULL) {
			str_ptr = "AB_SPEED";
		}
		Print (1, "New AB Speed Field Name = %s", str_ptr);

		data_file.Add_Field (str_ptr, Db_Code::INTEGER, 10);
		ab_spd_field = data_file.Num_Fields ();

		str_ptr = Get_Control_String (NEW_BA_SPEED_FIELD_NAME);

		if (str_ptr == NULL) {
			str_ptr = "BA_SPEED";
		}
		Print (1, "New BA Speed Field Name = %s", str_ptr);

		data_file.Add_Field (str_ptr, Db_Code::INTEGER, 10);
		ba_spd_field = data_file.Num_Fields ();
	}
	data_file.Write_Header ();

	//---- open the record equiv file ----

	str_ptr = Get_Control_String (NEW_DIRECTIONAL_DATA_FILE);

	if (str_ptr != NULL) {
		Print (1);
		new_dir_file.File_Type ("New Directional Data File");

		new_dir_file.Add_Field ("LINK", Db_Code::INTEGER, 10);
		new_dir_file.Add_Field ("DIR", Db_Code::INTEGER, 1);

		new_dir_file.Replicate_Fields (&dir_file);

		new_dir_file.Create (Project_Filename (str_ptr));
		output_flag = true;
	}

	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
