//*********************************************************
//	Link_Equiv.hpp - Process Link Equivalence Files
//*********************************************************

#ifndef LINK_EQUIV_HPP
#define LINK_EQUIV_HPP

#include "Integer_Array.hpp"
#include "Index_Array.hpp"
#include "Db_File.hpp"

//---------------------------------------------------------
//	Link_Equiv Class definition
//---------------------------------------------------------

class Link_Equiv : public Index_Array, public Db_File
{
public:
	Link_Equiv (void);
	virtual ~Link_Equiv (void);

	bool Read (bool report_flag);

	char * Group_Label (int group);
	Integer_List * Group_List (int group);

	int  Num_Groups (void)				{ return (Index_Array::Num_Records ()); };
	int  Max_Group (void)				{ return (max_group); }

	int  First_Group (void);
	int  Next_Group (void);

private:
	int max_group;

	typedef struct {
		int  group;
		char *label;
		Integer_List *list;
	} Group_Data;
};

#endif
