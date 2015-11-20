//*********************************************************
//	Integer_Queue.hpp - managed queue of integers
//*********************************************************

#ifndef INTEGER_QUEUE_HPP
#define INTEGER_QUEUE_HPP

//---------------------------------------------------------
//	Integer_Queue -- class definition
//---------------------------------------------------------

class Integer_Queue
{
public:
	Integer_Queue (int max_records = 0);
	virtual ~Integer_Queue (void)       { Clear (); }

	bool Add (int data);

	int First (void);
	int Next (void);

	void Remove (void);

	bool Max_Records (int max_records);
	int  Max_Records (void)				{ return (max_records); }
	int  Num_Records (void)				{ return (num_records); }

	void Clear (void);
	void Compress (void);

private:

	bool Add_Memory (void);

	int record_index;
	int num_removed;
	int num_records;
	int max_records;

	int *queue;
};
#endif
