//*********************************************************
//	Point_Array.hpp - dynamic point array
//*********************************************************

#ifndef POINT_ARRAY_HPP
#define POINT_ARRAY_HPP

#include "Data_Array.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	Point_Array
//---------------------------------------------------------

class Point_Array : public Data_Array
{
public:
	Point_Array (int max_points = 0);

	int  Num_Points (void)                 { return (Data_Array::Num_Records ()); }
	bool Num_Points (int number)           { return (Data_Array::Num_Records (number)); }

	int  Max_Points (void)                 { return (Data_Array::Max_Records ()); }
	bool Max_Points (int number)           { return (Data_Array::Max_Records (number)); }

	bool Add (XY_Point *data = NULL)       { return (Data_Array::Add (data)); }
	bool Add (int number)                  { return (Data_Array::Add_Records (number)); }

	bool Set (int index, XY_Point *data)   { return (Data_Array::Set (index, data)); }	

	XY_Point * New_Points (int number = 1) { return ((XY_Point *) Data_Array::New_Record (false, number)); }

	XY_Point * Point (int index)           { return ((XY_Point *) Data_Array::Record (index)); }
	XY_Point * Point (void)                { return ((XY_Point *) Data_Array::Record ()); }

	XY_Point * First (void)                { return ((XY_Point *) Data_Array::First ()); }
	XY_Point * Next (void)                 { return ((XY_Point *) Data_Array::Next ()); }
	
	XY_Point * Last (void)                 { return ((XY_Point *) Data_Array::Last ()); }
	XY_Point * Previous (void)             { return ((XY_Point *) Data_Array::Previous ()); }

	XY_Point * operator[] (int index)      { return (Point (index)); }

	void Flip_Points (void)                { Data_Array::Flip_Records (); }
};

//---------------------------------------------------------
//	Z_Array
//---------------------------------------------------------

class Z_Array : public Data_Array
{
public:
	Z_Array (int max_points = 0);

	int  Num_Points (void)                 { return (Data_Array::Num_Records ()); }
	bool Num_Points (int number)           { return (Data_Array::Num_Records (number)); }

	int  Max_Points (void)                 { return (Data_Array::Max_Records ()); }
	bool Max_Points (int number)           { return (Data_Array::Max_Records (number)); }

	bool Add (double *data = NULL)         { return (Data_Array::Add (data)); }
	bool Add (int number)                  { return (Data_Array::Add_Records (number)); }

	bool Set (int index, double *data)     { return (Data_Array::Set (index, data)); }	

	double * New_Points (int number = 1)   { return ((double *) Data_Array::New_Record (false, number)); }

	double * Point (int index)             { return ((double *) Data_Array::Record (index)); }
	double * Point (void)                  { return ((double *) Data_Array::Record ()); }

	double * First (void)                  { return ((double *) Data_Array::First ()); }
	double * Next (void)                   { return ((double *) Data_Array::Next ()); }
	
	double * Last (void)                   { return ((double *) Data_Array::Last ()); }
	double * Previous (void)               { return ((double *) Data_Array::Previous ()); }

	double * operator[] (int index)        { return (Point (index)); }

	void Flip_Points (void)                { Data_Array::Flip_Records (); }
};

//---------------------------------------------------------
//	Point_Data
//---------------------------------------------------------

class Point_Data
{
public:
	Point_Data (int max_points = 0, bool z_flag = false);

	bool  Z_Flag (void)                { return (z_flag); }
	void  Z_Flag (bool flag)           { z_flag = flag; }

	int  Num_Points (void)             { return (xy_data.Num_Points ()); }
	bool Num_Points (int num)          { if (z_flag) z_data.Num_Points (num); return (xy_data.Num_Points (num)); }

	int  Max_Points (void)             { return (xy_data.Max_Points ()); }
	bool Max_Points (int num)          { if (z_flag) z_data.Max_Points (num); return (xy_data.Max_Points (num)); }

	XY_Point * Get_XY_Data (int num)   { return (xy_data [num]); }
	double * Get_Z_Data (int num)      { return (z_data [num]); }

	XYZ_Point * Point (int num);

	XYZ_Point * First (void);
	XYZ_Point * Next (void);
	
	XYZ_Point * Last (void);
	XYZ_Point * Previous (void);

	bool Add (XYZ_Point *data = NULL);

	bool Set (XYZ_Point *data = NULL)  { return (Set (Record_Index (), data)); }
	bool Set (int index, XYZ_Point *data = NULL);

	int  Record_Index (void)           { return (xy_data.Record_Index ()); }
	void Reset (void)                  { xy_data.Reset (); z_data.Reset (); }

	XYZ_Point * operator[] (int num)   { return (Point (num)); }

	void Flip_Points (void)            { xy_data.Flip_Points (); z_data.Flip_Points (); }

private:
	bool z_flag;

	Point_Array xy_data;
	Z_Array z_data;

	XYZ_Point pt;
};

//---------------------------------------------------------
//	XYZ_Array
//---------------------------------------------------------

class XYZ_Array : public Data_Array
{
public:
	XYZ_Array (int max_points = 0);

	int  Num_Points (void)                 { return (Data_Array::Num_Records ()); }
	bool Num_Points (int number)           { return (Data_Array::Num_Records (number)); }

	int  Max_Points (void)                 { return (Data_Array::Max_Records ()); }
	bool Max_Points (int number)           { return (Data_Array::Max_Records (number)); }

	bool Add (XYZ *data = NULL)            { return (Data_Array::Add (data)); }
	bool Add (int number)                  { return (Data_Array::Add_Records (number)); }

	bool Set (int index, XYZ *data)        { return (Data_Array::Set (index, data)); }	

	XYZ * New_Points (int number = 1)       { return ((XYZ *) Data_Array::New_Record (false, number)); }

	XYZ * Point (int index)                 { return ((XYZ *) Data_Array::Record (index)); }
	XYZ * Point (void)                      { return ((XYZ *) Data_Array::Record ()); }

	XYZ * First (void)                      { return ((XYZ *) Data_Array::First ()); }
	XYZ * Next (void)                       { return ((XYZ *) Data_Array::Next ()); }
	
	XYZ * Last (void)                       { return ((XYZ *) Data_Array::Last ()); }
	XYZ * Previous (void)                   { return ((XYZ *) Data_Array::Previous ()); }

	XYZ * operator[] (int index)            { return (Point (index)); }

	void Flip_Points (void)                { Data_Array::Flip_Records (); }
};

#endif
