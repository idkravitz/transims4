//******************************************************** 
//	HHRec_File.hpp - household record file input/output
//********************************************************

#ifndef HHREC_FILE_HPP
#define HHREC_FILE_HPP

#include "Ext_Header.hpp"

//---------------------------------------------------------
//	HHRec_File Class definition
//---------------------------------------------------------

class  HHRec_File : public Ext_Header
{
public:

	HHRec_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	HHRec_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~HHRec_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)				{ Get_Field (person, &lvalue); return (lvalue); }
	int Record (void)				{ Get_Field (record, &lvalue); return (lvalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Person (int value)			{ Put_Field (person, value); }
	void Record (int value)			{ Put_Field (record, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int hhold, person, record;
};

#endif
