//********************************************************* 
//	Arcview_File.hpp - Arcview Shape File interface
//*********************************************************

#ifndef ARCVIEW_FILE_HPP
#define ARCVIEW_FILE_HPP

#include "Db_Header.hpp"
#include "Projection.hpp"
#include "Point_Array.hpp"
#include "Integer_Array.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	Arcview_File Class definition
//---------------------------------------------------------

class  Arcview_File : public Db_Header, public Projection
{
public:
	Arcview_File (Access_Type access = READ);
	Arcview_File (char *filename, Access_Type access = READ);
	~Arcview_File ();
	
	virtual bool Open (char *filename = NULL);
	virtual bool Close (void);
	
	virtual bool Read_Record (int number = 0);
	virtual bool Write_Record (int number = 0);

	Integer_List parts;
	Point_Data points;

	bool  Z_Flag (void)						{ return (z_flag); }
	void  Z_Flag (bool flag);

	bool  M_Flag (void)						{ return (m_flag); }
	void  M_Flag (bool flag)				{ m_flag = flag; }

	int   Num_Parts (void)					{ return (parts.Num_Records ()); }
	bool  Num_Parts (int number)			{ return (parts.Num_Records (number)); }

	int   Num_Points (void)					{ return (points.Num_Points ()); }
	bool  Num_Points (int number)			{ return (points.Num_Points (number)); }
	
	int   Max_Points (void)					{ return (points.Max_Points ()); }
	bool  Max_Points (int number)			{ return (points.Max_Points (number)); }

	int * Get_Parts (void)					{ return (parts.Record (1)); }

	XY_Point * Get_XY_Data (void)			{ return (points.Get_XY_Data (1)); }
	XY_Point * Get_XY_Data (int num)		{ return (points.Get_XY_Data (num)); }

	double * Get_Z_Data (void)				{ return (points.Get_Z_Data (1)); }
	double * Get_Z_Data (int num)			{ return (points.Get_Z_Data (num)); }

	Shape_Code Shape_Type (void)			{ return (shape_type); }
	void  Shape_Type (Shape_Code type)		{ shape_type = type; }

	bool Shape_Filename (char *path)		{ return (shape_file.Filename (path)); }
	char * Shape_Filename (void)			{ return (shape_file.Filename ()); }

	bool Send_Messages (void)				{ return (Db_Header::Send_Messages ()); }
	void Send_Messages (bool send)			{ Db_Header::Send_Messages (send); }
	
	bool Set_Projection (Projection_Data input, Projection_Data output);

private:
	void  Initialize (void);
	void  Write_Projection (void);

	enum Arc_Shapes { POINT_TYPE = 1, ARC_TYPE = 3, POLYGON_TYPE = 5, MULTIPOINT_TYPE = 8 };

	//---- XY_Range typedef ----

	typedef  struct {
		double  xmin;
		double  ymin;
		double  xmax;
		double  ymax;
	} XY_Range;

	//---- Z_Range typedef ----

	typedef  struct {
		double  min;
		double  max;
	} Range;

	//---- Arc_Header typedef ----

	typedef  struct {
		int       file_code;		//---- 9994 ----
		int       unused [5];
		int       file_size;		//---- words ----
		int       version;			//---- 1000 ----
		int       shape_type;		//---- 1, 3, 5, 8, 11, 13, 15, 18, 21, 23, 25, 28 ----
		XY_Range  box;
		Range     zbox;
		Range     mbox;
	} Arc_Header;

	//---- Arc_Record typedef ----

	typedef  struct {
		int  rec_num;			//---- 1... ----
		int  rec_size;			//---- words ----
	} Arc_Record;

	//---- Arc_Multi_Point typedef ----

	typedef  struct {			//---- shape_type = 8, 18, 28 ----
		XY_Range  box;
		int       num_pts;
	} Arc_Multi_Point;
		
	//---- Arc_Shape typedef ----

	typedef  struct {			//---- shape_types = 3, 5, 13, 15, 23, 25 ----
		XY_Range  box;
		int       num_parts;
		int       num_pts;
	} Arc_Shape;

	Shape_Code shape_type;
	
	bool z_flag, m_flag;

	XY_Range  extent;
	XY_Range  range;	
	
	Range   _extent;
	Range   _range;

	Db_File  shape_file;	
	Db_File  index_file;

	Arc_Header  shape_header;
	Arc_Header  index_header;

	void  Reorder_Bits (void *dat, int words);
};

#pragma pack (pop)
#endif
