//******************************************************** 
//	Veh_Type_File.hpp - Vehicle Type File Input/Output
//********************************************************

#ifndef VEH_TYPE_FILE_HPP
#define VEH_TYPE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Veh_Type_File Class definition
//---------------------------------------------------------

class  Veh_Type_File : public Db_Header
{
public:

	Veh_Type_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Veh_Type_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Veh_Type_File (void);

	int Type (void)					{ Get_Field (type, &lvalue); return (lvalue); }
	int Sub_Type (void)				{ Get_Field (sub_type, &lvalue); return (lvalue); }
	double Length (void)			{ Get_Field (length, &dvalue); return (dvalue); }
	double Max_Speed (void)			{ Get_Field (max_speed, &dvalue); return (dvalue); }
	double Max_Accel (void)			{ Get_Field (max_accel, &dvalue); return (dvalue); }
	double Max_Decel (void)			{ Get_Field (max_decel, &dvalue); return (dvalue); }
	char * Use (void)				{ Get_Field (use, svalue); return (svalue); }
	int Capacity (void)				{ Get_Field (capacity, &lvalue); return (lvalue); }
	double Loading (void)			{ Get_Field (load, &dvalue); return (dvalue); }
	double Unloading (void)			{ Get_Field (unload, &dvalue); return (dvalue); }
	char * Method (void)			{ Get_Field (method, svalue); return (svalue); }
	int Min_Dwell (void)			{ Get_Field (min_dwell, &lvalue); return (lvalue); }
	int Max_Dwell (void)			{ Get_Field (max_dwell, &lvalue); return (lvalue); }

	void Type (int value)			{ Put_Field (type, value); }
	void Sub_Type (int value)		{ Put_Field (sub_type, value); }
	void Length (double value)		{ Put_Field (length, value); }
	void Max_Speed (double value)	{ Put_Field (max_speed, value); }
	void Max_Accel (double value)	{ Put_Field (max_accel, value); }
	void Max_Decel (double value)	{ Put_Field (max_decel, value); }
	void Use (char *value)			{ Put_Field (use, value); }
	void Capacity (int value)		{ Put_Field (capacity, value); }
	void Loading (double value)		{ Put_Field (load, value); }
	void Unloading (double value)	{ Put_Field (unload, value); }
	void Method (char *value)		{ Put_Field (method, value); }
	void Min_Dwell (int value)		{ Put_Field (min_dwell, value); }
	void Max_Dwell (int value)		{ Put_Field (max_dwell, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int type, sub_type, length, max_speed, max_accel, max_decel, use, capacity;
	int load, unload, method, min_dwell, max_dwell;
};

#endif
