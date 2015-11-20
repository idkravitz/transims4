//*********************************************************
//	Time_Range.cpp - data range functions
//*********************************************************

#include "Time_Range.hpp"

#include "Utility.hpp"

char Time_Range::buffer [FIELD_BUFFER] = "";

//---------------------------------------------------------
//	Time_Range constructor
//---------------------------------------------------------

Time_Range::Time_Range (int steps, char *format) : Range_Array (10), Time_Step (steps, format)
{
	Increment (1);
	Period_Flag (false);
}

//---------------------------------------------------------
//	Time_Range constructor
//---------------------------------------------------------

Time_Range::Time_Range (int steps, Time_Type format) : Range_Array (10), Time_Step (steps, format)
{
	Increment (1);
	Period_Flag (false);
}

//---------------------------------------------------------
//	Add_Label
//---------------------------------------------------------

bool Time_Range::Add_Label (char *label)
{
	int low, high, periods;
	char *ptr;

	periods = (period_flag) ? increment : 0;

	//---- check for special conditions ----

	if (label == NULL || *label == '\0' || *label == '\n') return (true);

	//---- unpack the label string ----	

	Get_Token (label, buffer, sizeof (buffer));

	if (buffer [0] == '\0') return (false);

	ptr = strchr (buffer, '_');

	if (ptr == NULL) {
		ptr = strstr (buffer, "..");
		if (ptr == NULL) goto label_error;
		*ptr++ = '\0';
	}
	*ptr++ = '\0';

	low = Time_Label (buffer);

	high = Time_Label (ptr);

	return (Add_Range (low, high-1, periods));

label_error:
	if (Send_Messages ()) {
		exe->Error ("Label Value is Missing from Time Label %s", buffer);
	}
	return (false);
}

//---------------------------------------------------------
//	Add_Ranges
//---------------------------------------------------------

bool Time_Range::Add_Ranges (char *ranges)
{
	int low, high, periods;
	char *ptr;

	periods = (period_flag) ? increment : 0;

	//---- check for special conditions ----

	if (ranges == NULL || *ranges == '\0' || *ranges == '\n') return (true);
	
	if (str_cmp (ranges, "All") == 0) {
		high = (period_flag) ? Step (MIDNIGHT) : MAX_INTEGER;

		return (Add_Range (0, high-1, periods));
	}

	//---- unpack the range string ----	

	while (ranges != NULL && *ranges != '\0' && *ranges != '\n') {

		ranges = Get_Token (ranges, buffer, sizeof (buffer));

		if (buffer [0] == '\0') break;

		ptr = strchr (buffer, '.');

		if (ptr == NULL) goto range_error;

		if (*(ptr+1) == '.') {
			*ptr++ = '\0';
		} else {
			ptr = strchr (ptr+1, '.');

			if (ptr == NULL || *(ptr+1) != '.') goto range_error;

			*ptr++ = '\0';
		}
		low = Step (buffer);

		while (*ptr == '.') ptr++;

		high = Step (ptr);

		if (!Add_Range (low, high-1, periods)) return (false);
	}
	return (Num_Ranges () > 0);

range_error:
	if (Send_Messages ()) {
		exe->Error ("Range Value (..) is Missing from Time Range %s", buffer);
	}
	return (false);
}

//---------------------------------------------------------
//	Add_Breaks
//---------------------------------------------------------

bool Time_Range::Add_Breaks (char *breaks)
{
	int low, high;
	char *ptr;

	//---- check for special conditions ----

	if (breaks == NULL || *breaks == '\0' || *breaks == '\n') return (true);

	if (str_cmp (breaks, "None") == 0) {
		return (Add_Range (0, Step (MIDNIGHT)-1));
	}

	//---- unpack the break string ----	

	low = 0;

	while (breaks != NULL && *breaks != '\0' && *breaks != '\n') {

		breaks = Get_Token (breaks, buffer, sizeof (buffer));

		if (buffer [0] == '\0') break;

		ptr = strchr (buffer, '.');
		if (ptr != NULL && *(ptr+1) == '.') goto break_error;

		high = Step (buffer);

		if (low > high) goto break_error;

		if (low < high) {
			if (!Add_Range (low, high-1)) return (false);
		}
		low = high;
	}
	high = Step (MIDNIGHT);

	if (low < high) {
		return (Add_Range (low, high-1));
	}
	return (Num_Ranges () > 0);

break_error:
	if (Send_Messages ()) {
		exe->Error ("Range Breaks %s are Illogical", breaks);
	}
	return (false);
}

//---------------------------------------------------------
//	Range_Format
//---------------------------------------------------------

char * Time_Range::Range_Format (int num)
{
	Range_Data *range_ptr = (Range_Data *) Record (num);

	if (range_ptr == NULL) return (NULL);

	str_cpy (buffer, sizeof (buffer), Format_Step (range_ptr->Low ()));
	str_cat (buffer, sizeof (buffer), "..");
	str_cat (buffer, sizeof (buffer), Format_Step (range_ptr->High ()+1));

	return (buffer);
}

//---------------------------------------------------------
//	Range_Label
//---------------------------------------------------------

char * Time_Range::Range_Label (int num, bool pad_flag)
{
	char start [40], end [40];

	Range_Data *range_ptr = (Range_Data *) Record (num);

	if (range_ptr == NULL) return (NULL);

	str_cpy (start, sizeof (start), Time_Label (range_ptr->Low (), pad_flag));
	str_cpy (end, sizeof (end), Time_Label (range_ptr->High ()+1, pad_flag));

	//---- set time string ----

	str_fmt (buffer, sizeof (buffer), "%s_%s", start, end);
	return (buffer);
}
