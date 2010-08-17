/**
 ******************************************************************************
 *
 * @file       CoordinateConverions.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Header for Coordinate conversions library in CoordinateConversions.c
 *             - all angles in deg
 *             - distances in meters
 *             - altitude above WGS-84 elipsoid
 *
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef COORDINATECONVERSIONS_H_
#define COORDINATECONVERSIONS_H_

    // ****** convert Lat,Lon,Alt to ECEF  ************
	void LLA2ECEF(double LLA[3], double ECEF[3]);

    // ****** convert ECEF to Lat,Lon,Alt (ITERATIVE!) *********
    uint16_t ECEF2LLA(double ECEF[3], double LLA[3]);

	void RneFromLLA(double LLA[3], float Rne[3][3]);
	
	// ****** find roll, pitch, yaw from quaternion ********
	void Quaternion2RPY(float q[4], float rpy[3]);
	
	// ****** find quaternion from roll, pitch, yaw ********
	void RPY2Quaternion(float rpy[3], float q[4]);
	
	//** Find Rbe, that rotates a vector from earth fixed to body frame, from quaternion **
	void Quaternion2R(float q[4], float Rbe[3][3]);

	// ****** Express LLA in a local NED Base Frame ********
	void LLA2Base(double LLA[3], double BaseECEF[3], float Rne[3][3], float NED[3]);
	
	// ****** Express ECEF in a local NED Base Frame ********
	void ECEF2Base(double ECEF[3], double BaseECEF[3], float Rne[3][3], float NED[3]);

#endif  // COORDINATECONVERSIONS_H_
