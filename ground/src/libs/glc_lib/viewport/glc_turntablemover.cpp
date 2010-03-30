/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Copyright (C) 2009 Laurent Bauer
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

#include "glc_turntablemover.h"
#include "glc_viewport.h"

// Default constructor
GLC_TurnTableMover::GLC_TurnTableMover(GLC_Viewport* pViewport, const QList<GLC_RepMover*>& repsList)
: GLC_Mover(pViewport, repsList)
, m_Sign(1.0)
{

}


// Copy constructor
GLC_TurnTableMover::GLC_TurnTableMover(const GLC_TurnTableMover& mover)
: GLC_Mover(mover)
, m_Sign(mover.m_Sign)
{
}


GLC_TurnTableMover::~GLC_TurnTableMover()
{
}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Return a clone of the mover
GLC_Mover* GLC_TurnTableMover::clone() const
{
	return new GLC_TurnTableMover(*this);
}


//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Initialized the mover
void GLC_TurnTableMover::init(int x, int y)
{
	m_PreviousVector.setVect(static_cast<double>(x), static_cast<double>(y),0.0);
	GLC_Camera* pCamera= m_pViewport->cameraHandle();
	// Calculate angle sign
	m_Sign= pCamera->defaultUpVector() * pCamera->upVector();
	if (m_Sign == 0)
	{
		m_Sign= 1;
	}
	else
	{
		m_Sign= m_Sign / fabs(m_Sign);
	}

	pCamera->setUpCam(pCamera->defaultUpVector() * m_Sign);
}


void GLC_TurnTableMover::move(int x, int y)
{
	GLC_Camera* pCamera= m_pViewport->cameraHandle();
	// Turn table rotation
	const double rotSpeed= 2.3;
	const double width= static_cast<double> ( m_pViewport->viewVSize() );
	const double height= static_cast<double> ( m_pViewport->viewHSize() );

	const double alpha = -((static_cast<double>(x) - m_PreviousVector.X()) / width) * rotSpeed;
	const double beta = ((static_cast<double>(y) - m_PreviousVector.Y()) / height) * rotSpeed;

	// Rotation around the screen vertical axis
	pCamera->rotateAroundTarget(pCamera->defaultUpVector(), alpha * m_Sign);

	// Rotation around the screen horizontal axis
	GLC_Vector4d incidentVector= pCamera->camVector();
	GLC_Vector4d rightVector= incidentVector ^ pCamera->upVector();
	if (not rightVector.isNull())
	{
		pCamera->rotateAroundTarget(rightVector, beta);
	}

	m_PreviousVector.setVect(static_cast<double>(x), static_cast<double>(y), 0.0);

}
