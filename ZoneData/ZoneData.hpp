//*********************************************************
//	ZoneData.hpp - Zone Data Processing
//*********************************************************

#ifndef ZONEDATA_HPP
#define ZONEDATA_HPP

#include "Network_Service.hpp"
#include "Zone_File.hpp"
#include "User_Program.hpp"
#include "Time_Step.hpp"
#include "Db_Array.hpp"
#include "Db_Header.hpp"
#include "Arcview_File.hpp"
#include "Projection_Service.hpp"

//---------------------------------------------------------
//	ZoneData - execution class definition
//---------------------------------------------------------

class ZoneData : public Network_Service
{
public:

	ZoneData (void);
	virtual ~ZoneData (void);

	virtual void Execute (void);

protected:

	static char * COPY_EXISTING_FIELDS;
	static char * NEW_ZONE_FIELD;
	static char * NEW_ZONE_FIELD_x;
	static char * CONVERSION_SCRIPT;
	static char * DATA_FILE;
	static char * DATA_FORMAT;
	static char * DATA_JOIN_FIELD;
	static char * ZONE_JOIN_FIELD;
	static char * DATA_FILE_x;
	static char * DATA_FORMAT_x;
	static char * DATA_JOIN_FIELD_x;
	static char * ZONE_JOIN_FIELD_x;
	static char * BOUNDARY_POLYGON;
	static char * BOUNDARY_POLYGON_x;

	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum ZoneData_Reports { PRINT_SCRIPT = 1, PRINT_STACK };

	int num_data_files, *data_field, *zone_field, num_polygons;
	bool copy_flag, script_flag, data_flag, polygon_flag;
	
	Zone_File *input_file, *output_file;

	Db_Header **data_files;
	Db_Index_Array **data_db, **polygon_db;
	Db_Base **data_rec;
	Arcview_File **polygon_files;

	Db_File program_file;
	User_Program program;

	typedef struct {
		int zone;
		int record;
	} Zone_Map;

	typedef struct {
		int zone;
		Point_Data *points;
	} Boundary;

	Index_Array **polygons;
	Projection_Service projection;	

	void Set_Files (void);
	void Read_Data (int num);
	void Read_Zone (void);
	void Read_Polygons (void);
};
#endif


