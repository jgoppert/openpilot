/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_interpolator.h interface for the GLC_Interpolator class.

#ifndef GLC_INTERPOLATOR_H_
#define GLC_INTERPOLATOR_H_

#include "glc_vector4d.h"
#include "glc_matrix4x4.h"

// Types d'interpolation
enum INTERPOL_TYPE
{
	INTERPOL_LINEAIRE,
	INTERPOL_ANGULAIRE,
	INTERPOL_HOMOTETIE
};

//////////////////////////////////////////////////////////////////////
// La CLASSE
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//! \class GLC_Interpolator
/*! \brief GLC_Interpolator : Matrix interpolation class*/

/*! An GLC_Interpolator is a class used to interpolate 2 4D matrix*/
//////////////////////////////////////////////////////////////////////
class GLC_Interpolator
{

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
public:
	// Contructeur par d�faut Interpolation Lin�aire
	GLC_Interpolator();

//////////////////////////////////////////////////////////////////////
// Fonctions Set
//////////////////////////////////////////////////////////////////////
public:
	// D�fini la matrice d'interpolation
	void SetInterpolMat(int NbrPas, const GLC_Vector4d &VectDepart, const GLC_Vector4d &VectArrive
		, INTERPOL_TYPE Interpolation = INTERPOL_LINEAIRE);
	// Type d'interpolation
	void SetType(INTERPOL_TYPE Interpolation);
	// Nombre de pas
	void SetNbrPas(int NbrPas);
	// Vecteur d'arriv�e et de depart
	void SetVecteurs(const GLC_Vector4d &VectDepart, const GLC_Vector4d &VectArrive);

//////////////////////////////////////////////////////////////////////
// Fonctions Get
//////////////////////////////////////////////////////////////////////
public:
	// Retourne la matrice d'interpolation
	GLC_Matrix4x4 GetInterpolMat(void) const
	{
		return m_InterpolMat;
	}

//////////////////////////////////////////////////////////////////////
// Fonctions de Service priv�e
//////////////////////////////////////////////////////////////////////
private:
	// Calcul La matrice d'interpolation
	bool CalcInterpolMat(void);

	// Calcul la matrice d'interpolation lin�aire
	bool CalcInterpolLineaireMat(void);

	// Calcul la matrice d'interpolation angulaire
	bool CalcInterpolAngulaireMat(void);

//////////////////////////////////////////////////////////////////////
// Membres priv�s
//////////////////////////////////////////////////////////////////////
private:
	// Vecteur de d�part
	GLC_Vector4d m_VectDepart;
	// Vecteur d'arriver
	GLC_Vector4d m_VectArrive;

	// Type d'interpolation courante
	INTERPOL_TYPE m_InterpolType;

	// Nombre de pas d'interpolation
	int m_nNbrPas;

	// Matrice d'interpolation
	GLC_Matrix4x4 m_InterpolMat;
};

#endif /*GLC_INTERPOLATOR_H_*/
