//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Progression.hpp"

#include "Time_Step.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Progression::Program_Control (void)
{
	int lvalue;
	char *str_ptr;
	Db_Header *file;

	//---- create the network files ----

	Demand_Service::Program_Control ();

	delay_flag = Demand_File_Flag (LINK_DELAY);

	//---- set the field names ----

	Network_Copy_Header (SIGNALIZED_NODE, NEW_SIGNALIZED_NODE);

	//---- setup the signal database ----

	file = (Db_Header *) Network_Db_Base (SIGNALIZED_NODE);

	signal_db.Replicate_Fields (file, true);
	signal_db.File_ID ("Signal");


	//---- open the link equivalence ----

	str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

	if (str_ptr != NULL) {
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
		equiv_flag = true;
	}

	//---- clear existing offsets ----

	str_ptr = Get_Control_String (CLEAR_EXISTING_OFFSETS);

	if (str_ptr != NULL) {
		Print (2, "Clear Existing Offsets = %s", str_ptr);

		clear_flag = Get_Control_Flag (CLEAR_EXISTING_OFFSETS);
	}

	//---- evaluate existing offsets ----

	str_ptr = Get_Control_String (EVALUATE_EXISTING_OFFSETS);

	if (str_ptr != NULL) {
		Print (2, "Evaluate Existing Offsets = %s", str_ptr);

		eval_flag = Get_Control_Flag (EVALUATE_EXISTING_OFFSETS);
	}

	//---- progression time of day ----

	str_ptr = Get_Control_String (PROGRESSION_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Progression Time Periods = %s", str_ptr);

		if (!progress_time.Add_Ranges (str_ptr)) {
			Error ("Progression Time Periods %s", str_ptr);
		}
		period_flag = true;
	} else {
		progress_time.Add_Ranges ("0:00..24:00");
	}

	//---- progression speed ----

	str_ptr = Get_Control_String (PROGRESSION_PERIOD_SPEED);

	if (str_ptr == NULL) {
		progress_speed.Add (0);
	} else {
		Print (2, "Progression Period Speed =");

		do {
			str_ptr = Get_Double (str_ptr, &progression_speed);

			if (progression_speed < 0.0 || progression_speed > 30.0) {
				Error ("Progression Speed %.1lf is Out of Range (0..30)", progression_speed);
			}
			lvalue = (int) (progression_speed * 100.0 + 0.5);
			speed_flag = (lvalue != 0);

			if (speed_flag) {
				Print (0, " %.1lf ", (lvalue / 100.0));
			} else {
				Print (0, " N/A ");
			}
			if (!progress_speed.Add (lvalue)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "meters per second");
	}

	//---- optimization method ----

	str_ptr = Get_Control_String (OPTIMIZATION_METHOD);

	if (str_ptr != NULL) {
		Print (2, "Optimization Method = %s", str_ptr);

		if (!str_cmp (str_ptr, "PERCENT_THRU")) {
			method = PERCENT_THRU;
		} else if (!str_cmp (str_ptr, "NETWORK_SPEED")) {
			method = MAX_SPEED;
		} else if (!str_cmp (str_ptr, "VEHICLE_HOURS")) {
			method = VEH_HOURS;
		} else {
			Error ("Unrecognized Optimization Method = %s", str_ptr);
		}
	} else {
		Print (2, "Optimization Method = PERCENT_THRU");
	}

	//---- group period weight file ----

	str_ptr = Get_Control_String (GROUP_PERIOD_WEIGHT_FILE);

	if (str_ptr != NULL) {
		Print (1);
		weight_file.File_Type ("Group Period Weight File");

		if (!weight_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening Period Weights");
		}
		weight_flag = true;
	}

	//---- keep link group order ----

	str_ptr = Get_Control_String (KEEP_LINK_GROUP_ORDER);

	if (str_ptr != NULL) {
		if (!equiv_flag) {
			Error ("A Link Equivalance File is required to Keep Link Group Order");
		}
		Print (2, "Keep Link Group Order = %s", str_ptr);

		order_flag = Get_Control_Flag (KEEP_LINK_GROUP_ORDER);
	}

	//---- get the arcview progression file ----

	str_ptr = Get_Control_String (ARCVIEW_PROGRESSION_FILE);

	if (str_ptr != NULL) {
		arcview_flag = true;

		projection.Read_Control ();

		Print (1);
		arcview_file.File_Type ("New ArcView Progression File");
		arcview_file.File_Access (CREATE);
		arcview_file.Shape_Type (VECTOR);

		arcview_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		arcview_file.Add_Field ("PERIOD", INTEGER, 2);
		period_fld = arcview_file.Num_Fields ();

		arcview_file.Add_Field ("ORDER", INTEGER, 5);
		order_fld = arcview_file.Num_Fields ();

		arcview_file.Add_Field ("GROUP", INTEGER, 5);
		group_fld = arcview_file.Num_Fields ();

		arcview_file.Add_Field ("PERCENT", DOUBLE, 6, 1);
		percent_fld =arcview_file.Num_Fields ();

		arcview_file.Add_Field ("TTIME", INTEGER, 6);
		time_fld = arcview_file.Num_Fields ();

		arcview_file.Add_Field ("LENGTH", INTEGER, 6);
		length_fld = arcview_file.Num_Fields ();

		arcview_file.Add_Field ("SPEED", DOUBLE, 6, 1);
		speed_fld = arcview_file.Num_Fields ();

		arcview_file.Add_Field ("VHT", INTEGER, 10);
		vht_fld = arcview_file.Num_Fields ();

		if (!arcview_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening ArcView Progression File", arcview_file.Shape_Filename ());
		}
		arcview_file.Write_Header ();

		//---- link direction offset ----

		str_ptr = Get_Control_String (LINK_DIRECTION_OFFSET);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &link_offset);

			if (link_offset < 0.0 || link_offset > 15.0) {
				Error ("Link Direction Offset %.2lf is Out of Range (0..15)", link_offset);
			}
		}
		Print (1, "Link Direction Offset = %.2lf meters", link_offset);
	}

	//---- read report types ----

	List_Reports ();

	//---- process support data ----

	if (equiv_flag) {
		if (!link_equiv.Read (Report_Flag (LINK_EQUIV))) {
			Error ("Reading Link Equivalence File");
		}
	}
	return;

mem_error:
	Error ("Insufficient Memory for Progression Data");
} 
