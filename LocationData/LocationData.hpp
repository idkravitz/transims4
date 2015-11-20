//*********************************************************
//	LocationData.hpp - Activity Location Data Processing
//*********************************************************

#ifndef LOCATIONDATA_HPP
#define LOCATIONDATA_HPP

#include "Network_Service.hpp"
#include "Location_File.hpp"
#include "User_Program.hpp"
#include "Time_Range.hpp"
#include "Db_Array.hpp"
#include "Db_Header.hpp"
#include "Subzone_File.hpp"
#include "Subzone_Data.hpp"
#include "Arcview_File.hpp"
#include "Data_Range.hpp"
#include "Projection_Service.hpp"
#include "Integer_Array.hpp"

//---------------------------------------------------------
//	LocationData - execution class definition
//---------------------------------------------------------

class LocationData : public Network_Service
{
public:

	LocationData (void);
	virtual ~LocationData (void);

	virtual void Execute (void);

	virtual bool Zone_Processing (Db_File *fh);

protected:

	static char * COPY_EXISTING_FIELDS;
	static char * NEW_WALK_ACCESS_FIELD;
	static char * MAX_WALK_DISTANCE;
	static char * WALK_ACCESS_TIME_RANGE;
	static char * TIME_OF_DAY_FORMAT;
	
	static char * NEW_USE_FLAG_FIELD;
	static char * LINK_USE_FLAG_TYPES;
	
	static char * NEW_USE_FLAG_FIELD_x;
	static char * LINK_USE_FLAG_TYPES_x;

	static char * SUBZONE_ZONE_FACTOR_FILE;
	static char * NEW_SUBZONE_FIELD;
	static char * MAX_SUBZONE_DISTANCE;
	static char * SUBZONE_DATA_FILE;
	static char * SUBZONE_DATA_FORMAT;
	static char * SUBZONE_DATA_FIELD;
	static char * SUBZONE_ZONE_FIELD;

	static char * NEW_SUBZONE_FIELD_x;
	static char * MAX_SUBZONE_DISTANCE_x;
	static char * SUBZONE_DATA_FILE_x;
	static char * SUBZONE_DATA_FORMAT_x;
	static char * SUBZONE_DATA_FIELD_x;
	static char * SUBZONE_ZONE_FIELD_x;

	static char * NEW_LOCATION_FIELD;
	static char * NEW_LOCATION_FIELD_x;
	static char * CONVERSION_SCRIPT;

	static char * DATA_FILE;
	static char * DATA_FORMAT;
	static char * DATA_JOIN_FIELD;
	static char * LOCATION_JOIN_FIELD;

	static char * DATA_FILE_x;
	static char * DATA_FORMAT_x;
	static char * DATA_JOIN_FIELD_x;
	static char * LOCATION_JOIN_FIELD_x;

	static char * ZONE_BOUNDARY_POLYGON;
	static char * ZONE_FIELD_NAME;
	static char * ZONE_UPDATE_RANGE;
	
	static char * BOUNDARY_POLYGON;
	static char * BOUNDARY_POLYGON_x;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum LocationData_Reports { PRINT_SCRIPT = 1, PRINT_STACK, ZONE_CHECK };

	int num_subzone, num_data_files, *data_field, *location_field, walk_access_field, walk_distance;
	int zone_field, num_polygons;
	bool copy_flag, script_flag, data_flag, polygon_flag, zone_file_flag, subzone_map_flag;
	bool walk_access_flag, walk_time_flag, zone_flag, coverage_flag, use_flag;

	Location_File *input_file, *output_file;

	Db_Header **data_files, subzone_zone_file;
	Db_Index_Array **data_db, **polygon_db;
	Db_Base **data_rec;
	Arcview_File boundary_file, **polygon_files;

	Time_Range walk_time;
	Integer_List use_code, use_field;

	Db_File program_file;
	User_Program program;

	typedef struct {
		int zone;
		double data;
	} Zone_Field_Data;
	
	typedef struct {
		int location;
		double weight;
	} Loc_Weight;

	typedef struct {
		Subzone_File file;
		Subzone_Array data;
		Index_Array loc_weight;
		Index_Array zone_field_data;
		int location_field;
		int max_distance;
		int data_field;
		int zone_field;
	} Subzone;

	Subzone **subzone;

	typedef struct {
		int location;
		int zone;
	} Zone_Map;

	typedef struct {
		int zone;
		Point_Data *points;
	} Boundary;

	typedef struct {
		int subzone;
		int location;
		double weight;
	} Weight;

	typedef struct {
		int zone;
		int subzone;
		double factor;
	} Subzone_Map;

	Index_Array zone_map, zone_boundary, **polygons, loc_weight;
	Complex_Array subzone_weight, subzone_map;
	Data_Range zone_range;
	Projection_Service projection;	

	void Set_Files (void);
	void Read_Data (int num);
	void Read_Location (void);
	void Read_Subzone_Map (void);
	void Read_Subzone (int num);
	void Read_Boundary (void);
	void Read_Polygons (void);
	void Check_Zones (void);
};
#endif


