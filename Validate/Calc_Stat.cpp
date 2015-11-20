//*********************************************************
//	Calc_Stat.cpp - Calculate Performance Statistics
//*********************************************************

#include "Validate.hpp"

#include "math.h"

//---------------------------------------------------------
//	Calc_Stats
//---------------------------------------------------------

void Validate::Calc_Stats (Group_Data *data, Statistics *stats)
{
	long n;
	double num, diff, count, volume, count_volume, volume_sq, count_sq, error, error_sq;

	n = data->number;
	count = data->count;

	if (n == 0 || count == 0.0) return;

	num = (double) n;
	volume = data->volume;
	count_volume = data->count_volume;
	count_sq = data->count_sq;
	volume_sq = data->volume_sq;
	error = data->error;
	error_sq = data->error_sq;
		
	diff = volume - count;
	stats->per_diff = diff * 100.0 / count;
	
	stats->avg_error = error / num;
	stats->per_error = error * 100.0 / count;

	if (n > 1) {		
		error = (error_sq - error * error / num) / (num - 1.0);
		stats->std_dev = (error > 0.0) ? sqrt (error) : 0.0;
	} else {
		stats->std_dev = 0.0;
	}
	stats->rmse = 100.0 * sqrt (error_sq / num) * num / count;

	stats->r_sq = (volume_sq - volume * volume / num) * (count_sq - count * count / num);

	if (stats->r_sq != 0.0) {
		stats->r_sq = (count_volume - count * volume / num) / sqrt (stats->r_sq);
		stats->r_sq *= stats->r_sq;
	} else {
		stats->r_sq = 0.0;
	}

	//stats->correl = (num * volume_sq - volume * volume) * (num * count_sq - count * count);

	//if (stats->correl != 0.0) {
	//	stats->correl = (num * count_volume - count * volume) / sqrt (stats->correl);
	//} else {
	//	stats->correl = 0.0;
	//}

	if (data->capacity > 0.0) {
		stats->avg_vc = volume / data->capacity;
	} else {
		stats->avg_vc = 0.0;
	}

	//---- adjust to output limits ----

	if (stats->per_diff > 99999.0) stats->per_diff = 99999.9; 
	if (stats->per_diff < -9999.0) stats->per_diff = -9999.9;

	if (stats->avg_error > 9999999.0) stats->avg_error = 9999999.0;
	if (stats->per_error > 99999.0) stats->per_error = 99999.9;

	if (stats->std_dev > 9999999.0) stats->std_dev = 9999999.0;
	if (stats->rmse > 99999.0) stats->rmse = 99999.9;

	if (stats->r_sq > 1.0) stats->r_sq = 1.0;
	if (stats->r_sq < 0.0) stats->r_sq = 0.0;

	//if (stats->correl > 1.0) stats->correl = 1.0;
	//if (stats->correl < -1.0) stats->correl = -1.0;

	if (stats->avg_vc > 99.99) stats->avg_vc = 99.99;
}
