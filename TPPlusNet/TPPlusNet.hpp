//*********************************************************
//	TPPlusNet.hpp - TP+ Network Conversion
//*********************************************************

#ifndef TPPLUSNET_HPP
#define TPPLUSNET_HPP

#include "Network_Service.hpp"
#include "User_Program.hpp"
#include "Projection.hpp"
#include "Projection_Service.hpp"
#include "Link_Name.hpp"
#include "Arcview_File.hpp"
#include "Shape_File.hpp"
#include "Link_Detail_File.hpp"

#define MAX_AT		100
#define MAX_FUNCL	100
#define MAX_LANE	100

#undef  link_data
#define link_data		(*((Link_Name_Array *) link_array))

//---------------------------------------------------------
//	TPPlusNet - execution class definition
//---------------------------------------------------------

class TPPlusNet : public Network_Service
{
public:

	TPPlusNet (void);
	virtual ~TPPlusNet (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:

	static char * TPPLUS_LINK_FILE;
	static char * TPPLUS_LINK_FORMAT;
	static char * TPPLUS_NODE_FILE;
	static char * TPPLUS_NODE_FORMAT;
	static char * TPPLUS_SPDCAP_FILE;
	static char * TPPLUS_SPDCAP_FORMAT;
	static char * CONVERT_MPH_TO_MPS;
	static char * FACILITY_INDEX_FIELD;
	static char * AREA_TYPE_INDEX_FIELD;
	static char * CONVERSION_SCRIPT;
	static char * EXTERNAL_ZONE_NUMBER;
	static char * NEW_LINK_DETAIL_FILE;

	virtual void Program_Control (void);

private:
	enum TPPlusNet_Reports { SCRIPT_REPORT = 1, STACK_REPORT };

	int cap_map [MAX_AT] [MAX_FUNCL] [MAX_LANE];
	int spd_map [MAX_AT] [MAX_FUNCL] [MAX_LANE];

	int ext_zone, fac_fld, at_fld;
	bool convert_flag, spdcap_flag, link_shape_flag, node_shape_flag, units_flag, detail_flag;

	Db_Header link_file, node_file;
	Db_Base spdcap_file;
	Db_File script_file;
	Link_Detail_File detail_file;
	
	Arcview_File link_shape_file, node_shape_file;
	Shape_File *shape_file;

	User_Program convert;
	
	Projection projection;
	Projection_Service proj_service;

	void Read_SpdCap (void);
	void Read_Node (void);
	void Read_Link (void);
	void Write_Link (void);
};
#endif
