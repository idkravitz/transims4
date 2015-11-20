//*********************************************************
//	Index_Array.hpp - index array of data elements
//*********************************************************

#ifndef INDEX_ARRAY_HPP
#define INDEX_ARRAY_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Index_Array class definition
//---------------------------------------------------------

class Index_Array : public Data_Array
{
public:
	Index_Array (int data_size = 0, int max_records = 0, bool sorted = false);
	virtual ~Index_Array (void)         { Index_Array::Clear (); }

	bool Initialize (int data_size, int max_records = 0, bool sorted = false);

	virtual bool Add (void *data = NULL);

	virtual int  Max_Records (void)     { return (Data_Array::Max_Records ()); }	
	virtual bool Max_Records (int max_records);
	virtual void Clear (void);
	
	int Get_Index (void *data);

	void * Get (void *data);
	void * Get_GE (void *data);
	void * Get_LE (void *data);

	void * First_Key (void);
	void * Next_Key (void);

	void * Last_Key (void);
	void * Previous_Key (void);

	bool Sorted (bool sorted = true);
	bool Optimize (bool compress = true);

protected:
	virtual int  Sort_Function (void *data1, void *data2); 

	virtual bool Split_Function (void *data1, void *data2);

	int Key_Precedent (int key1, int key2);

private:
	typedef struct {
		int  higher;
		int  lower;
	} Index_Element;

	bool sorted;
	int root;

	int *index;

	int Cut_Lower (int current_record, void *key);
	int Cut_Higher (int current_record, void *key);
};
#endif
