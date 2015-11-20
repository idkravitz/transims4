//*********************************************************
//	PopSyn.hpp - Population Synthesizer
//*********************************************************

#ifndef POPSYN_HPP
#define POPSYN_HPP

#include "Demand_Service.hpp"
#include "Problem_Service.hpp"
#include "Problem_File.hpp"
#include "Household_File.hpp"
#include "Population_File.hpp"
#include "Vehicle_File.hpp"
#include "Db_Header.hpp"
#include "Db_Array.hpp"
#include "Random.hpp"

#include "Data/Location_List.hpp"
#include "Data/Zone_Location.hpp"
#include "Data/Household_Model.hpp"
#include "Data/Veh_Distribution.hpp"
#include "Data/PUMA_Index.hpp"
#include "Data/Stage2_IPF_Data.hpp"
#include "Data/IPF_Data.hpp"

//---------------------------------------------------------
//	PopSyn - execution class definition
//---------------------------------------------------------

class PopSyn : public Demand_Service, public Problem_Service
{
public:

	PopSyn (void);
	virtual ~PopSyn (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

	virtual bool Location_Processing (Db_File *fh);
	virtual bool Access_Processing (Db_File *fh);
	virtual bool Household_Processing (Db_File *fh);
	virtual bool Population_Processing (Db_File *fh);
	virtual bool Vehicle_Processing (Db_File *fh);

protected:

	static char * PUMS_HOUSEHOLD_FILE;
	static char * PUMS_HOUSEHOLD_FORMAT;
	static char * PUMS_POPULATION_FILE;
	static char * PUMS_POPULATION_FORMAT;
	static char * ZONE_DATA_FILE;
	static char * ZONE_DATA_FORMAT;
	static char * NEAREST_ZONE_LOCATION;
	static char * VEHICLE_TYPE_DISTRIBUTION;
	static char * NEW_PROBLEM_FILE;
	static char * NEW_PROBLEM_FORMAT;
	static char * RANDOM_NUMBER_SEED;
	static char * MAXIMUM_IPF_ITERATIONS;
	static char * MAXIMUM_IPF_DIFFERENCE;
	static char * STATE_PUMA_LIST;
	static char * PUMS_WEIGHT_FIELD;
	static char * PUMS_VEHICLE_FIELD;
	static char * PUMS_AGE_FIELD;
	static char * ZONE_DATA_ID_FIELD;
	static char * LOCATION_ZONE_FIELD;
	static char * PUMS_ATTRIBUTE_FIELD_x;
	static char * PUMS_ATTRIBUTE_BREAKS_x;
	static char * ZONE_FIELD_GROUP_x;
	static char * ZONE_TOTAL_FIELD_x;
	static char * LOCATION_WEIGHT_FIELD_x;
	static char * STARTING_HOUSEHOLD_ID;
	static char * STARTING_VEHICLE_ID;
	static char * OUTPUT_HOUSEHOLD_FIELDS;
	static char * OUTPUT_POPULATION_FIELDS;
	static char * LOCATION_WARNING_FLAG;

	virtual void Program_Control (void);

private:
	enum PopSyn_Reports { CROSS_CLASS = 1, PUMS_HOUSEHOLD, PUMS_POPULATION, 
		SYN_HOUSEHOLD, SYN_POPULATION };

	int household, population, vehicle, old_hh, old_pop, old_veh, max_hh, max_veh;

	int weight_field, zone_field, vehicle_field, age_field;
	int hh_state_field, hh_puma_field, hh_hhold_field, pop_hhold_field, pop_person_field;
	int zone_id_field, zone_state_field, zone_puma_field, hh_fields, pop_fields;
	double *hh_total, *hh_minimum, *hh_maximum;
	double *pop_total, *pop_minimum, *pop_maximum;

	bool veh_type_flag, puma_report_flag, zone_loc_flag, hh_sum_flag, pop_sum_flag, warning_flag;

	Household_File *new_hh_file;
	Population_File *new_pop_file;
	Vehicle_File *new_veh_file;

	Db_Header pums_hh_file;
	Db_Header pums_pop_file;
	Db_Header zone_file;
	Db_File veh_type_file, zone_loc_file;

	Db_Data_Array household_db;
	Db_Data_Array population_db;
	
	Problem_File problem_file;
	bool problem_flag;

	Zone_Location_Array zone_location;
	Location_List_Array location_list;

	PUMA_Index_Array puma_index;
	Household_Model_Array household_model;
	Veh_Distribution_Array veh_distribution;

	IPF_Data          ipf_data;
	Stage2_IPF_Data   stage2_data;

	Random random;
	Random random_type;

	//---- methods ----

	void Read_Zone_Locations (void);
	void Vehicle_Types (void);
	void Location_Weights (void);
	void PUMS_Households (void);
	void PUMS_Population (void);
	void Read_Zones (void);
	void IPF_Setup (Household_Model *model_ptr);
	void IPF_Process (Household_Model *model_ptr);
	void Write_Households (Household_Model *model_ptr);

	void Cross_Class_Header (void);

	void PUMS_HH_Summary (void);
	void PUMS_HH_Header (void);

	void PUMS_Pop_Summary (void);
	void PUMS_Pop_Header (void);

	void Household_Report (int num_hh);
	void Household_Header (void);

	void Population_Report (int num_pop);
	void Population_Header (void);
};
#endif
