/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Copyright (C) 2009 Laurent Bauer
 Version 2.0.0, packaged on July 2010.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/
//! \file glc_pointsprite.cpp implementation of the GLC_PointSprite class.

#include "glc_pointsprite.h"
#include "../glc_openglexception.h"
#include "../glc_state.h"
#include "../glc_ext.h"

// The maximum point size
float GLC_PointSprite::m_MaxSize= -1.0f;

// Default constructor
GLC_PointSprite::GLC_PointSprite(float size, GLC_Material* pMaterial)
:GLC_Geometry("PointSprite", false)
, m_Size(size)
, m_DistanceAttenuation(3)
, m_FadeThresoldSize(60.0f)
{
	Q_ASSERT(pMaterial != NULL);
	Q_ASSERT(pMaterial->hasTexture());
	addMaterial(pMaterial);

	// Set Distance attenuation defaults values
	m_DistanceAttenuation[0]= 1.0f;
	m_DistanceAttenuation[1]= 0.0f;
	m_DistanceAttenuation[2]= 0.0f;
}

GLC_PointSprite::~GLC_PointSprite()
{

}

// return the point bounding box
const GLC_BoundingBox& GLC_PointSprite::boundingBox(void)
{

	if (NULL == m_pBoundingBox)
	{
		m_pBoundingBox= new GLC_BoundingBox();
		const double epsilon= 1e-2;
		GLC_Point3d lower( 	- epsilon,
							- epsilon,
							- epsilon);
		GLC_Point3d upper(  epsilon,
							epsilon,
							epsilon);
		m_pBoundingBox->combine(lower);
		m_pBoundingBox->combine(upper);
	}
	return *m_pBoundingBox;
}

// Return a copy of the current geometry
GLC_Geometry* GLC_PointSprite::clone() const
{
	return new GLC_PointSprite(*this);
}

// Return the point size
void GLC_PointSprite::setSize(float size)
{
	m_GeometryIsValid= false;
	m_Size= size;
    // Clamp m_Size to m_MaxSize
    if(qFuzzyCompare(-1.0f, m_MaxSize) && (m_MaxSize < m_Size))
    {
    	m_Size= m_MaxSize;
    }
}

// Set the point distance attenuation values
void GLC_PointSprite::setPointDistanceAttenuation(QVector<float> parameters)
{
	Q_ASSERT(3 == parameters.size());
	m_DistanceAttenuation= parameters;
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
// Specific glExecute method
void GLC_PointSprite::render(const GLC_RenderProperties& renderProperties)
{
	// Check if extension GL_ARB_point_parameters is present
	if (!GLC_State::pointSpriteSupported()) return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (!GLC_State::isInSelectionMode())
	{
		glEnable( GL_BLEND );
		glDepthMask(GL_FALSE);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);

	    if(m_MaterialHash.size() == 1)
	    {
	    	GLC_Material* pCurrentMaterial= m_MaterialHash.begin().value();
	    	const GLfloat red= pCurrentMaterial->diffuseColor().redF();
	    	const GLfloat green= pCurrentMaterial->diffuseColor().greenF();
	    	const GLfloat blue= pCurrentMaterial->diffuseColor().blueF();
	    	const GLfloat alpha= pCurrentMaterial->diffuseColor().alphaF();

	    	glColor4f(red, green, blue, alpha);
	    	pCurrentMaterial->glExecute();
	    }
	}
	else
	{
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
	}


	// Executed only the first time
	if (qFuzzyCompare(-1.0f, m_MaxSize))
	{
		 // Query for the max point size supported by the hardware
		glGetFloatv(GL_POINT_SIZE_MAX, &m_MaxSize);

	    // Clamp m_Size to m_MaxSize
	    if(m_MaxSize < m_Size)
	    	m_Size= m_MaxSize;
	}

    // This is how will our point sprite's size will be modified by
    // distance from the viewer
    glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, m_DistanceAttenuation.data());
    glPointSize(m_Size);

    // The alpha of a point is calculated to allow the fading of points
    // instead of shrinking them past a defined threshold size. The threshold
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to
    // the minimum and maximum point sizes.
    glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, m_FadeThresoldSize);

    glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
    glPointParameterf(GL_POINT_SIZE_MAX, m_Size);

    // Specify point sprite texture coordinate replacement mode for each
    // texture unit
    glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    glEnable(GL_POINT_SPRITE);
    glDraw(renderProperties);

    glPopAttrib();

}
// Point sprite set up
void GLC_PointSprite::glDraw(const GLC_RenderProperties&)
{
	// Point Display
	glBegin(GL_POINTS);
		glVertex3f(0.0f,0.0f,0.0f);
	glEnd();

	// OpenGL error handler
	GLenum error= glGetError();
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_PointSprite::GlDraw ", error);
		throw(OpenGlException);
	}
}

