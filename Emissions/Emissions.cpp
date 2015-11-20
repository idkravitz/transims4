//*********************************************************
//	Emissions.cpp - Summarize Microsimulator Emissions
//*********************************************************

#include "Emissions.hpp"

char * Emissions::SPEED_BIN_FILE					= "SPEED_BIN_FILE";
char * Emissions::SPEED_BIN_FORMAT					= "SPEED_BIN_FORMAT";
char * Emissions::SPEED_BIN_FACTOR					= "SPEED_BIN_FACTOR";
char * Emissions::SPEED_BIN_FILE_x					= "SPEED_BIN_FILE_*";
char * Emissions::SPEED_BIN_FORMAT_x				= "SPEED_BIN_FORMAT_*";
char * Emissions::SPEED_BIN_FACTOR_x				= "SPEED_BIN_FACTOR_*";

char * Emissions::TRAVELER_FILE						= "TRAVELER_FILE";
char * Emissions::TRAVELER_FORMAT					= "TRAVELER_FORMAT";

char * Emissions::VOLUME_SPEED_FILE					= "VOLUME_SPEED_FILE";
char * Emissions::VOLUME_SPEED_FORMAT				= "VOLUME_SPEED_FORMAT";
char * Emissions::VOLUME_SPEED_FACTOR				= "VOLUME_SPEED_FACTOR";
char * Emissions::VOLSPD_LENGTH_FIELD				= "VOLSPD_LENGTH_FIELD";
char * Emissions::VOLSPD_LENGTH_UNITS				= "VOLSPD_LENGTH_UNITS";
char * Emissions::VOLSPD_FACILITY_FIELD				= "VOLSPD_FACILITY_FIELD";
char * Emissions::VOLSPD_RAMP_FACILITY				= "VOLSPD_RAMP_FACILITY";
char * Emissions::VOLSPD_AREATYPE_FIELD				= "VOLSPD_AREATYPE_FIELD";
char * Emissions::VOLSPD_PERIOD_FIELD				= "VOLSPD_PERIOD_FIELD";
char * Emissions::VOLSPD_PERIOD_UNITS				= "VOLSPD_PERIOD_UNITS";
char * Emissions::VOLSPD_SPEED_FIELD				= "VOLSPD_SPEED_FIELD";
char * Emissions::VOLSPD_SPEED_UNITS				= "VOLSPD_SPEED_UNITS";
char * Emissions::VOLSPD_VOLUME_FIELD_x				= "VOLSPD_VOLUME_FIELD_*";
char * Emissions::USE_SIMPLE_INTERPOLATION			= "USE_SIMPLE_INTERPOLATION";

char * Emissions::EMISSION_RATE_FILE				= "EMISSION_RATE_FILE";
char * Emissions::EMISSION_RATE_FORMAT				= "EMISSION_RATE_FORMAT";
char * Emissions::RATE_YEAR_FIELD					= "RATE_YEAR_FIELD";
char * Emissions::RATE_MONTH_FIELD					= "RATE_MONTH_FIELD";
char * Emissions::RATE_PERIOD_FIELD					= "RATE_PERIOD_FIELD";
char * Emissions::RATE_REGION_FIELD					= "RATE_REGION_FIELD";
char * Emissions::RATE_AREA_TYPE_FIELD				= "RATE_AREA_TYPE_FIELD";
char * Emissions::RATE_FACILITY_FIELD				= "RATE_FACILITY_FIELD";
char * Emissions::RATE_ROAD_TYPE_FIELD				= "RATE_ROAD_TYPE_FIELD";
char * Emissions::RATE_VEHICLE_FIELD				= "RATE_VEHICLE_FIELD";
char * Emissions::RATE_POLLUTANT_FIELD				= "RATE_POLLUTANT_FIELD";
char * Emissions::RATE_SPEED_BIN_FIELD				= "RATE_SPEED_BIN_FIELD";
char * Emissions::EMISSION_RATE_FIELD				= "EMISSION_RATE_FIELD";
char * Emissions::EMISSION_RATE_UNITS				= "EMISSION_RATE_UNITS";
char * Emissions::EMISSION_RATE_POLLUTANT			= "EMISSION_RATE_POLLUTANT";

char * Emissions::EMISSION_RATE_FILE_x				= "EMISSION_RATE_FILE_*";
char * Emissions::EMISSION_RATE_FORMAT_x			= "EMISSION_RATE_FORMAT_*";
char * Emissions::RATE_YEAR_FIELD_x					= "RATE_YEAR_FIELD_*";
char * Emissions::RATE_MONTH_FIELD_x				= "RATE_MONTH_FIELD_*";
char * Emissions::RATE_PERIOD_FIELD_x				= "RATE_PERIOD_FIELD_*";
char * Emissions::RATE_REGION_FIELD_x				= "RATE_REGION_FIELD_*";
char * Emissions::RATE_AREA_TYPE_FIELD_x			= "RATE_AREA_TYPE_FIELD_*";
char * Emissions::RATE_FACILITY_FIELD_x				= "RATE_FACILITY_FIELD_*";
char * Emissions::RATE_ROAD_TYPE_FIELD_x			= "RATE_ROAD_TYPE_FIELD_*";
char * Emissions::RATE_VEHICLE_FIELD_x				= "RATE_VEHICLE_FIELD_*";
char * Emissions::RATE_POLLUTANT_FIELD_x			= "RATE_POLLUTANT_FIELD_*";
char * Emissions::RATE_SPEED_BIN_FIELD_x			= "RATE_SPEED_BIN_FIELD_*";
char * Emissions::EMISSION_RATE_FIELD_x				= "EMISSION_RATE_FIELD_*";
char * Emissions::EMISSION_RATE_UNITS_x				= "EMISSION_RATE_UNITS_*";
char * Emissions::EMISSION_RATE_POLLUTANT_x			= "EMISSION_RATE_POLLUTANT_*";

char * Emissions::YEAR_NUMBER_x						= "YEAR_NUMBER_*";
char * Emissions::MONTH_NUMBER_x					= "MONTH_NUMBER_*";
char * Emissions::PERIOD_NUMBER_x					= "PERIOD_NUMBER_*";
char * Emissions::REGION_NUMBER_x					= "REGION_NUMBER_*";
char * Emissions::AREA_TYPE_NUMBER_x				= "AREA_TYPE_NUMBER_*";
char * Emissions::FACILITY_NUMBER_x					= "FACILITY_NUMBER_*";
char * Emissions::ROAD_TYPE_NUMBER_x				= "ROAD_TYPE_NUMBER_*";
char * Emissions::VEHICLE_NUMBER_x					= "VEHICLE_NUMBER_*";
char * Emissions::SPEED_BIN_NUMBER_x				= "SPEED_BIN_NUMBER_*";
char * Emissions::POLLUTANT_NUMBER_x				= "POLLUTANT_NUMBER_*";
char * Emissions::POLLUTANT_UNITS_x					= "POLLUTANT_UNITS_*";
char * Emissions::HPMS_NUMBER_x						= "HPMS_NUMBER_*";

char * Emissions::SUMMARY_YEARS						= "SUMMARY_YEARS";
char * Emissions::SUMMARY_MONTHS					= "SUMMARY_MONTHS";
char * Emissions::SUMMARY_TIME_PERIODS				= "SUMMARY_TIME_PERIODS";
char * Emissions::SUMMARY_TIME_INCREMENT			= "SUMMARY_TIME_INCREMENT";
char * Emissions::WEEKEND_TRAVEL_FACTOR				= "WEEKEND_TRAVEL_FACTOR";

char * Emissions::SELECT_ZONE_RANGE					= "SELECT_ZONE_RANGE";
char * Emissions::SELECT_SUBAREA_POLYGON			= "SELECT_SUBAREA_POLYGON";

char * Emissions::LINK_EQUIVALENCE_FILE				= "LINK_EQUIVALENCE_FILE";
char * Emissions::ZONE_EQUIVALENCE_FILE				= "ZONE_EQUIVALENCE_FILE";
char * Emissions::REGION_EQUIVALENCE_FILE			= "REGION_EQUIVALENCE_FILE";

char * Emissions::AREA_TYPE_LABELS					= "AREA_TYPE_LABELS";
char * Emissions::FACILITY_TYPE_LABELS				= "FACILITY_TYPE_LABELS";
char * Emissions::VEHICLE_TYPE_LABELS				= "VEHICLE_TYPE_LABELS";
char * Emissions::ROAD_TYPE_LABELS					= "ROAD_TYPE_LABELS";

char * Emissions::AREA_TYPE_MAP						= "AREA_TYPE_MAP";
char * Emissions::FACILITY_TYPE_MAP					= "FACILITY_TYPE_MAP";
char * Emissions::VEHICLE_TYPE_MAP					= "VEHICLE_TYPE_MAP";
char * Emissions::ROAD_TYPE_MAP						= "ROAD_TYPE_MAP";
char * Emissions::HPMS_TYPE_MAP						= "HPMS_TYPE_MAP";

char * Emissions::NEW_EMISSION_RATE_FILE			= "NEW_EMISSION_RATE_FILE";
char * Emissions::NEW_EMISSION_RATE_FORMAT			= "NEW_EMISSION_RATE_FORMAT";
char * Emissions::NEW_EMISSION_SUMMARY_FILE			= "NEW_EMISSION_SUMMARY_FILE";
char * Emissions::NEW_EMISSION_SUMMARY_FORMAT		= "NEW_EMISSION_SUMMARY_FORMAT";
char * Emissions::NEW_SPEED_SUMMARY_FILE			= "NEW_SPEED_SUMMARY_FILE";
char * Emissions::NEW_SPEED_SUMMARY_FORMAT			= "NEW_SPEED_SUMMARY_FORMAT";

char * Emissions::NEW_MOVES_HPMS_VMT_FILE			= "NEW_MOVES_HPMS_VMT_FILE";
char * Emissions::NEW_MOVES_HPMS_VMT_FORMAT			= "NEW_MOVES_HPMS_VMT_FORMAT";
char * Emissions::NEW_MOVES_RAMP_FRACTION_FILE		= "NEW_MOVES_RAMP_FRACTION_FILE";
char * Emissions::NEW_MOVES_RAMP_FRACTION_FORMAT	= "NEW_MOVES_RAMP_FRACTION_FORMAT";
char * Emissions::NEW_MOVES_SPEED_HOUR_FILE			= "NEW_MOVES_SPEED_HOUR_FILE";
char * Emissions::NEW_MOVES_SPEED_HOUR_FORMAT		= "NEW_MOVES_SPEED_HOUR_FORMAT";
char * Emissions::NEW_MOVES_VMT_HOUR_FILE			= "NEW_MOVES_VMT_HOUR_FILE";
char * Emissions::NEW_MOVES_VMT_HOUR_FORMAT			= "NEW_MOVES_VMT_HOUR_FORMAT";
char * Emissions::NEW_MOVES_VMT_ROAD_FILE			= "NEW_MOVES_VMT_ROAD_FILE";
char * Emissions::NEW_MOVES_VMT_ROAD_FORMAT			= "NEW_MOVES_VMT_ROAD_FORMAT";

char * Emissions::NEW_MOVES_LINK_SUMMARY_FILE		= "NEW_MOVES_LINK_SUMMARY_FILE";
char * Emissions::NEW_MOVES_LINK_SUMMARY_FORMAT		= "NEW_MOVES_LINK_SUMMARY_FORMAT";
char * Emissions::NEW_MOVES_LINK_VEHICLE_FILE		= "NEW_MOVES_LINK_VEHICLE_FILE";
char * Emissions::NEW_MOVES_LINK_VEHICLE_FORMAT		= "NEW_MOVES_LINK_VEHICLE_FORMAT";
char * Emissions::NEW_MOVES_LINK_SPEED_FILE			= "NEW_MOVES_LINK_SPEED_FILE";
char * Emissions::NEW_MOVES_LINK_SPEED_FORMAT		= "NEW_MOVES_LINK_SPEED_FORMAT";
char * Emissions::USE_AVERAGE_SEGMENT_SPEEDS		= "USE_AVERAGE_SEGMENT_SPEEDS";
char * Emissions::NEW_MOVES_LINK_OPERATIONS_FILE	= "NEW_MOVES_LINK_OPERATIONS_FILE";
char * Emissions::NEW_MOVES_LINK_OPERATIONS_FORMAT	= "NEW_MOVES_LINK_OPERATIONS_FORMAT";

char * Emissions::SMOOTH_TO_MOVES_SPEED_BINS		= "SMOOTH_TO_MOVES_SPEED_BINS";
char * Emissions::SMOOTH_GROUP_SIZE					= "SMOOTH_GROUP_SIZE";
char * Emissions::SMOOTH_SPEED_INCREMENT			= "SMOOTH_SPEED_INCREMENT";
char * Emissions::PERCENT_MOVED_FORWARD				= "PERCENT_MOVED_FORWARD";
char * Emissions::PERCENT_MOVED_BACKWARD			= "PERCENT_MOVED_BACKWARD";
char * Emissions::NUMBER_OF_ITERATIONS				= "NUMBER_OF_ITERATIONS";

char * Emissions::NEW_SMOOTH_SAMPLE_FILE			= "NEW_SMOOTH_SAMPLE_FILE";
char * Emissions::NEW_SMOOTH_SAMPLE_FORMAT			= "NEW_SMOOTH_SAMPLE_FORMAT";
char * Emissions::PERCENT_SAMPLE_SIZE				= "PERCENT_SAMPLE_SIZE";
char * Emissions::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";

char * Emissions::NEW_TRAVELER_FILE					= "NEW_TRAVELER_FILE";
char * Emissions::NEW_TRAVELER_FORMAT				= "NEW_TRAVELER_FORMAT";

char * Emissions::REPORT_TRAVEL_UNITS				= "REPORT_TRAVEL_UNITS";
char * Emissions::REPORT_EMISSIONS_UNITS			= "REPORT_EMISSIONS_UNITS";

//---------------------------------------------------------
//	Emissions constructor
//---------------------------------------------------------

Emissions::Emissions (void) : Demand_Service ()
{
	Program ("Emissions");
	Version ("4.0.16");
	Title ("Apply Vehicle Emissions Rates");

	Network_File optional_network [] = {
		DIRECTORY, LINK, NODE, ZONE, ACTIVITY_LOCATION, END_NETWORK
	};
	Demand_File optional_demand [] = {
		VEHICLE, END_DEMAND
	};

	char *keys [] = {
		SPEED_BIN_FILE,
		SPEED_BIN_FORMAT,
		SPEED_BIN_FACTOR,
		SPEED_BIN_FILE_x,
		SPEED_BIN_FORMAT_x,
		SPEED_BIN_FACTOR_x,

		TRAVELER_FILE,
		TRAVELER_FORMAT,

		VOLUME_SPEED_FILE,
		VOLUME_SPEED_FORMAT,
		VOLUME_SPEED_FACTOR,
		VOLSPD_LENGTH_FIELD,
		VOLSPD_LENGTH_UNITS,
		VOLSPD_FACILITY_FIELD,
		VOLSPD_RAMP_FACILITY,
		VOLSPD_AREATYPE_FIELD,
		VOLSPD_PERIOD_FIELD,
		VOLSPD_PERIOD_UNITS,
		VOLSPD_SPEED_FIELD,
		VOLSPD_SPEED_UNITS,
		VOLSPD_VOLUME_FIELD_x,
		USE_SIMPLE_INTERPOLATION,

		EMISSION_RATE_FILE,
		EMISSION_RATE_FORMAT,
		RATE_YEAR_FIELD,
		RATE_MONTH_FIELD,
		RATE_PERIOD_FIELD,
		RATE_REGION_FIELD,
		RATE_AREA_TYPE_FIELD,
		RATE_FACILITY_FIELD,
		RATE_ROAD_TYPE_FIELD,
		RATE_VEHICLE_FIELD,
		RATE_POLLUTANT_FIELD,
		RATE_SPEED_BIN_FIELD,
		EMISSION_RATE_FIELD,
		EMISSION_RATE_UNITS,
		EMISSION_RATE_POLLUTANT,

		EMISSION_RATE_FILE_x,
		EMISSION_RATE_FORMAT_x,
		RATE_YEAR_FIELD_x,
		RATE_MONTH_FIELD_x,
		RATE_PERIOD_FIELD_x,
		RATE_REGION_FIELD_x,
		RATE_AREA_TYPE_FIELD_x,
		RATE_FACILITY_FIELD_x,
		RATE_ROAD_TYPE_FIELD_x,
		RATE_VEHICLE_FIELD_x,
		RATE_POLLUTANT_FIELD_x,
		RATE_SPEED_BIN_FIELD_x,
		EMISSION_RATE_FIELD_x,
		EMISSION_RATE_UNITS_x,
		EMISSION_RATE_POLLUTANT_x,

		YEAR_NUMBER_x,
		MONTH_NUMBER_x,
		PERIOD_NUMBER_x,
		REGION_NUMBER_x,
		AREA_TYPE_NUMBER_x,
		FACILITY_NUMBER_x,
		ROAD_TYPE_NUMBER_x,
		VEHICLE_NUMBER_x,
		SPEED_BIN_NUMBER_x,
		POLLUTANT_NUMBER_x,
		POLLUTANT_UNITS_x,
		HPMS_NUMBER_x,

		SUMMARY_YEARS,
		SUMMARY_MONTHS,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		WEEKEND_TRAVEL_FACTOR,

		SELECT_ZONE_RANGE,
		SELECT_SUBAREA_POLYGON,

		LINK_EQUIVALENCE_FILE,
		ZONE_EQUIVALENCE_FILE,
		REGION_EQUIVALENCE_FILE,

		AREA_TYPE_LABELS,
		FACILITY_TYPE_LABELS,
		VEHICLE_TYPE_LABELS,
		ROAD_TYPE_LABELS,

		AREA_TYPE_MAP,
		FACILITY_TYPE_MAP,
		VEHICLE_TYPE_MAP,
		ROAD_TYPE_MAP,
		HPMS_TYPE_MAP,

		NEW_EMISSION_RATE_FILE,
		NEW_EMISSION_RATE_FORMAT,
		NEW_EMISSION_SUMMARY_FILE,
		NEW_EMISSION_SUMMARY_FORMAT,

		NEW_SPEED_SUMMARY_FILE,
		NEW_SPEED_SUMMARY_FORMAT,
		NEW_MOVES_HPMS_VMT_FILE,
		NEW_MOVES_HPMS_VMT_FORMAT,
		NEW_MOVES_RAMP_FRACTION_FILE,
		NEW_MOVES_RAMP_FRACTION_FORMAT,
		NEW_MOVES_SPEED_HOUR_FILE,
		NEW_MOVES_SPEED_HOUR_FORMAT,
		NEW_MOVES_VMT_HOUR_FILE,
		NEW_MOVES_VMT_HOUR_FORMAT,
		NEW_MOVES_VMT_ROAD_FILE,
		NEW_MOVES_VMT_ROAD_FORMAT,

		NEW_MOVES_LINK_SUMMARY_FILE,
		NEW_MOVES_LINK_SUMMARY_FORMAT,
		NEW_MOVES_LINK_VEHICLE_FILE,
		NEW_MOVES_LINK_VEHICLE_FORMAT,
		NEW_MOVES_LINK_SPEED_FILE,
		NEW_MOVES_LINK_SPEED_FORMAT,
		USE_AVERAGE_SEGMENT_SPEEDS,
		NEW_MOVES_LINK_OPERATIONS_FILE,
		NEW_MOVES_LINK_OPERATIONS_FORMAT,

		SMOOTH_TO_MOVES_SPEED_BINS,
		SMOOTH_GROUP_SIZE,
		SMOOTH_SPEED_INCREMENT,
		PERCENT_MOVED_FORWARD,
		PERCENT_MOVED_BACKWARD,
		NUMBER_OF_ITERATIONS,

		NEW_SMOOTH_SAMPLE_FILE,
		NEW_SMOOTH_SAMPLE_FORMAT,
		PERCENT_SAMPLE_SIZE,
		RANDOM_NUMBER_SEED,

		NEW_TRAVELER_FILE,
		NEW_TRAVELER_FORMAT,

		REPORT_TRAVEL_UNITS,
		REPORT_EMISSIONS_UNITS,
		NULL
	};
	char *reports [] = {
		"EMISSIONS_BY_AREA_TYPE",
		"EMISSIONS_BY_FACILITY_TYPE",
		"EMISSIONS_BY_VEHICLE_TYPE",
		"EMISSIONS_BY_ROAD_TYPE",
		"EMISSIONS_BY_AREA_AND_FACILITY",
		"EMISSIONS_BY_AREA_AND_VEHICLE",
		"EMISSIONS_BY_FACILITY_AND_VEHICLE",
		"EMISSIONS_BY_ROAD_AND_VEHICLE",
		"EMISSIONS_SUMMARY_REPORT",
		"PRINT_LINK_EQUIVALENCIES",
		"PRINT_ZONE_EQUIVALENCIES",
		"PRINT_REGION_EQUIVALENCIES",
		NULL
	};

	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	
	Page_Size (120, 47);
	
	num_pollutants = box_length = cell_size = num_bins = max_bin = num_sample = 0;
	zone_flag = location_flag = link_flag = link_eq_flag = output_flag = input_flag = false;
	link_sum_flag = link_veh_flag = link_spd_flag = link_ops_flag = speed_flag = false;
	traveler_flag = new_traveler_flag = interpolate_flag = false;
	veh_map_flag = fac_map_flag = at_map_flag = road_map_flag = sample_flag = false;
	year_flag = month_flag = at_equiv_flag = reg_equiv_flag = new_rate_flag = false;
	select_flag = subarea_flag = rate_flag = vol_spd_flag = ramp_flag = hpms_flag = false;
	spd_flag = vmt_flag = road_flag = moves_bin_flag = smooth_flag = scan_flag = bin_sum_flag = false;

	length_field = facility_field = areatype_field = period_field = speed_field = 0;
	length_units = period_units = speed_units = num_vol_fld = emission_units = 0;
	travel_units = 1;
	volume_field = NULL;

	ramp_facility = 9;
	niter = 3;
	naverage = 3;
	forward = 20.0;
	backward = 20.0;
	bin_size = 1.0;
	weekend_factor = 1.0;
	volume_factor = 1.0;
	sample_size = 0.05;

	memset (facility_map, '\0', sizeof (facility_map));

	area_type.Initialize (sizeof (Label_Type), 10);
	facility_type.Initialize (sizeof (Label_Type), 10);
	vehicle_type.Initialize (sizeof (Label_Type), 10);
	road_type.Initialize (sizeof (Label_Type), 10);
}

//---------------------------------------------------------
//	Emissions destructor
//---------------------------------------------------------

Emissions::~Emissions (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Emissions *exe = new Emissions ();

	return (exe->Start_Execution (commands, control));
}

