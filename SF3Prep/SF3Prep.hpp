//*********************************************************
//	SF3Prep.hpp - SF3 data extraction
//*********************************************************

#ifndef SF3PREP_HPP
#define SF3PREP_HPP

#include "Execution_Service.hpp"
#include "Db_Header.hpp"
#include "Data_Array.hpp"
#include "Data_Range.hpp"
#include "Utility.hpp"
#include "Projection_Service.hpp"
#include "Projection.hpp"

class SF3Prep : public Execution_Service, public Db_Code
{
public:
	SF3Prep (void);
	virtual ~SF3Prep (void);

	virtual void Execute (void);

protected:

	static char * SF3_GEOGRAPHY_FILE;
	static char * SF3_GEOGRAPHY_FILE_x;
	static char * SF3_SEGMENT_FILE_x;
	static char * SF3_SUMMARY_LEVEL;
	static char * STATE_COUNTY_LIST;
	static char * ZONE_DATA_FILE;
	static char * ZONE_DATA_FORMAT;
	static char * NEW_ZONE_DATA_FILE;
	static char * NEW_ZONE_DATA_FORMAT;
	static char * ZONE_DATA_FIELD_RANGE_x;
 
	virtual void Program_Control (void);

private:

	bool input_flag, select_flag, type_flag;
	int sum_level, nrec, zone_in, zone_out;

	typedef struct {
		int number;
		Db_File *file;
	} SEGMENT;

	typedef struct {
		int group;
		Data_Array *segment;
	} SF3_GROUP;

	typedef struct {
		char state [4];
		int  county;
	} STATE_COUNTY;

	typedef struct {
		int segment;
		int offset;
		int count;
		int field;
	} FIELD_RANGE;

	typedef struct {
		char table [12];
		int segment;
		int offset;
		int fields;
	} SF3_FIELD;

	Data_Array sf3_group, state_county, field_range;

	static SF3_FIELD sf3_field [];

	Db_Header zone_file, new_zone_file;

	Projection_Service proj_service;	
	Projection projection;

	void Copy_Data (void);
	void Read_SF3 (SF3_GROUP *group);
};
#endif
