//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ActBalance.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ActBalance::Program_Control (void)
{
	int i, num, num_group;
	char *str_ptr, *format_ptr;
	
	Factor_Group *group_ptr;
	Range_Data *range_ptr;

	//---- open network and demand files ----

	Demand_Service::Program_Control ();
	
	Demand_File_False (ACTIVITY);

	//---- open the balancing target file ----

	str_ptr = Get_Control_String (BALANCING_TARGET_FILE);

	if (str_ptr != NULL) {
		target_file.File_Type ("Balancing Target File");
		Print (1);

		format_ptr = Get_Control_String (BALANCING_TARGET_FORMAT);

		if (format_ptr != NULL) {
			target_file.Dbase_Format (format_ptr);
		}
		target_file.Open (Project_Filename (str_ptr));

		if (target_file.Field_Number ("ZONE") == 0) {
			Error ("Balancing Target File ZONE field Not Found");
		}
	}

	//---- create the balancing factor file ----

	str_ptr = Get_Control_String (NEW_BALANCING_FACTOR_FILE);

	if (str_ptr != NULL) {
		factor_file.File_Type ("New Balancing Factor File");
		Print (1);

		format_ptr = Get_Control_String (NEW_BALANCING_FACTOR_FORMAT);

		if (format_ptr != NULL) {
			factor_file.Dbase_Format (format_ptr);
		}
		factor_file.Create (Project_Filename (str_ptr));
			
		if (!factor_file.Add_Field ("ZONE", INTEGER, 5)) {
			Error ("Balancing Factor File ZONE Field Error");
		}
	}

	//---- get the number of groups ----

	num_group = Highest_Control_Group (ACTIVITY_PURPOSE_RANGE_x, 0);

	if (num_group == 0) {
		Error ("No Activity Purpose Groups were Found");
	}

	//---- allocate purpose space ----

	if (!factor_group.Max_Records (num_group)) {
		Error ("Insufficient Memory for %d Activity Purpose Groups", num_group);
	}
	max_purpose = 0;

	//---- process each file ----

	for (i=1; i <= num_group; i++) {
		
		str_ptr = Get_Control_String (ACTIVITY_PURPOSE_RANGE_x, i);

		if (str_ptr == NULL) continue;

		if (!Break_Check (3)) {
			Print (1);
		}
		group_ptr = (Factor_Group *) factor_group.New_Record (true);

		group_ptr->group = i;

		//---- purpose range ----

		Print (1, "Activity Purpose Range #%d = %s", i, str_ptr);

		group_ptr->purpose = new Data_Range ();
		if (group_ptr->purpose == NULL) {
			Error ("Creating Data Range");
		}
		if (!group_ptr->purpose->Add_Ranges (str_ptr)) {
			Error ("Activity Purpose Range %s is Illogical", str_ptr);
		}
		for (range_ptr = group_ptr->purpose->First (); range_ptr; range_ptr = group_ptr->purpose->Next ()) {
			if (range_ptr->High () > max_purpose) max_purpose = range_ptr->High ();
		}
		if (max_purpose >= MAX_PURPOSE) {
			Error ("Too Many Purpose Codes (%d > %d)", max_purpose, MAX_PURPOSE-1);
		}

		//---- target field ----

		str_ptr = Get_Control_String (BALANCING_TARGET_FIELD_x, i);

		if (str_ptr == NULL) goto control_error;

		group_ptr->target_field = target_file.Field_Number (str_ptr);

		if (group_ptr->target_field == 0) {
			Error ("Balancing Target Field %s was Not Found", str_ptr);
		}
		Print (1, "Balancing Target Field #%d = %s, Number = %d", i, str_ptr, group_ptr->target_field);

		//---- factor field ----

		str_ptr = Get_Control_String (BALANCING_FACTOR_FIELD_x, i);

		if (str_ptr == NULL) goto control_error;

		if (!factor_file.Add_Field (str_ptr, DOUBLE, 15, 8)) {
			Error ("Balancing Factor File %s Field Error", str_ptr);
		}
		num = factor_file.Num_Fields ();

		group_ptr->factor_field = factor_file.Field_Number (str_ptr);

		if (group_ptr->factor_field != num) {
			Error ("Balancing Factor %s Field Error", str_ptr);
		}
		Print (1, "Balancing Factor Field #%d = %s, Number = %d", i, str_ptr, group_ptr->factor_field);

		if (!factor_group.Add ()) {
			Error ("Adding Factor Group");
		}
	}
	if (max_purpose == 0) {
		Error ("No Purpose Values were Defined");
	}
	factor_file.Write_Header ();

	//---- open the zone equivalence ----

	if (Report_Flag (DISTRICT_SUM)) {
		str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (str_ptr == NULL) goto control_error;

		Print (1);
		zone_equiv.Open (Project_Filename (str_ptr));
	}

	//---- read report types ----

	List_Reports ();

	if (Report_Flag (DISTRICT_SUM)) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
