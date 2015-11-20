//******************************************************** 
//	Link_Dir_File.hpp - Link Direction File Input/Output
//********************************************************

#ifndef LINK_DIR_FILE_HPP
#define LINK_DIR_FILE_HPP

#include "Db_Header.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Link_Direction_File Class definition
//---------------------------------------------------------

class  Link_Direction_File : public Db_Header, public Time_Range
{
public:

	Link_Direction_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_Direction_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Link_Direction_File (void);

	int Link_Dir (void)						{ Get_Field (link_dir, &lvalue); return (lvalue); }
	int Link (void)							{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)							{ Get_Field (dir, &lvalue); return (lvalue); }
	double Data (int period)				{ Get_Field (Index (period), &dvalue); return (dvalue); }
	double Data2 (int period)				{ Get_Field (Index2 (period), &dvalue); return (dvalue); }
	double Diff (int period)				{ Get_Field (Index3 (period), &dvalue); return (dvalue); }

	void Link_Dir (int value)               { Put_Field (link_dir, value); }
	void Link (int value)					{ Put_Field (link, value); }
	void Dir (int value)					{ Put_Field (dir, value); }
	void Data (int period, double value)	{ Put_Field (Index (period), value); }
	void Data2 (int period, double value)	{ Put_Field (Index2 (period), value); }
	void Diff (int period, double value)	{ Put_Field (Index3 (period), value); }

	void Num_Decimals (int number)			{ decimal = number; }

	bool Direction_Index (void)             { return (index_flag); }
	void Direction_Index (bool flag)        { index_flag = flag; }

	bool Ignore_Periods (void)				{ return (ignore_flag); }
	void Ignore_Periods (bool flag)			{ ignore_flag = flag; }

	bool Join_Flag (void)					{ return (join_flag); }
	void Join_Flag (bool flag)				{ join_flag = flag; }

	bool Difference_Flag (void)				{ return (diff_flag); }
	void Difference_Flag (bool flag)		{ diff_flag = flag; }

	virtual bool Create_Fields (void);
	bool Create_Fields (int low, int high, int increment = 0);

	void Zero_Fields (void);
	void Difference (void);

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	void Clear_Fields (void);

	int  Index (int period)		{ return ((data != NULL && Check_Index (period)) ? data [period] : 0); }
	int  Index2 (int period)	{ return ((data2 != NULL && Check_Index (period)) ? data2 [period] : 0); }
	int  Index3 (int period)	{ return ((diff != NULL && Check_Index (period)) ? diff [period] : 0); }

	int lvalue, link_dir, link, dir, decimal, *data, *data2, *diff;
	double dvalue;
	bool ignore_flag, join_flag, diff_flag, index_flag;
};

#endif
