//******************************************************** 
//	Phasing_File.hpp - Phasing Plan File Input/Output
//********************************************************

#ifndef PHASING_FILE_HPP
#define PHASING_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Phasing_File Class definition
//---------------------------------------------------------

class  Phasing_File : public Db_Header
{
public:

	Phasing_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Phasing_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Phasing_File (void);

	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int Timing (void)				{ Get_Field (timing, &lvalue); return (lvalue); }
	int Phase (void)				{ Get_Field (phase, &lvalue); return (lvalue); }
	int In_Link (void)				{ Get_Field (in_link, &lvalue); return (lvalue); }
	int Out_Link (void)				{ Get_Field (out_link, &lvalue); return (lvalue); }
	char *Protection (void)			{ Get_Field (protect, svalue); return (svalue); }
	char *Detectors (void)			{ Get_Field (detectors, svalue); return (svalue); }

	void Node (int value)			{ Put_Field (node, value); }
	void Timing (int value)			{ Put_Field (timing, value); }
	void Phase (int value)			{ Put_Field (phase, value); }
	void In_Link (int value)		{ Put_Field (in_link, value); }
	void Out_Link (int value)		{ Put_Field (out_link, value); }
	void Protection (char *value)	{ Put_Field (protect, value); }
	void Detectors (char *value)	{ Put_Field (detectors, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int node, timing, phase, in_link, out_link, protect, detectors;
};

#endif
