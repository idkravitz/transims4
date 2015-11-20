//*********************************************************
//	ArcDelay.hpp - ArcView Link Delay Files
//*********************************************************

#ifndef ARCDELAY_HPP
#define ARCDELAY_HPP

#include "Demand_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Delay_File.hpp"
#include "Link_Dir_File.hpp"
#include "Link_Data_File.hpp"
#include "System_Event_File.hpp"
#include "Time_Range.hpp"
#include "Integer_Array.hpp"
#include "Complex_Array.hpp"
#include "Utility.hpp"
#include "Random.hpp"
#include "Node_List.hpp"
#include "List_Data.hpp"

#undef  node_data
#define node_data		(*((Node_List_Array *) node_array))

class ArcDelay : public Demand_Service
{
public:
	ArcDelay (void);
	virtual ~ArcDelay (void);

	virtual bool Performance_Processing (Db_File *file);
	virtual bool Pocket_Processing (Db_File *file);

	virtual void Execute (void);

protected:

	static char * LINK_DATA_FILE;
	static char * LINK_DIRECTION_FILE;
	static char * SYSTEM_EVENT_FILE;
	static char * ARCVIEW_LINK_DATA_FILE;
	static char * ARCVIEW_LINK_DIR_FILE;
	static char * ARCVIEW_DELAY_FILE;
	static char * ARCVIEW_INTERSECTION_FILE;
	static char * ARCVIEW_TURN_FILE;
	static char * ARCVIEW_TRAFFIC_IMAGE;
	static char * ARCVIEW_TRAFFIC_IMAGE_x;
	static char * TRAFFIC_IMAGE_ATTRIBUTE;
	static char * TRAFFIC_IMAGE_ATTRIBUTE_x;
	static char * VEHICLE_TYPE_DISTRIBUTION;
	static char * RANDOM_NUMBER_SEED;
	static char * DRAW_VEHICLE_SHAPES;
	static char * DRAW_NETWORK_LANES;
	static char * LANE_WIDTH;
	static char * CENTER_ONEWAY_LINKS;
	static char * LINK_DIRECTION_OFFSET;
	static char * BANDWIDTH_FIELD;
	static char * BANDWIDTH_SCALING_FACTOR;
	static char * MINIMUM_BANDWIDTH_VALUE;
	static char * MINIMUM_BANDWIDTH_SIZE;
	static char * MAXIMUM_BANDWIDTH_SIZE;
	static char * MAXIMUM_SHAPE_ANGLE;
	static char * MINIMUM_SHAPE_LENGTH;
	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TIME_PERIODS;
	static char * SELECT_TIME_INCREMENT;
	static char * SELECT_SUBAREA_POLYGON;
 	static char * ADD_LINK_DIRECTION_INDEX;
	static char * IGNORE_TIME_RANGE_FIELDS;

	virtual void Program_Control (void);

private:
	enum Image_Attributes { VOLUME, DENSITY, MAX_DENSITY, QUEUE, MAX_QUEUE, FAILURE };

	Delay_File *delay_file;
	Link_Direction_File link_dir_file;
	Link_Data_File link_data_file;
	System_Event_File event_file;

	Arcview_File select_subarea, arcview_delay, arcview_turn, arcview_link_dir, arcview_node;
	Arcview_File arcview_link_data;

	Point_Data points;

	int max_angle, min_length, width_field, green_field, yellow_field, red_field, split_field;
	double lane_width, link_offset, width_factor, min_width, max_width, min_value;
	bool center_flag, lanes_flag, turn_flag, subarea_flag, index_flag, image_flag, shape_flag;
	bool delay_flag, link_dir_flag, link_data_flag, intersection_flag, width_flag, event_flag;

	List_Array dir_list;
	Integer_List type_share;
	Time_Range time_period;
	Random random;

	Projection_Service projection;	

	char shapename [FILE_NAME_SIZE], turnname [FILE_NAME_SIZE], nodename [FILE_NAME_SIZE];

	typedef struct {
		int  number;
		int  type;
		char name [FILE_NAME_SIZE];
		Arcview_File *file;
	} Image_Data;

	Data_Array image_data;

	typedef struct {
		int link_dir;
		int pocket_id;
	} Pocket_Index;

	Complex_Array pocket_index;

	typedef struct {
		int node;
		int plan_phase;
		int time;
		int type;
		int green;
		int yellow;
		int red;
	} Signal_Time;

	Complex_Array signal_time;

	//---- methods ----

	void Read_Link_Data (void);
	void Read_Link_Dir (void);
	void Write_Delay (void);
	void Write_Turn (void);
	void Write_Image (void);
	void Write_Intersection (void);
};
#endif
