/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
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
//! \file glc_wiredata.h Interface for the GLC_WireData class.

#ifndef GLC_WIREDATA_H_
#define GLC_WIREDATA_H_

#include <QColor>
#include "../glc_global.h"
#include "../glc_boundingbox.h"
#include "../shading/glc_renderproperties.h"

#include "../glc_config.h"
//////////////////////////////////////////////////////////////////////
//! \class GLC_WireData
/*! \brief GLC_WireData : Contains geometries's wire data
 */
//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_WireData
{
	friend QDataStream &operator<<(QDataStream &, const GLC_WireData &);
	friend QDataStream &operator>>(QDataStream &, GLC_WireData &);

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Construct a empty wire data
	GLC_WireData();

	//! Construct wire data from the given wire data
	GLC_WireData(const GLC_WireData&);

	//! Copy the given wire data in this wire data
	GLC_WireData& operator=(const GLC_WireData&);

	//! Destructor
	virtual ~GLC_WireData();
//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return this wire data class Chunk ID
	static quint32 chunckID();

	//! Return this wire data Position Vector
	GLfloatVector positionVector() const;

	//! Return true if this wire data is empty
	inline bool isEmpty() const
	{return ((m_PositionSize == 0) && m_Positions.isEmpty());}

	//! Return this wire data bounding box
	GLC_BoundingBox& boundingBox();

	//! Return the number of polylines
	inline int polylineCount() const
	{return m_PolylinesCount;}

	//! Return the polyline offset from the given index
	inline GLuint polylineOffset(int index) const
	{return m_PolylinesOffset.at(index);}

	//! Return the polyline size from the given index
	inline GLsizei polylineSize(int index) const
	{return m_PolylinesSizes.at(index);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Add a Polyline to this wire and returns its id if id are managed
	GLC_uint addPolyline(const GLfloatVector&);

	//! Clear the content of this wire Data and makes it empty
	void clear();

	//! Copy VBO to the Client Side
	void copyVboToClientSide();

	//! Release client VBO
	void releaseVboClientSide(bool update= false);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Make this wire data a VBO
	void finishVbo();

	//! Set vbo usage of this wire data
	void useVBO(bool usage);

	//! Render this wire data using Opengl
	void glDraw(const GLC_RenderProperties&);

private:
	//! Create this wire data VBO id
	void createVBOs();

	//! Fill this wire data VBO from memmory
	void fillVBOs();
//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! VBO ID
	GLuint m_VboId;

	//! The next primitive local id
	GLC_uint m_NextPrimitiveLocalId;

	//! Vertex Position Vector
	GLfloatVector m_Positions;

	//! The size of the VBO
	int m_PositionSize;

	//! Wire data bounding box
	GLC_BoundingBox* m_pBoundingBox;

	//! Polylines size
	IndexSizes m_PolylinesSizes;

	//! Vector of polyline group offset
	OffsetVectori m_PolylinesOffset;

	//! Polyline groups id
	QList<GLC_uint> m_PolylinesId;

	//! The number of polylines
	int m_PolylinesCount;

	//! Class chunk id
	static quint32 m_ChunkId;
};

//! Non-member stream operator
QDataStream &operator<<(QDataStream &, const GLC_WireData &);
QDataStream &operator>>(QDataStream &, GLC_WireData &);

#endif /* GLC_WIREDATA_H_ */
