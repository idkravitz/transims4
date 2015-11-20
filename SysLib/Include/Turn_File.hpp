//******************************************************** 
//	Turn_File.hpp - Turn Restriction File Input/Output
//********************************************************

#ifndef TURN_FILE_HPP
#define TURN_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Turn_File Class definition
//---------------------------------------------------------

class  Turn_File : public Db_Header
{
public:

	Turn_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Turn_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Turn_File (void);

	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int In_Link (void)				{ Get_Field (in_link, &lvalue); return (lvalue); }
	int Out_Link (void)				{ Get_Field (out_link, &lvalue); return (lvalue); }
	char * Start (void)				{ Get_Field (start, svalue); return (svalue); }
	char * End (void)				{ Get_Field (end, svalue); return (svalue); }
	char * Use (void)				{ Get_Field (use, svalue); return (svalue); }
	int Penalty (void)				{ Get_Field (penalty, &lvalue); return (lvalue); }

	void Node (int value)			{ Put_Field (node, value); }
	void In_Link (int value)		{ Put_Field (in_link, value); }
	void Out_Link (int value)		{ Put_Field (out_link, value); }
	void Start (char * value)		{ Put_Field (start, value); }
	void End (char * value)			{ Put_Field (end, value); }
	void Use (char * value)			{ Put_Field (use, value); }
	void Penalty (int value)		{ Put_Field (penalty, value); }

	bool Node_Based (void)			{ return (!in_link && !out_link && in_node && out_node); }

	int In_Node (void)				{ Get_Field (in_node, &lvalue); return (lvalue); }
	int Out_Node (void)				{ Get_Field (out_node, &lvalue); return (lvalue); }

	void In_Node (int value)		{ Put_Field (in_node, value); }
	void Out_Node (int value)		{ Put_Field (out_node, value); }
	
	bool Use_Flag (void)			{ return (use != 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int node, in_link, out_link, start, end, use, penalty, in_node, out_node;
};

#endif
