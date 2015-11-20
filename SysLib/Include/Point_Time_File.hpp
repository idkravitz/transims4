//******************************************************** 
//	Point_Time_File.hpp - Point Data File Input/Output
//********************************************************

#ifndef POINT_TIME_FILE_HPP
#define POINT_TIME_FILE_HPP

#include "Db_Header.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Point_Time_File Class definition
//---------------------------------------------------------

class  Point_Time_File : public Db_Header, public Time_Range
{
public:

	Point_Time_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Point_Time_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Point_Time_File (void);

	int ID (void)							{ Get_Field (id, &lvalue); return (lvalue); }
	int Total_In (int period)				{ Get_Field (In (period), &lvalue); return (lvalue); }
	int Total_Out (int period)				{ Get_Field (Out (period), &lvalue); return (lvalue); }
	double Data_In (int period)				{ Get_Field (In (period), &dvalue); return (dvalue); }
	double Data_Out (int period)			{ Get_Field (Out (period), &dvalue); return (dvalue); }

	void ID (int value)						{ Put_Field (id, value); }
	void Total_In (int period, int value)	{ Put_Field (In (period), value); }
	void Total_Out (int period, int value)	{ Put_Field (Out (period), value); }
	void Data_In (int period, double value)	{ Put_Field (In (period), value); }
	void Data_Out (int period, double value){ Put_Field (Out (period), value); }

	void Num_Decimals (int number)			{ decimal = number; }

	virtual bool Create_Fields (void);
	bool Create_Fields (int low, int high, int increment = 0);
	
	void Zero_Fields (void);

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	void Clear_Fields (void);

	int  In (int period)		{ return ((in != NULL && Check_Index (period)) ? in [period] : 0); }
	int  Out (int period)		{ return ((out != NULL && Check_Index (period)) ? out [period] : 0); }

	int lvalue, id, decimal, *in, *out;
	double dvalue;
};

#endif
