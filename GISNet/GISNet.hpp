//*********************************************************
//	GISNet.hpp - Arcview Network Conversion
//*********************************************************

#ifndef GISNET_HPP
#define GISNET_HPP

#include "Network_Service.hpp"
#include "Arcview_File.hpp"
#include "User_Program.hpp"
#include "Projection_Service.hpp"
#include "Link_File.hpp"
#include "Node_File.hpp"
#include "Shape_File.hpp"

class GISNet : public Network_Service
{
public:
	GISNet (void);
	virtual ~GISNet (void);
	
	virtual void Execute (void);
	virtual void Page_Header (void);

protected:

	static char * GIS_NODE_FILE;
	static char * GIS_LINK_FILE;
	static char * CONVERSION_SCRIPT;
	static char * FIRST_LINK_NUMBER;
	static char * FIRST_NODE_NUMBER;
	static char * MAXIMUM_SHAPE_ANGLE;
	static char * MINIMUM_SHAPE_LENGTH;
	static char * UPDATE_LINK_LENGTHS;

	virtual void Program_Control (void);

private:
	enum GISNet_Reports { SCRIPT_REPORT = 1, STACK_REPORT };

	Link_File *link_file;
	Node_File *node_file;
	Shape_File *shape_file;

	int first_link, first_node;
	int max_angle, min_length;
	
	int nlink, nnode, nshape, nrec, num_in, diff;

	bool convert_flag, node_flag, update_flag;

	Arcview_File gis_file, gis_node;

	Db_File script_file;

	User_Program convert;
	
	Projection_Service projection;

	void Read_Node (void);
	void Read_Link (void);
};
#endif
