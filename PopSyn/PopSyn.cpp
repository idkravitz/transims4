//*********************************************************
//	PopSyn.cpp - Population Synthesizer
//*********************************************************

#include "PopSyn.hpp"

char * PopSyn::PUMS_HOUSEHOLD_FILE			= "PUMS_HOUSEHOLD_FILE";
char * PopSyn::PUMS_HOUSEHOLD_FORMAT		= "PUMS_HOUSEHOLD_FORMAT";
char * PopSyn::PUMS_POPULATION_FILE			= "PUMS_POPULATION_FILE";
char * PopSyn::PUMS_POPULATION_FORMAT		= "PUMS_POPULATION_FORMAT";
char * PopSyn::ZONE_DATA_FILE				= "ZONE_DATA_FILE";
char * PopSyn::ZONE_DATA_FORMAT				= "ZONE_DATA_FORMAT";
char * PopSyn::NEAREST_ZONE_LOCATION		= "NEAREST_ZONE_LOCATION";
char * PopSyn::VEHICLE_TYPE_DISTRIBUTION	= "VEHICLE_TYPE_DISTRIBUTION";
char * PopSyn::NEW_PROBLEM_FILE				= "NEW_PROBLEM_FILE";
char * PopSyn::NEW_PROBLEM_FORMAT			= "NEW_PROBLEM_FORMAT";
char * PopSyn::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * PopSyn::MAXIMUM_IPF_ITERATIONS		= "MAXIMUM_IPF_ITERATIONS";
char * PopSyn::MAXIMUM_IPF_DIFFERENCE		= "MAXIMUM_IPF_DIFFERENCE";
char * PopSyn::STATE_PUMA_LIST				= "STATE_PUMA_LIST";
char * PopSyn::PUMS_WEIGHT_FIELD			= "PUMS_WEIGHT_FIELD";
char * PopSyn::PUMS_VEHICLE_FIELD			= "PUMS_VEHICLE_FIELD";
char * PopSyn::PUMS_AGE_FIELD				= "PUMS_AGE_FIELD";
char * PopSyn::ZONE_DATA_ID_FIELD			= "ZONE_DATA_ID_FIELD";
char * PopSyn::LOCATION_ZONE_FIELD			= "LOCATION_ZONE_FIELD";
char * PopSyn::PUMS_ATTRIBUTE_FIELD_x		= "PUMS_ATTRIBUTE_FIELD_*";
char * PopSyn::PUMS_ATTRIBUTE_BREAKS_x		= "PUMS_ATTRIBUTE_BREAKS_*";
char * PopSyn::ZONE_FIELD_GROUP_x			= "ZONE_FIELD_GROUP_*";
char * PopSyn::ZONE_TOTAL_FIELD_x			= "ZONE_TOTAL_FIELD_*";
char * PopSyn::LOCATION_WEIGHT_FIELD_x		= "LOCATION_WEIGHT_FIELD_*";
char * PopSyn::STARTING_HOUSEHOLD_ID		= "STARTING_HOUSEHOLD_ID";
char * PopSyn::STARTING_VEHICLE_ID			= "STARTING_VEHICLE_ID";
char * PopSyn::OUTPUT_HOUSEHOLD_FIELDS		= "OUTPUT_HOUSEHOLD_FIELDS";
char * PopSyn::OUTPUT_POPULATION_FIELDS		= "OUTPUT_POPULATION_FIELDS";
char * PopSyn::LOCATION_WARNING_FLAG		= "LOCATION_WARNING_FLAG";

//---------------------------------------------------------
//	PopSyn constructor
//---------------------------------------------------------

PopSyn::PopSyn (void) : Demand_Service (), Problem_Service ()
{
	Program ("PopSyn");
	Version ("4.0.7");
	Title ("Population Synthesizer");

	Network_File required_network [] = {
		ACTIVITY_LOCATION, PROCESS_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, END_NETWORK
	};

	Demand_File required_demand [] = {
		NEW_HOUSEHOLD, NEW_POPULATION, NEW_VEHICLE, END_DEMAND
	};

	Demand_File optional_demand [] = {
		HOUSEHOLD, POPULATION, VEHICLE, END_DEMAND
	};

	char *keys [] = {
		PUMS_HOUSEHOLD_FILE,
		PUMS_HOUSEHOLD_FORMAT,
		PUMS_POPULATION_FILE,
		PUMS_POPULATION_FORMAT,
		ZONE_DATA_FILE,
		ZONE_DATA_FORMAT,
		NEAREST_ZONE_LOCATION,
		VEHICLE_TYPE_DISTRIBUTION,
		NEW_PROBLEM_FILE,
		NEW_PROBLEM_FORMAT,
		RANDOM_NUMBER_SEED,
		MAXIMUM_IPF_ITERATIONS,
		MAXIMUM_IPF_DIFFERENCE,
		STATE_PUMA_LIST,
		PUMS_WEIGHT_FIELD,
		PUMS_VEHICLE_FIELD,
		PUMS_AGE_FIELD,
		ZONE_DATA_ID_FIELD,
		LOCATION_ZONE_FIELD,
		PUMS_ATTRIBUTE_FIELD_x,
		PUMS_ATTRIBUTE_BREAKS_x,
		ZONE_FIELD_GROUP_x,
		ZONE_TOTAL_FIELD_x,
		LOCATION_WEIGHT_FIELD_x,
		STARTING_HOUSEHOLD_ID,
		STARTING_VEHICLE_ID,
		OUTPUT_HOUSEHOLD_FIELDS,
		OUTPUT_POPULATION_FIELDS,
		LOCATION_WARNING_FLAG,
		NULL
	};

	char *reports [] = {
		"PUMA_CROSS_CLASSIFICATION",
		"PUMS_HOUSEHOLD_SUMMARY",
		"PUMS_POPULATION_SUMMARY",
		"SYNTHETIC_HOUSEHOLD_SUMMARY",
		"SYNTHETIC_POPULATION_SUMMARY",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	Check_Data (true);
	Renumber (true);

	problem_flag = veh_type_flag = puma_report_flag = zone_loc_flag = false;
	hh_sum_flag = pop_sum_flag = warning_flag = false;

	household = vehicle = 1;
	population = 0;
	old_hh = old_pop = old_veh = max_hh = max_veh = 0;

	zone_field = weight_field = vehicle_field = age_field = 0;
	hh_state_field = hh_puma_field = hh_hhold_field = pop_hhold_field = pop_person_field = 0;
	zone_id_field = zone_state_field = zone_puma_field = 0;

	hh_fields = pop_fields = 0;
	hh_total = hh_minimum = hh_maximum = NULL;
	pop_total = pop_minimum = pop_maximum = NULL;

	//---- temporary fix ----

	location_array = (Location_Array *) &location_list;
}

//---------------------------------------------------------
//	PopSyn destructor
//---------------------------------------------------------

PopSyn::~PopSyn (void)
{
	if (hh_fields > 0) {
		if (hh_total != NULL) {
			delete [] hh_total;
			hh_total = NULL;
		}
		if (hh_minimum != NULL) {
			delete [] hh_minimum;
			hh_minimum = NULL;
		}
		if (hh_maximum != NULL) {
			delete [] hh_maximum;
			hh_maximum = NULL;
		}
	}
	if (pop_fields > 0) {
		if (pop_total != NULL) {
			delete [] pop_total;
			pop_total = NULL;
		}
		if (pop_minimum != NULL) {
			delete [] pop_minimum;
			pop_minimum = NULL;
		}
		if (pop_maximum != NULL) {
			delete [] pop_maximum;
			pop_maximum = NULL;
		}
	}
	household_model.Clear ();
	ipf_data.Clear ();
	stage2_data.Clear ();
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PopSyn *exe = new PopSyn ();

	return (exe->Start_Execution (commands, control));
}

