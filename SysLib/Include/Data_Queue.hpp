//*********************************************************
//	Data_Queue.hpp - managed queue of data pointers
//*********************************************************

#ifndef DATA_QUEUE_HPP
#define DATA_QUEUE_HPP

//---------------------------------------------------------
//	Data_Queue -- class definition
//---------------------------------------------------------

class Data_Queue
{
public:
	Data_Queue (int max_records = 0);
	virtual ~Data_Queue (void)		{ Clear (); }

	bool Add (void *data);

	void * First (void);
	void * Next (void);

	virtual void Remove (bool delete_flag = true);

	bool Max_Records (int max_records);
	int  Max_Records (void)			{ return (max_records); }
	int  Num_Records (void)			{ return (num_records); }

	void Reset (void)				{ num_records = num_removed = 0; }
	void Clear (void);
	void Compress (void);

protected:

	void * Record (int index)		{ return ((index >= 0 && index < num_records) ? queue [index] : 0); }
	void * Record (void)			{ return (Record (record_index)); }

private:

	bool Add_Memory (void);

	int record_index;
	int num_removed;
	int num_records;
	int max_records;

	void **queue;
};
#endif
