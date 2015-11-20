//******************************************************** 
//	Link_Data_File.hpp - Link Data File Input/Output
//********************************************************

#ifndef LINK_DATA_FILE_HPP
#define LINK_DATA_FILE_HPP

#include "Db_Header.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Link_Data_File Class definition
//---------------------------------------------------------

class  Link_Data_File : public Db_Header, public Time_Range
{
public:

	Link_Data_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_Data_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Link_Data_File (void);

	int Link (void)							{ Get_Field (link, &lvalue); return (lvalue); }
	int Anode (void)						{ Get_Field (anode, &lvalue); return (lvalue); }
	int Bnode (void)						{ Get_Field (bnode, &lvalue); return (lvalue); }
	int Volume_AB (int period)				{ Get_Field (AB (period), &lvalue); return (lvalue); }
	int Volume_BA (int period)				{ Get_Field (BA (period), &lvalue); return (lvalue); }
	double Data_AB (int period)				{ Get_Field (AB (period), &dvalue); return (dvalue); }
	double Data_BA (int period)				{ Get_Field (BA (period), &dvalue); return (dvalue); }

	void Link (int value)					{ Put_Field (link, value); }
	void Anode (int value)					{ Put_Field (anode, value); }
	void Bnode (int value)					{ Put_Field (bnode, value); }
	void Volume_AB (int period, int value)	{ Put_Field (AB (period), value); }
	void Volume_BA (int period, int value)	{ Put_Field (BA (period), value); }
	void Data_AB (int period, double value)	{ Put_Field (AB (period), value); }
	void Data_BA (int period, double value)	{ Put_Field (BA (period), value); }

	void Num_Decimals (int number)			{ decimal = number; }

	virtual bool Create_Fields (void);
	bool Create_Fields (int low, int high, int increment = 0);
	
	void Zero_Fields (void);

	int  AB (int period)		{ return ((ab != NULL && Check_Index (period)) ? ab [period] : 0); }
	int  BA (int period)		{ return ((ba != NULL && Check_Index (period)) ? ba [period] : 0); }

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	void Clear_Fields (void);

	int lvalue, link, anode, bnode, decimal, *ab, *ba;
	double dvalue;
};

#endif
