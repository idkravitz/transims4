//*********************************************************
//	TPPlusRoute.hpp - TP+ Transit Network Conversion
//*********************************************************

#ifndef TPPLUSROUTE_HPP
#define TPPLUSROUTE_HPP

#include "Network_Service.hpp"
#include "Route_Header_File.hpp"
#include "Route_Nodes_File.hpp"
#include "Db_Header.hpp"

#define MAX_NODES		10000

//---------------------------------------------------------
//	TPPlusRoute - execution class definition
//---------------------------------------------------------

class TPPlusRoute : public Network_Service
{
public:

	TPPlusRoute (void);
	virtual ~TPPlusRoute (void);

	virtual void Execute (void);

protected:

	static char * TRANSIT_TIME_PERIODS;
	static char * PERIOD_TRAVEL_TIMES;
	static char * FIRST_ROUTE_NUMBER;
	static char * TPPLUS_LINE_FILE_x;
	static char * TPPLUS_PERIOD_MAP_x;
	static char * TPPLUS_FLIP_ROUTE_x;
	static char * TPPLUS_MODE_MAP_x;
	static char * TPPLUS_NODE_MAP_FILE;

	virtual void Program_Control (void);

private:
	int nnode, mode, period, ttime, offset, *headway, node [MAX_NODES];
	int route_id, nroute, nnodes, num_periods, num_match, num_modes, *mode_map;
	bool node_flag, link_flag, time_flag, input_flag, map_flag, oneway, flip;
	char name [FIELD_BUFFER], notes [FIELD_BUFFER];

	Route_Header_File *header_file;
	Route_Nodes_File *nodes_file;

	Db_Header node_map_file;

	typedef struct {
		int group;
		Db_File *line_file;
		int *period_map;
		bool flip;
	} File_Group;

	Data_Array file_group;

	typedef struct {
		int node;
		int new_node;
	} Node_Map;

	Index_Array node_map;

	void Read_Node_Map (void);
	void Read_Line (void);
	void Save_Route (void);
	void Write_Route (void);
};
#endif
