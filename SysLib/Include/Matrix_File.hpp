//******************************************************** 
//	Matrix_File.hpp - Matrix File Input/Output
//********************************************************

#ifndef MATRIX_FILE_HPP
#define MATRIX_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Matrix_File Class definition
//---------------------------------------------------------

class  Matrix_File : public Db_Header
{
public:

	Matrix_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Matrix_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Matrix_File (void);

	int Origin (void)				{ Get_Field (org, &lvalue); return (lvalue); }
	int Destination (void)			{ Get_Field (des, &lvalue); return (lvalue); }
	int Period (void)				{ Get_Field (period, &lvalue); return (lvalue); }
	int Data (void)					{ Get_Field (data, &lvalue); return (lvalue); }

	void Origin (int value)			{ Put_Field (org, value); }
	void Destination (int value)	{ Put_Field (des, value); }
	void Period (int value)			{ Put_Field (period, value); }
	void Data (int value)			{ Put_Field (data, value); }

	void Period_Flag (bool flag)	{ period_flag = flag; }
	bool Period_Flag (void)			{ return (period_flag); }

	int  Origin_Field (void)		{ return (org); }
	int  Destination_Field (void)	{ return (des); }
	int  Period_Field (void)		{ return (period); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

	void Setup (void);

private:
	bool period_flag;

	int lvalue;

	int org, des, period, data;
};

#endif
