//*********************************************************
//	Complex_Array.hpp - data array with complex index keys
//*********************************************************

#ifndef COMPLEX_ARRAY_HPP
#define COMPLEX_ARRAY_HPP

#include "Index_Array.hpp"

//---------------------------------------------------------
//	Complex_Array class definition
//---------------------------------------------------------

class Complex_Array : public Index_Array
{
public:
	Complex_Array (int data_size = 0, int num_keys = 1, bool pointer_flag = false, int max_records = 0, bool sorted = false);
	virtual ~Complex_Array (void);

	bool Initialize (int data_size, int num_keys = 1, bool pointer_flag = false, int max_records = 0, bool sorted = false);
	
	virtual int Sort_Function (void *data1, void *data2); 
	virtual bool Split_Function (void *data1, void *data2);

	void Delete_Pointers (void);
	virtual void Clear (void);
	virtual void Reset (void);

	int  Num_Keys (void)			{ return (num_keys); }
	bool Pointer_Flag (void)		{ return (pointer_flag); }
    
private:
	bool pointer_flag;
	int num_keys;
};
#endif
