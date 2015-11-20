//*********************************************************
//	Line_Equiv.hpp - Process Transit Route Equivalence Files
//*********************************************************

#ifndef LINE_EQUIV_HPP
#define LINE_EQUIV_HPP

#include "Integer_Array.hpp"
#include "Index_Array.hpp"
#include "Db_File.hpp"

//---------------------------------------------------------
//	Line_Equiv Class definition
//---------------------------------------------------------

class Line_Equiv : public Index_Array, public Db_File
{
public:
	Line_Equiv (void);
	virtual ~Line_Equiv (void);

	bool Read (bool report_flag);

	char * Group_Label (int group);
	Integer_List * Group_List (int group);

	int  Num_Groups (void)				{ return (Index_Array::Num_Records ()); };
	int  Max_Group (void)				{ return (max_group); }

private:
	int max_group;

	typedef struct {
		int  group;
		char *label;
		Integer_List *list;
	} Group_Data;
};

#endif
