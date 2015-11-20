//******************************************************** 
//	Connect_File.hpp - Lane Connectivity Input/Output
//********************************************************

#ifndef CONNECT_FILE_HPP
#define CONNECT_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Connect_File Class definition
//---------------------------------------------------------

class  Connect_File : public Db_Header
{
public:

	Connect_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Connect_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Connect_File (void);

	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int In_Link (void)				{ Get_Field (in_link, &lvalue); return (lvalue); }
	int Out_Link (void)				{ Get_Field (out_link, &lvalue); return (lvalue); }
	int In_Lane (void)				{ Get_Field (in_low, &lvalue); return (lvalue); }
	int In_Low (void)				{ Get_Field (in_low, &lvalue); return (lvalue); }
	int In_High (void)				{ Get_Field (in_high, &lvalue); return (lvalue); }
	int Out_Lane (void)				{ Get_Field (out_low, &lvalue); return (lvalue); }
	int Out_Low (void)				{ Get_Field (out_low, &lvalue); return (lvalue); }
	int Out_High (void)				{ Get_Field (out_high, &lvalue); return (lvalue); }
	char * Type (void)              { Get_Field (type, svalue); return (svalue); }
	int Penalty (void)				{ Get_Field (penalty, &lvalue); return (lvalue); }
	int Speed (void)				{ Get_Field (speed, &lvalue); return (lvalue); }
	int Capacity (void)				{ Get_Field (capacity, &lvalue); return (lvalue); }

	void Node (int value)			{ Put_Field (node, value); }
	void In_Link (int value)		{ Put_Field (in_link, value); }
	void Out_Link (int value)		{ Put_Field (out_link, value); }
	void In_Lane (int value)		{ Put_Field (in_low, value); }
	void In_Low (int value)			{ Put_Field (in_low, value); }
	void In_High (int value)		{ Put_Field (in_high, value); }
	void Out_Lane (int value)		{ Put_Field (out_low, value); }
	void Out_Low (int value)		{ Put_Field (out_low, value); }
	void Out_High (int value)		{ Put_Field (out_high, value); }
	void Type (char *value)         { Put_Field (type, value); }
	void Penalty (int value)		{ Put_Field (penalty, value); }
	void Speed (int value)			{ Put_Field (speed, value); }
	void Capacity (int value)		{ Put_Field (capacity, value); }
	
	bool Type_Flag (void)			{ return (type != 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int node, in_link, out_link, in_low, in_high, out_low, out_high, type, penalty, speed, capacity;
};

#endif
