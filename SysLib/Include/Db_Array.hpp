//********************************************************* 
//	Db_Array.hpp - database arrays interface
//*********************************************************

#ifndef DB_ARRAY_HPP
#define DB_ARRAY_HPP

#include "Db_Base.hpp"
#include "Data_Array.hpp"
#include "Index_Array.hpp"

//---------------------------------------------------------
//	Db_Data_Array Class definition
//---------------------------------------------------------

class  Db_Data_Array : public Db_Base, public Data_Array
{
public:
	Db_Data_Array ();

	virtual bool Read_Record (int number = 0); 
	virtual bool Write_Record (int number = 0);
	virtual bool Add_Record (void);

	virtual bool Record_Size (int size);

	virtual bool Rewind (void)			{ return (Record_Index (0)); }

	int  Num_Records (void)				{ return (Data_Array::Num_Records ()); }
	bool Num_Records (int num)			{ return (Data_Array::Num_Records (num)); }
};

//---------------------------------------------------------
//	Db_Index_Array Class definition
//---------------------------------------------------------

class  Db_Index_Array : public Db_Base, public Index_Array
{
public:
	Db_Index_Array ();

	virtual bool Read_Record (int number = 0); 
	virtual bool Write_Record (int number = 0);
	virtual bool Add_Record (void);

	virtual bool Get_Record (int key)	{ return (Get_Record (&key)); }
	virtual bool Get_Record (void *key)	{ int i = Get_Index (key); return ((i > 0) ? Read_Record (i) : false); }

	virtual bool Record_Size (int size);

	virtual bool Rewind (void)			{ return (Record_Index (0)); }

	int  Num_Records (void)				{ return (Index_Array::Num_Records ()); }
	bool Num_Records (int num)			{ return (Index_Array::Num_Records (num)); }
};

#endif
