//*********************************************************
//	DynusTNet.hpp - Convert Network to DynusT text format
//*********************************************************

#ifndef DYNUSTNET_HPP
#define DYNUSTNET_HPP

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
//	DynusTNet - execution class definition
//---------------------------------------------------------

class DynusTNet : public Demand_Service
{
public:
	DynusTNet (void);
	virtual ~DynusTNet (void);
	
	virtual void Execute (void);

	virtual bool Link_Processing (Db_File *file);

protected:

	static char * TIME_OF_DAY_FORMAT;
	static char * SELECT_TIME_PERIOD;
	static char * DYNUST_PROJECT_NAME;
	static char * DYNUST_NETWORK_DIRECTORY;
	static char * SATURATION_FLOW_RATES;
	static char * SERVICE_FLOW_RATES;

	virtual void Program_Control (void);

private:

	char net_dir [FILE_NAME_SIZE];
	int min_x, min_y, nzone, sat_flow [EXTERNAL+1], ser_flow [EXTERNAL+1];
	bool name_flag;

	Db_File dws_file;
	Db_File xy_file;
	Db_File shp_file;
	Db_File net_file;
	Db_File name_file;
	Db_File move_file;
	Db_File ctl_file;
	Db_File org_file;
	Db_File des_file;
	Db_File bus_file;
	Db_File pce_file;
	Db_File left_file;
	Db_File out_file;
	Db_File ramp_file;
	Db_File cap2_file;
	Db_File cap4_file;
	Db_File super_file;
	Db_File model_file;
	Db_File vms_file;
	Db_File yield_file;
	Db_File zone_file;
	Db_File dem_file;
	Db_File hov_file;
	Db_File trk_file;
	Db_File zmap_file;

	Time_Range time_range;

	Point_Data points;

	void Data_Setup (void);
	void Write_Coordinates (void);
	void Write_Network (void);
	void Write_Shapes (void);
	void Write_Connections (void);
	void Write_Controls (void);
	void Write_Zones (void);
	void Write_Dummy (void);
};
#endif
