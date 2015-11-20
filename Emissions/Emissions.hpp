//*********************************************************
//	Emissions.hpp - Summarize Microsimulator Emissions
//*********************************************************

#ifndef EMISSIONS_HPP
#define EMISSIONS_HPP

#include "Demand_Service.hpp"
#include "Integer_Array.hpp"
#include "Index_Array.hpp"
#include "Db_Header.hpp"
#include "Link_Equiv.hpp"
#include "Zone_Equiv.hpp"
#include "Speed_Bin_Data.hpp"
#include "Traveler_File.hpp"
#include "Traveler_Data.hpp"
#include "Time_Range.hpp"
#include "Data_Range.hpp"
#include "Arcview_File.hpp"
#include "Smooth_Data.hpp"
#include "Random.hpp"

#include "Data/Data_Group.hpp"
#include "Data/Rate_Group.hpp"
#include "Data/Category_Data.hpp"
#include "Data/Summary_Data.hpp"
#include "Data/Type_Map.hpp"
#include "Data/Emission_Rate.hpp"
#include "Data/Bin_Sum_Data.hpp"
#include "Data/Vol_Spd_Data.hpp"

//---------------------------------------------------------
//	Emissions - execution class definition
//---------------------------------------------------------

class Emissions : public Demand_Service
{
public:

	Emissions (void);
	virtual ~Emissions (void);

	virtual void Execute (void);

protected:

	static char * SPEED_BIN_FILE;
	static char * SPEED_BIN_FORMAT;	
	static char * SPEED_BIN_FACTOR;
	static char * SPEED_BIN_FILE_x;
	static char * SPEED_BIN_FORMAT_x;
	static char * SPEED_BIN_FACTOR_x;

	static char * TRAVELER_FILE;
	static char * TRAVELER_FORMAT;

	static char * VOLUME_SPEED_FILE;
	static char * VOLUME_SPEED_FORMAT;
	static char * VOLUME_SPEED_FACTOR;
	static char * VOLSPD_LENGTH_FIELD;
	static char * VOLSPD_LENGTH_UNITS;
	static char * VOLSPD_FACILITY_FIELD;
	static char * VOLSPD_RAMP_FACILITY;
	static char * VOLSPD_AREATYPE_FIELD;
	static char * VOLSPD_PERIOD_FIELD;
	static char * VOLSPD_PERIOD_UNITS;
	static char * VOLSPD_SPEED_FIELD;
	static char * VOLSPD_SPEED_UNITS;
	static char * VOLSPD_VOLUME_FIELD_x;
	static char * USE_SIMPLE_INTERPOLATION;

	static char * EMISSION_RATE_FILE;
	static char * EMISSION_RATE_FORMAT;
	static char * RATE_YEAR_FIELD;
	static char * RATE_MONTH_FIELD;
	static char * RATE_PERIOD_FIELD;
	static char * RATE_REGION_FIELD;
	static char * RATE_AREA_TYPE_FIELD;
	static char * RATE_FACILITY_FIELD;
	static char * RATE_ROAD_TYPE_FIELD;
	static char * RATE_VEHICLE_FIELD;
	static char * RATE_POLLUTANT_FIELD;
	static char * RATE_SPEED_BIN_FIELD;
	static char * EMISSION_RATE_FIELD;
	static char * EMISSION_RATE_UNITS;
	static char * EMISSION_RATE_POLLUTANT;

	static char * EMISSION_RATE_FILE_x;
	static char * EMISSION_RATE_FORMAT_x;
	static char * RATE_YEAR_FIELD_x;
	static char * RATE_MONTH_FIELD_x;
	static char * RATE_PERIOD_FIELD_x;
	static char * RATE_REGION_FIELD_x;
	static char * RATE_AREA_TYPE_FIELD_x;
	static char * RATE_FACILITY_FIELD_x;
	static char * RATE_ROAD_TYPE_FIELD_x;
	static char * RATE_VEHICLE_FIELD_x;
	static char * RATE_POLLUTANT_FIELD_x;
	static char * RATE_SPEED_BIN_FIELD_x;
	static char * EMISSION_RATE_FIELD_x;
	static char * EMISSION_RATE_UNITS_x;
	static char * EMISSION_RATE_POLLUTANT_x;

	static char * YEAR_NUMBER_x;
	static char * MONTH_NUMBER_x;
	static char * PERIOD_NUMBER_x;
	static char * REGION_NUMBER_x;
	static char * AREA_TYPE_NUMBER_x;
	static char * FACILITY_NUMBER_x;
	static char * ROAD_TYPE_NUMBER_x;
	static char * VEHICLE_NUMBER_x;
	static char * SPEED_BIN_NUMBER_x;
	static char * POLLUTANT_NUMBER_x;
	static char * POLLUTANT_UNITS_x;
	static char * HPMS_NUMBER_x;

	static char * SUMMARY_YEARS;
	static char * SUMMARY_MONTHS;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * WEEKEND_TRAVEL_FACTOR;

	static char * SELECT_ZONE_RANGE;
	static char * SELECT_SUBAREA_POLYGON;

	static char * LINK_EQUIVALENCE_FILE;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * REGION_EQUIVALENCE_FILE;

	static char * AREA_TYPE_LABELS;
	static char * FACILITY_TYPE_LABELS;
	static char * VEHICLE_TYPE_LABELS;
	static char * ROAD_TYPE_LABELS;

	static char * AREA_TYPE_MAP;
	static char * FACILITY_TYPE_MAP;
	static char * VEHICLE_TYPE_MAP;
	static char * ROAD_TYPE_MAP;
	static char * HPMS_TYPE_MAP;

	static char * NEW_EMISSION_RATE_FILE;
	static char * NEW_EMISSION_RATE_FORMAT;
	static char * NEW_EMISSION_SUMMARY_FILE;
	static char * NEW_EMISSION_SUMMARY_FORMAT;

	static char * NEW_SPEED_SUMMARY_FILE;
	static char * NEW_SPEED_SUMMARY_FORMAT;
	static char * NEW_MOVES_HPMS_VMT_FILE;
	static char * NEW_MOVES_HPMS_VMT_FORMAT;
	static char * NEW_MOVES_RAMP_FRACTION_FILE;
	static char * NEW_MOVES_RAMP_FRACTION_FORMAT;
	static char * NEW_MOVES_SPEED_HOUR_FILE;
	static char * NEW_MOVES_SPEED_HOUR_FORMAT;
	static char * NEW_MOVES_VMT_HOUR_FILE;
	static char * NEW_MOVES_VMT_HOUR_FORMAT;
	static char * NEW_MOVES_VMT_ROAD_FILE;
	static char * NEW_MOVES_VMT_ROAD_FORMAT;

	static char * NEW_MOVES_LINK_SUMMARY_FILE;
	static char * NEW_MOVES_LINK_SUMMARY_FORMAT;
	static char * NEW_MOVES_LINK_VEHICLE_FILE;
	static char * NEW_MOVES_LINK_VEHICLE_FORMAT;
	static char * NEW_MOVES_LINK_SPEED_FILE;
	static char * NEW_MOVES_LINK_SPEED_FORMAT;
	static char * USE_AVERAGE_SEGMENT_SPEEDS;
	static char * NEW_MOVES_LINK_OPERATIONS_FILE;
	static char * NEW_MOVES_LINK_OPERATIONS_FORMAT;

	static char * SMOOTH_GROUP_SIZE;
	static char * SMOOTH_TO_MOVES_SPEED_BINS;
	static char * SMOOTH_SPEED_INCREMENT;
	static char * PERCENT_MOVED_FORWARD;
	static char * PERCENT_MOVED_BACKWARD;
	static char * NUMBER_OF_ITERATIONS;

	static char * NEW_SMOOTH_SAMPLE_FILE;
	static char * NEW_SMOOTH_SAMPLE_FORMAT;
	static char * PERCENT_SAMPLE_SIZE;
	static char * RANDOM_NUMBER_SEED;

	static char * NEW_TRAVELER_FILE;
	static char * NEW_TRAVELER_FORMAT;

	static char * REPORT_TRAVEL_UNITS;
	static char * REPORT_EMISSIONS_UNITS;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:

	enum Emissions_Reports { AT_REPORT = 1, FT_REPORT, VT_REPORT, RT_REPORT, AT_FT_REPORT,
		AT_VT_REPORT, FT_VT_REPORT, RT_VT_REPORT, SUMMARY_REPORT, LINK_EQUIV, ZONE_EQUIV, REGION_EQUIV };

	enum Category_Codes { YEAR = 1, MONTH, PERIOD, REGION, AREATYPE, FACILITY, ROADTYPE, VEHTYPE, POLLUTANT, SPEED_BIN, HPMS };

	enum Report_Types { NO_TYPE, AREA_TYPE, FACILITY_TYPE, ROAD_TYPE, VEH_TYPE };

	Db_Header vol_spd_file, ramp_file, hpms_file, sample_file;
	Db_Header link_sum_file, link_veh_file, link_spd_file, link_ops_file;
	Db_Header rate_file, output_file, new_rate_file, bin_sum_file, spd_file, vmt_file, road_file;
	Db_File veh_map_file, fac_map_file, at_map_file, road_map_file, hpms_map_file;
	Arcview_File select_subarea;

	int num_pollutants, box_length, cell_size, num_bins, max_bin, num_sample;
	bool zone_flag, location_flag, link_flag, link_eq_flag, output_flag, new_rate_flag, vol_spd_flag;
	bool link_sum_flag, link_veh_flag, link_spd_flag, link_ops_flag, interpolate_flag;
	bool veh_map_flag, fac_map_flag, at_map_flag, road_map_flag, bin_sum_flag, rate_flag;
	bool year_flag, month_flag, at_equiv_flag, reg_equiv_flag, select_flag, subarea_flag;
	bool spd_flag, vmt_flag, road_flag, moves_bin_flag, smooth_flag, scan_flag;
	bool ramp_flag, hpms_flag, input_flag, sample_flag, speed_flag, traveler_flag, new_traveler_flag;

	double spd_bin [200], vmt_bin [200], vht_bin [200], ramp_vmt [20], hpms_vmt [100];

	double sample_size;
	Random random;

	int length_field, facility_field, areatype_field, period_field, speed_field, *volume_field, num_vol_fld;
	int length_units, period_units, speed_units, ramp_facility, travel_units, emission_units;
	
	int naverage, niter;
	double forward, backward, bin_size, weekend_factor, volume_factor;
	Smooth_Data smooth_data;

	Data_Range select_zone;
	Zone_Equiv zone_equiv, region_equiv;
	Link_Equiv link_equiv;
	Integer_Sort link_list;

	Traveler_File traveler_file, new_traveler_file;
	Traveler_Array traveler_data;

	Integer_Sort years, months;
	Summary_Array summary_data;
	Bin_Sum_Array bin_sum_data;
	Vol_Spd_Array vol_spd_data;

	Time_Range time_periods;
	Speed_Bin_Array box_location, speed_bin_data;

	Data_Group_Array data_group;
	Rate_Group_Array rate_group;
	Category_Array category_data;
	Category_Data *bin_ptr;

	Emission_Rate_Array emission_rate;

	int facility_map [EXTERNAL+1];
	Type_Map_Array vehicle_map;
	Type_Map_Array area_type_map;
	Type_Map_Array road_type_map;
	Type_Map_Array hpms_type_map;

	//---- facility type and area type labels ----

	typedef struct {
		int  type;
		char *label;
	} Label_Type;

	Index_Array area_type;
	Index_Array facility_type;
	Index_Array vehicle_type;
	Index_Array road_type;

	Integer_List pollutant_units;

	void Read_Area_Type_Map (void);
	void Read_Facility_Map (void);
	void Read_Vehicle_Map (void);
	void Read_Road_Map (void);
	void Read_HPMS_Map (void);

	void Set_Link_List (void);
	void Set_Location (void);
	void Set_Link_Types (void);
	void Scan_Rates (void);
	void Read_Rates (void);
	void Write_Rates (void);
	void Write_Speeds (void);
	void Write_HPMS_VMT (void);
	void Write_Ramp_Fraction (void);
	void Write_Speed_Hour (void);
	void Write_VMT_Hour (void);
	void Write_VMT_Road (void);
	void Read_Speed_Bin (void);
	void Read_Volume_Speed (void);
	void Read_Traveler (void);
	void Traveler_Emissions (void);
	void Output_Emissions (void);

	void Write_Link_Sum (void);
	void Write_Link_Veh (void);
	void Write_Link_Spd (void);
	void Write_Link_Ops (void);

	int  Speed_Bin_Distribution (int group = 1);

    int  Closest_Zone (int x, int y);
	void Calculate_Emissions (int type, int vehicle, double vmt, double vht, double speed);

	void Type_Reports (Emissions_Reports report, Report_Types type1, Report_Types type2 = NO_TYPE);
	void Type_Headers (char *label);

	void Summary_Report (void);
	void Summary_Header (void);
};
#endif
