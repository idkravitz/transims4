//******************************************************** 
//	Detector_File.hpp - Detector File Input/Output
//********************************************************

#ifndef DETECTOR_FILE_HPP
#define DETECTOR_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Detector_File Class definition
//---------------------------------------------------------

class  Detector_File : public Db_Header
{
public:

	Detector_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Detector_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Detector_File (void);

	int ID (void)					{ Get_Field (id, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	double Length (void)			{ Get_Field (length, &dvalue); return (dvalue); }
	int Low_Lane (void)				{ Get_Field (low, &lvalue); return (lvalue); }
	int High_Lane (void)			{ Get_Field (high, &lvalue); return (lvalue); }
	char *Type (void)				{ Get_Field (type, svalue); return (svalue); }
	int Coordinator (void)			{ Get_Field (coord, &lvalue); return (lvalue); }

	void ID (int value)				{ Put_Field (id, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Length (double value)		{ Put_Field (length, value); }
	void Low_Lane (int value)		{ Put_Field (low, value); }
	void High_Lane (int value)		{ Put_Field (high, value); }
	void Type (char *value)			{ Put_Field (type, value); }
	void Coordinator (int value)	{ Put_Field (coord, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int id, node, link, offset, length, low, high, type, coord;
};

#endif
