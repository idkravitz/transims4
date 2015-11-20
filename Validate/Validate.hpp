//*********************************************************
//	Validate.hpp - Network Validation Utility
//*********************************************************

#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include "Demand_Service.hpp"
#include "Index_Array.hpp"
#include "Db_Header.hpp"
#include "Link_Equiv.hpp"
#include "Zone_Equiv.hpp"
#include "Line_Equiv.hpp"
#include "Stop_Equiv.hpp"
#include "Time_Range.hpp"
#include "Link_Data_File.hpp"
#include "Movement_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Validate - execution class definition
//---------------------------------------------------------

class Validate : public Demand_Service
{
public:
	Validate (void);
	virtual ~Validate (void);

	virtual void Execute (void);

protected:

	static char * INPUT_VOLUME_FILE;
	static char * TRAFFIC_COUNT_FILE;
	static char * TURN_VOLUME_FILE;
	static char * TURN_COUNT_FILE;
	static char * LINE_GROUP_COUNT_FILE;
	static char * STOP_GROUP_COUNT_FILE;
	static char * OUTPUT_VOLUME_FILE;
	static char * SUMMARY_TIME_PERIODS;
	static char * SUMMARY_TIME_INCREMENT;
	static char * ANALYSIS_METHOD;
	static char * ADJUSTMENT_FACTOR;
	static char * LINK_EQUIVALENCE_FILE;
	static char * ZONE_EQUIVALENCE_FILE;
	static char * STOP_EQUIVALENCE_FILE;
	static char * LINE_EQUIVALENCE_FILE;
	static char * FACILITY_TYPE_LABELS;
	static char * AREA_TYPE_LABELS;
	static char * SELECT_FACILITY_TYPES;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Validate_Reports {VOLUME_LEVEL = 1, FACILITY_TYPE, AREA_TYPE, ZONE_GROUP, 
		LINK_GROUP, GROUP_DETAILS, TURN_MOVEMENT, TURN_LOS, ZONE_EQUIV, LINK_EQUIV,
		LINE_GROUP, STOP_GROUP, BOARD_GROUP, ALIGHT_GROUP, LINE_EQUIV, STOP_EQUIV};

	bool delay_flag, link_flag, turn_flag, output_flag, zone_flag, method, line_flag, stop_flag;
	bool select_flag, type_flag [EXTERNAL+1];
	double factor, hours;
	char **street_name;

	Db_Header zone_file, line_count_file, stop_count_file;
	Link_Data_File count_file, volume_file, output_file;
	Movement_File turn_count, turn_volume;

	Zone_Equiv zone_equiv;
	Link_Equiv link_equiv;
	Line_Equiv line_equiv;
	Stop_Equiv stop_equiv;
	Time_Range time_periods;

	static int volume_level [];
	char *group_label;

	//---- facility type and area type labels ----

	typedef struct {
		int  type;
		char *label;
	} Label_Type;

	Index_Array facility_type;
	Index_Array area_type;

	//---- evaluation statistics ----

	typedef struct {
		double per_diff;
		double avg_error;
		double per_error;
		double std_dev;
		double r_sq;
		double rmse;
		//double correl;
		double avg_vc;
	} Statistics; 

	typedef struct {
		int    index;
		int    number;
		double count;
		double volume;
		double count_sq;
		double volume_sq;
		double count_volume;
		double error;
		double error_sq;
		double capacity;
		double max_vc;
	} Group_Data; 

	Group_Data total;
	Index_Array group_data;
	Integer_List node_zone;

	//---- methods ----

	virtual bool Link_Processing (Db_File *file);

	void Read_Volume (Link_Data_File *file, bool type);
	void Read_Turn (Movement_File *file, bool type);

	void Sum_Volume (void);
	void Sum_Turn (void);

	void Write_Volume (void);
    int  Closest_Zone (int x, int y);

	void Volume_Stats (void);
	void Volume_Header (void);

	void Facility_Stats (void);
	void Facility_Header (void);

	void Area_Stats (void);
	void Area_Header (void);

	void Zone_Group_Stats (void);
	void Zone_Group_Header (void);

	void Link_Group_Stats (void);
	void Link_Group_Header (void);

	void Link_Group_Details (void);
	void Group_Detail_Header (void);

	void Turn_Stats (void);
	void Turn_Header (void);

	void Turn_LOS (void);
	void Turn_LOS_Header (void);

	void Calc_Stats (Group_Data *data, Statistics *stats);
	void Sum_Group (int group, Link_Data *link, int dir);

	void Line_Group_Stats (void);
	void Line_Group_Header (void);

	void Stop_Group_Stats (int type);
	void Stop_Group_Header (int type);

};

#endif
