//*********************************************************
//	Shape_Data.hpp - shape point classes
//*********************************************************

#ifndef SHAPE_DATA_HPP
#define SHAPE_DATA_HPP

#include "Class_Array.hpp"
#include "Point_Array.hpp"

#include <string.h>

//---------------------------------------------------------
//	Shape_Data class definition
//---------------------------------------------------------

class Shape_Data : public Class_Index
{
public:
	Shape_Data (int link = 0);
	virtual ~Shape_Data (void)           { Clear (); }

	int  Link (void)                     { return (ID ()); }
	int  Points (void)                   { return ((point) ? point->Num_Points () : 0); }
	XYZ * Point (int num)                { return ((point) ? point->Point (num) : NULL); }
	bool Point (int num, int *x, int *y, int *z = NULL);

	XYZ * First_Point (void)             { return ((point) ? point->First () : NULL); }
	XYZ * Next_Point (void)              { return ((point) ? point->Next () : NULL); }
	
	XYZ * Last_Point (void)              { return ((point) ? point->Last () : NULL); }
	XYZ * Previous_Point (void)          { return ((point) ? point->Previous () : NULL); }

	void Link (int value)                { ID (value); }
	bool Points (int value);
	bool Point (int x, int y, int z = 0) { XYZ pt; pt.x = x; pt.y = y; pt.z = z; return (Point (&pt)); }
	bool Point (XYZ *value)              { return ((point) ? point->Add (value) : false); }
	bool Point (int num, XYZ *value)     { return ((point) ? point->Set (num, value) : false); } 

	bool Max_Points (int value);

	void Clear (void)                    { if (point) point->Clear (); }

	void Flip_Points (void)              { if (point) point->Flip_Points (); }

private:
	XYZ_Array *point;
};

//---------------------------------------------------------
//	Shape_Array class definition
//---------------------------------------------------------

class Shape_Array : public Class_Array
{
public:
	Shape_Array (int max_records = 0);
	virtual ~Shape_Array (void);

	bool Add (Shape_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Shape_Data * New_Record (bool clear = false, int number = 1)
	                                    { return ((Shape_Data *) Class_Array::New_Record (clear, number)); }

	Shape_Data * Record (int index)     { return ((Shape_Data *) Class_Array::Record (index)); }
	Shape_Data * Record (void)          { return ((Shape_Data *) Class_Array::Record ()); }

	Shape_Data * Get (int id)           { return ((Shape_Data *) Class_Array::Get (id)); }
	Shape_Data * Get_GE (int id)        { return ((Shape_Data *) Class_Array::Get_GE (id)); }
	Shape_Data * Get_LE (int id)        { return ((Shape_Data *) Class_Array::Get_LE (id)); }

	Shape_Data * First (void)           { return ((Shape_Data *) Class_Array::First ()); }
	Shape_Data * Next (void)            { return ((Shape_Data *) Class_Array::Next ()); }
	
	Shape_Data * Last (void)            { return ((Shape_Data *) Class_Array::Last ()); }
	Shape_Data * Previous (void)        { return ((Shape_Data *) Class_Array::Previous ()); }

	Shape_Data * First_Key (void)       { return ((Shape_Data *) Class_Array::First_Key ()); }
	Shape_Data * Next_Key (void)        { return ((Shape_Data *) Class_Array::Next_Key ()); }

	Shape_Data * operator[] (int index) { return (Record (index)); }

	bool Z_Flag (void)                  { return (z_flag); }
	void Z_Flag (bool flag)             { z_flag = flag; }

private:
	bool z_flag;
};

#endif
