//******************************************************** 
//	Node_File.hpp - Node File Input/Output
//********************************************************

#ifndef NODE_FILE_HPP
#define NODE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Node_File Class definition
//---------------------------------------------------------

class  Node_File : public Db_Header
{
public:

	Node_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Node_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Node_File (void);

	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	double X (void)					{ Get_Field (x, &dvalue); return (dvalue); }
	double Y (void)					{ Get_Field (y, &dvalue); return (dvalue); }
	double Z (void)					{ Get_Field (z, &dvalue); return (dvalue); }

	void Node (int value)			{ Put_Field (node, value); }
	void X (double value)			{ Put_Field (x, value); }
	void Y (double value)			{ Put_Field (y, value); }
	void Z (double value)			{ Put_Field (z, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int node, x, y, z;
};

#endif
