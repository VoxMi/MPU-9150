/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */
/**
 *  @defgroup ML
 *  @brief  Motion Library APIs.
 *          The Motion Library processes gyroscopes, accelerometers, and
 *          compasses to provide a physical model of the movement for the
 *          sensors.
 *          The results of this processing may be used to control objects
 *          within a user interface environment, detect gestures, track 3D
 *          movement for gaming applications, and analyze the blur created
 *          due to hand movement while taking a picture.
 *
 *  @{
 *      @file   ml.c
 *      @brief  The Motion Library APIs.
 */

#include "ml.h"

/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
inline uint16_t inv_row_2_scale(const int8_t *row)
{
	uint16_t b;

	if (row[0] > 0)
		b = 0;
	else if (row[0] < 0)
		b = 4;
	else if (row[1] > 0)
		b = 1;
	else if (row[1] < 0)
		b = 5;
	else if (row[2] > 0)
		b = 2;
	else if (row[2] < 0)
		b = 6;
	else
		b = 7;      // error
	return b;
}

inline uint16_t inv_orientation_matrix_to_scalar(const int8_t *mtx)
{
	uint16_t scalar;

	/*
	   XYZ  010_001_000 Identity Matrix
	   XZY  001_010_000
	   YXZ  010_000_001
	   YZX  000_010_001
	   ZXY  001_000_010
	   ZYX  000_001_010
	 */

	scalar = inv_row_2_scale(mtx);
	scalar |= inv_row_2_scale(mtx + 3) << 3;
	scalar |= inv_row_2_scale(mtx + 6) << 6;

	return scalar;
}
