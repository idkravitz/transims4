//*********************************************************
//	ArcSnapshot.hpp - ArcView Snapshot Files
//*********************************************************

#ifndef ARCSNAPSHOT_HPP
#define ARCSNAPSHOT_HPP

#include "Demand_Service.hpp"
#include "Projection.hpp"
#include "Projection_Service.hpp"
#include "Occupancy_File.hpp"
#include "Arcview_File.hpp"
#include "Time_Range.hpp"
#include "Data_Range.hpp"
#include "Utility.hpp"

class ArcSnapshot : public Demand_Service
{
public:
	ArcSnapshot (void);
	virtual ~ArcSnapshot (void);

	virtual void Execute (void);

protected:
	static char * SNAPSHOT_FILE;
	static char * SNAPSHOT_FORMAT;
	static char * OCCUPANCY_FILE;
	static char * OCCUPANCY_FORMAT;
	static char * ARCVIEW_SNAPSHOT_FILE;
	static char * LANE_WIDTH;
	static char * CELL_SIZE;
	static char * CENTER_ONEWAY_LINKS;
	static char * DRAW_VEHICLE_SHAPES;
	static char * ADD_PASSENGER_CIRCLE_SIZE;
	static char * ADD_PASSENGER_SQUARE_SIZE;
	static char * TIME_STEPS_PER_SECOND;
	static char * PAD_FILE_TIME_LABEL;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_TIME_INCREMENT;
	static char * SELECT_SUBAREA_POLYGON;
	static char * SELECT_VEHICLE_TYPES;
	static char * TIME_PROCESSING_METHOD;
	static char * NEW_SNAPSHOT_FILE;
	static char * NEW_SNAPSHOT_FORMAT;
	static char * NEW_LINK_SUMMARY_FILE;
	static char * NEW_LINK_SUMMARY_FORMAT;
 
	virtual void Program_Control (void);

private:
	enum Method_Types { AT_INCREMENT, TOTAL, MAXIMUM };

	Snapshot_File snapshot_file, new_snapshot_file;
	Occupancy_File occupancy_file;
	Db_Header summary_file;

	Arcview_File arcview_snapshot;
	Arcview_File select_subarea;

	Point_Data points;

	double lane_width, cell_size, *veh_length;
	int method, nsaved, max_type, max_time, steps_per_second;
	int link_field, dir_field, offset_field, lane_field, type_field, occ_field, start_field, end_field;
	int link_fld, start_fld, end_fld, *veh_fld, *per_fld, num_links, num_sum, circle_field, square_field;
	bool center_flag, shape_flag, subarea_flag, snapshot_flag, occupancy_flag, output_flag, project_flag;
	bool pad_flag, sum_flag, veh_type_flag, circle_flag, square_flag;

	Time_Range time_period;
	Data_Range veh_types;

	int **vehicles, **persons;

	Projection project_coord;
	Projection_Service projection;	

	char shapename [FILE_NAME_SIZE];
	
	typedef struct {
		int lanes;
		int cells;
		int total;
		int max_total;
		int *data;
		int *maximum;
	} Cell_Data;
		
	Data_Array cell_data;

	//---- methods ----

	void Set_Subarea (void);
	void Set_Veh_Types (void);
	void Create_Cells (void);
	void Read_Snapshot (void);
	void Sum_Snapshot (void);
	void Maximum_Check (void);
	void Write_Snapshot (int period);
	void Write_Summary (int period);
	void Draw_Vehicle (void);
};
#endif


