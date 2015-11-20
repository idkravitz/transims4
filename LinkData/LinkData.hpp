//*********************************************************
//	LinkData.hpp - Convert Link Data to Link ID
//*********************************************************

#ifndef LISTID_HPP
#define LISTID_HPP

#include "Execution_Service.hpp"
#include "Complex_Array.hpp"
#include "Index_Array.hpp"
#include "Db_Header.hpp"
#include "System_Defines.hpp"

class LinkData : public Execution_Service
{
public:
	LinkData (void);
	virtual ~LinkData (void);

	virtual void Execute (void);

protected:

	static char * LINK_NODE_EQUIVALENCE;
	static char * DIRECTIONAL_DATA_FILE;
	static char * FROM_NODE_FIELD_NAME;
	static char * TO_NODE_FIELD_NAME;
	static char * VOLUME_DATA_FIELD_NAME;
	static char * SPEED_DATA_FIELD_NAME;
	static char * NEW_LINK_DATA_FILE;
	static char * NEW_AB_VOLUME_FIELD_NAME;
	static char * NEW_BA_VOLUME_FIELD_NAME;
	static char * NEW_AB_SPEED_FIELD_NAME;
	static char * NEW_BA_SPEED_FIELD_NAME;
	static char * NEW_DIRECTIONAL_DATA_FILE;

	virtual void Program_Control (void);

private:

	bool speed_flag, output_flag; 
	int nequiv, nab, ndir, nlink;
	int from_field, to_field, vol_field, spd_field;
	int ab_vol_field, ba_vol_field, ab_spd_field, ba_spd_field;

	typedef struct {
		int anode;
		int bnode;
		int link;
	} Link_Equiv;

	typedef struct {
		int link;
		int anode;
		int bnode;
		double volume_ab;
		double volume_ba;
		double speed_ab;
		double speed_ba;
		int num_ab;
		int num_ba;
	} Link_Data;

	Complex_Array link_equiv;
	Index_Array link_data;

	Db_File link_node;
	Db_Header dir_file, data_file, new_dir_file;

	void Read_Equiv (void);
	void Read_Data (void);
	void Write_Data (void);
};
#endif
