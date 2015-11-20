//******************************************************** 
//	Skim_File.hpp - Skim File Input/Output
//********************************************************

#ifndef SKIM_FILE_HPP
#define SKIM_FILE_HPP

#include "Matrix_File.hpp"

//---------------------------------------------------------
//	Skim_File Class definition
//---------------------------------------------------------

class  Skim_File : public Matrix_File
{
public:

	Skim_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Skim_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Skim_File (void);

	int Mode (void)					{ Get_Field (mode, &lvalue); return (lvalue); }
	int Count (void)				{ Get_Field (count, &lvalue); return (lvalue); }
	int Walk (void)					{ Get_Field (walk, &lvalue); return (lvalue); }
	int Wait (void)					{ Get_Field (wait, &lvalue); return (lvalue); }
	int Transit (void)				{ Get_Field (transit, &lvalue); return (lvalue); }
	int Drive (void)				{ Get_Field (drive, &lvalue); return (lvalue); }
	int Other (void)				{ Get_Field (other, &lvalue); return (lvalue); }
	int Length (void)               { Get_Field (length, &lvalue); return (lvalue); }
	int Cost (void)					{ Get_Field (cost, &lvalue); return (lvalue); }
	double Time (void)				{ Get_Field (time, &dvalue); return (dvalue); }
	double Variance (void)			{ Get_Field (variance, &dvalue); return (dvalue); }
	int Total_Time (void)			{ Get_Field (total, &lvalue); return (lvalue); }

	void Mode (int value)			{ Put_Field (mode, value); }
	void Count (int value)			{ Put_Field (count, value); }
	void Walk (int value)			{ Put_Field (walk, value); }
	void Wait (int value)			{ Put_Field (wait, value); }
	void Transit (int value)		{ Put_Field (transit, value); }
	void Drive (int value)			{ Put_Field (drive, value); }
	void Other (int value)			{ Put_Field (other, value); }
	void Length (int value)         { Put_Field (length, value); }
	void Cost (int value)			{ Put_Field (cost, value); }
	void Time (double value)		{ Put_Field (time, value); }
	void Variance (double value)	{ Put_Field (variance, value); }
	void Total_Time (int value)		{ Put_Field (total, value); }

	bool Total_Time_Flag (void)		{ return (total_flag); }
	void Total_Time_Flag (bool flag){ total_flag = flag; }

	bool Length_Flag (void)			{ return (length_flag); }
	void Length_Flag (bool flag)    { length_flag = flag; }

	bool Transit_Flag (void)		{ return (transit_flag); }
	void Transit_Flag (bool flag)	{ transit_flag = flag; }

	bool Drive_Flag (void)			{ return (drive_flag); }
	void Drive_Flag (bool flag)		{ drive_flag = flag; }

	bool Other_Flag (void)			{ return (other_flag); }
	void Other_Flag (bool flag)		{ other_flag = flag; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

	void Setup (void);

private:
	bool transit_flag, drive_flag, other_flag, total_flag, length_flag;

	int lvalue;
	int dvalue;

	int mode, count, walk, wait, transit, drive, other, length, cost;
	int time, variance, total;
};

#endif
