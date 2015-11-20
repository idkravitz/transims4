//*********************************************************
//	CoordMatch.hpp - coordinate adjustment and equivalence
//*********************************************************

#ifndef COORDMATCH_HPP
#define COORDMATCH_HPP

#include "Network_Service.hpp"
#include "Node_File.hpp"
#include "Node_Data.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	CoordMatch - execution class definition
//---------------------------------------------------------

class CoordMatch : public Network_Service
{
public:

	CoordMatch (void);
	virtual ~CoordMatch (void);

	virtual void Execute (void);

protected:

	static char * REFERENCE_NODE_FILE;
	static char * NODE_MAP_FILE;
	static char * NEW_NODE_MAP_FILE;
	static char * MAXIMUM_SNAP_DISTANCE;
	static char * REPLACE_NODE_NUMBERS;

	virtual void Program_Control (void);

private:

	int max_distance, num_new_nodes, num_new_maps, num_match;
	bool ref_node_flag, node_map_flag, new_map_flag, new_node_flag, replace_flag;

	Node_File ref_node_file;
	Db_Header node_map_file, new_map_file;

	typedef struct {
		int node;
		int new_node;
		int x;
		int y;
		int dx;
		int dy;
	} Node_Map;

	Index_Array node_map;
	Node_Array ref_node_data;

	void Read_Reference (void);
	void Read_Node_Map (void);
	void Make_Node_Map (void);
	void Expand_Node_Map (void);
	void Adjust_Nodes (void);
	void Adjust_Coord (int *x, int *y);
};
#endif
