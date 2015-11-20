//*********************************************************
//	TransimsNet.hpp - Network Conversion Utility
//*********************************************************

#ifndef TRANSIMSNET_HPP
#define TRANSIMSNET_HPP

#include "Network_Service.hpp"
#include "Random.hpp"
#include "Time_Step.hpp"
#include "Integer_Array.hpp"
#include "Db_Header.hpp"
#include "Dir_Connect.hpp"
#include "Node_List.hpp"
#include "Data_Range.hpp"
#include "Projection.hpp"
#include "Projection_Service.hpp"
#include "Link_Detail_File.hpp"
#include "Link_Detail_Data.hpp"

#include "Data/Link_Nodes.hpp"
#include "Data/Area_Type.hpp"

#undef  node_data
#define node_data		(*((Node_List_Array *) node_array))

#undef  link_data
#define link_data		(*((Link_Nodes_Array *) link_array))

#undef  dir_data
#define dir_data		(*((Dir_Connect_Array *) dir_array))

//---------------------------------------------------------
//	TransimsNet - execution class definition
//---------------------------------------------------------

class TransimsNet : public Network_Service
{
public:
	TransimsNet (void);
	virtual ~TransimsNet (void);

	virtual void Execute (void);

	virtual bool Link_Processing (Db_File *file);
	virtual bool Zone_Processing (Db_File *file);
	virtual bool Location_Processing (Db_File *file);
	virtual bool Parking_Processing (Db_File *file);
	virtual bool Access_Processing (Db_File *file);
	virtual bool Pocket_Processing (Db_File *file);
	virtual bool Connectivity_Processing (Db_File *file);
	virtual bool Sign_Processing (Db_File *file);
	virtual bool Signal_Processing (Db_File *file);
	
protected:

	static char * KEEP_NODE_LIST;
	static char * LINK_NODE_EQUIVALENCE;
	static char * POCKET_LENGTHS_FOR_FACILITY_x;
	static char * POCKET_TO_FACILITY_BY_AREA_TYPE;
	static char * SIGNAL_WARRANT_FOR_AREA_TYPE_x;
	static char * STOP_WARRANT_FOR_AREA_TYPE_x;
	static char * ACTIVITY_LOCATION_SIDE_OFFSET;
	static char * FACILITY_TYPE_ACCESS_FLAGS;
	static char * MAXIMUM_ACCESS_POINTS;
	static char * MINIMUM_SPLIT_LENGTHS;
	static char * CELL_SIZE;
	static char * MINIMUM_LINK_LENGTH;
	static char * MAXIMUM_LENGTH_TO_XY_RATIO;
	static char * MAXIMUM_CONNECTION_ANGLE;
	static char * ADD_UTURN_TO_DEADEND_LINKS;
	static char * INTERSECTION_SETBACK_DISTANCE;
	static char * FIRST_EXTERNAL_ZONE_NUMBER;
	static char * KEEP_ZONE_CONNECTORS_AS_LOCALS;
	static char * REPLICATE_MPO_NETWORK;
	static char * COLLAPSE_NODES_FLAG;
	static char * UPDATE_NODE_RANGE;
	static char * UPDATE_LINK_RANGE;
	static char * UPDATE_NODE_FILE;
	static char * UPDATE_LINK_FILE;
	static char * DELETE_NODE_RANGE;
	static char * DELETE_LINK_RANGE;
	static char * DELETE_NODE_FILE;
	static char * DELETE_LINK_FILE;
	static char * LINK_DETAIL_FILE;

	virtual void Program_Control (void);

private:

	enum Node_Type { KEEP_NODE = 1, DELETE_NODE, INTERSECTION, INTERCHANGE, 
					CHANGE_POINT, ZONE_CENTROID, DEAD_END };

	bool access_flag [EXTERNAL+1], update_flag, delete_flag, connect_flag, replicate_flag;
	bool update_node_flag, update_link_flag, delete_node_flag, delete_link_flag;
	bool zone_flag, zout_flag, turn_flag, shape_flag, name_flag, collapse_flag, uturn_flag;
	bool pocket_to_flag, detail_flag, lane_use_flag;
	double max_length_factor, location_side, cell_size;

	int straight_diff, thru_diff, max_angle, short_length;

	int nnode, nlink, nparking, nactivity, nprocess, npocket, nconnect, nsign, nsignal, nlane_use;
	int mparking, mactivity, mprocess, mpocket;
	int xlink, xnode, xparking, xactivity, xprocess, xpocket, xconnect, xsign, xsignal;
	int nshort, nlength, nexternal, nzone, nzout, ext_zone, high_zone, max_splits;
	int nfixed1, nfixed2, nfixed3, nactuated1, nactuated2, nactuated3, nstop, nyield;
	int nshape, nshapes, nturn;
	int min_length, setback, external_offset;

	Db_File keep_nodes, turn_nodes, node_string;
	Db_File update_link_file, update_node_file, delete_link_file, delete_node_file;

	Area_Type_Array area_type;
	Integer_List pocket_len [EXTERNAL], pocket_to;
	Integer_Sort zone_centroid;

	Data_Range update_nodes, update_links, repair_nodes, delete_nodes, delete_links;

	Link_Detail_File detail_file;
	Link_Detail_Array detail_data;
	
	Projection coord;
	Projection_Service projection;

	//---- methods ----

	void Read_Files (void);
	void Read_Details (void);
	void Data_Setup (void);
	bool Delete_Node (Node_List *node);
	void Connection_List (void);
	void Write_Node (void);
	void Write_Link (void);
	void Pocket_Lanes (Dir_Connect *dir, Node_List *node, int link_len);
	void Intersection_Data (void);
	void Lane_Connect (int dir, Node_List *node);
	int  Closest_Zone (int x, int y);
	void External_Station (Link_Name *link_ptr);
	void Write_Turn (void);
	void Write_Lane_Use (Link_Data *link_ptr);
};
#endif


