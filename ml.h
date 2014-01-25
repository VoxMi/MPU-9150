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
 *      @file   ml.h
 *      @brief  Header file for the Motion Library.
 */

#ifndef _ML_H_
#define _ML_H_

#include <avr/io.h>

extern uint16_t inv_orientation_matrix_to_scalar(const int8_t *mtx);

#endif /* _ML_H_ */
