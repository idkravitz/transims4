//*********************************************************
//	VISSIMNet.hpp - Convert Network to VISSIM XML format
//*********************************************************

#ifndef VISSIMNET_HPP
#define VISSIMNET_HPP

#include "Demand_Service.hpp"
#include "Node_List.hpp"
#include "Link_Name.hpp"
#include "Dir_List.hpp"
#include "Db_File.hpp"
#include "Point_Array.hpp"
#include "Time_Range.hpp"

#undef  node_data
#define node_data		(*((Node_List_Array *) node_array))

#undef  link_data
#define link_data		(*((Link_Name_Array *) link_array))

#undef  dir_data
#define dir_data		(*((Dir_List_Array *) dir_array))

#define MAX_SPLITS		10
#define PARKING_OFFSET	20
#define PARKING_SPEED	5
#define OFFSET_RANGE	Round (20)

//---------------------------------------------------------
//	VISSIMNet - execution class definition
//---------------------------------------------------------

class VISSIMNet : public Demand_Service
{
public:
	VISSIMNet (void);
	virtual ~VISSIMNet (void);
	
	virtual void Execute (void);

	virtual bool Link_Processing (Db_File *file);

protected:

	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TIME_PERIOD;
	static char * NEW_VISSIM_XML_FILE;

	virtual void Program_Control (void);

private:

	Db_File xml_file;

	FILE *file;
	bool new_flag;

	Time_Range time_range;

	Point_Data points;

	Node_List_Array new_node_data;
	Link_Name_Array new_link_data;
	Dir_List_Array new_dir_data;
	Shape_Array new_shape_data;
	Pocket_Array new_pocket_data;
	Connect_Array new_connect_data;
	Parking_Array new_parking_data;
	Location_Array new_location_data;
	Access_Array new_access_data;
	Sign_Array new_sign_data;
	Signal_Array new_signal_data;
	Timing_Array new_timing_data;
	Phasing_Array new_phasing_data;
	Detector_Array new_detector_data;
	Coordinator_Array new_coordinator_data;

	typedef struct {
		int link;
		int record;
		int num_splits;
		int offset [MAX_SPLITS];
		int node [MAX_SPLITS];
		int park_ab [MAX_SPLITS];
		int park_ba [MAX_SPLITS];
		int link_ab [MAX_SPLITS];
		int link_ba [MAX_SPLITS];
	} Link_Splits;

	Index_Array link_splits;

	void New_Network (void);
	void New_Node (void);
	void New_Link (void);
	void New_Parking (void);
	void New_Location (void);
	void New_Access (void);
	void New_Connect (void);
	void New_Pocket (void);
	void New_Sign (void);
	void New_Signal (void);

	void Data_Setup (void);
	void Write_Vehicle_Types (void);
	void Write_Node (void);
	void Write_Link (void);
	void Write_Zone (void);
	void Write_Signal (void);
};
#endif
