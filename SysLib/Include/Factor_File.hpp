//******************************************************** 
//	Factor_File.hpp - Factor File Input/Output
//********************************************************

#ifndef FACTOR_FILE_HPP
#define FACTOR_FILE_HPP

#include "Matrix_File.hpp"

//---------------------------------------------------------
//	Factor_File Class definition
//---------------------------------------------------------

class  Factor_File : public Matrix_File
{
public:

	Factor_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Factor_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Factor_File (void);

	double Factor (void)              { Get_Field (factor, &dvalue); return (dvalue); }

	void   Factor (double value)      { Put_Field (factor, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void)		{ return (false); }

	void Setup (void);

private:
	double dvalue;

	int factor;
};

#endif
