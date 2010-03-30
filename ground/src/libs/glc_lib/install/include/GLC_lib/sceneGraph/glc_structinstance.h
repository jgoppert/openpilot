
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

//! \file glc_structinstance.h interface for the GLC_StructInstance class.

#ifndef GLC_STRUCTINSTANCE_H_
#define GLC_STRUCTINSTANCE_H_

#include <QList>
#include <QString>
#include "../maths/glc_matrix4x4.h"
#include "glc_3dviewinstance.h"
#include "glc_attributes.h"

class GLC_StructReference;
class GLC_StructOccurence;

//////////////////////////////////////////////////////////////////////
//! \class GLC_StructInstance
/*! \brief GLC_StructInstance : A scene graph instance node */
//////////////////////////////////////////////////////////////////////
class GLC_StructInstance
{
public:
	//! Default constructor
	GLC_StructInstance(GLC_StructReference*);

	//! Create instance with a rep
	GLC_StructInstance(GLC_Rep*);

	//! Copy constructor
	GLC_StructInstance(const GLC_StructInstance&);

	//! Copy constructor
	GLC_StructInstance(GLC_StructInstance*);

	//! Create empty instance
	GLC_StructInstance(const QString&);

	//! Set the reference of an empty instance
	void setReference(GLC_StructReference*);

	// Destructor
	virtual ~GLC_StructInstance();
//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return true if this instance have occurence
	inline bool hasStructOccurence() const
	{ return not m_ListOfOccurences.isEmpty();}

	//! Return the number of occurence
	inline int numberOfOccurence() const
	{return m_ListOfOccurences.size();}

	//! Return first occurence handle
	inline GLC_StructOccurence* firstOccurenceHandle() const
	{ return m_ListOfOccurences.first();}

	//! Return the relative matrix of this instance
	inline GLC_Matrix4x4 relativeMatrix() const
	{ return m_RelativeMatrix;}

	//! Return the reference of this instance
	inline GLC_StructReference* structReference() const
	{ return m_pStructReference;}

	//! Return the list off occurence of this instance
	inline QList<GLC_StructOccurence*> listOfStructOccurences() const
	{ return m_ListOfOccurences;}

	//! Return the instance name
	inline QString name() const
	{return m_Name;}

	//! Return true if the reference contains User attributes
	inline bool containsAttributes() const
	{ return ((NULL != m_pAttributes) and not m_pAttributes->isEmpty());}

	//! Return handle to the reference attributes
	inline GLC_Attributes* attributesHandle() const
	{return m_pAttributes;}

//@}
//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! An occurence of this instance have been created
	inline void structOccurenceCreated(GLC_StructOccurence* pOccurence)
	{
		Q_ASSERT(not m_ListOfOccurences.contains(pOccurence));
		m_ListOfOccurences.append(pOccurence);
	}

	inline void structOccurenceDeleted(GLC_StructOccurence *pOccurence)
	{m_ListOfOccurences.removeOne(pOccurence);}

	//! Move the instance by specified matrix
	inline GLC_StructInstance* move(const GLC_Matrix4x4& matrix)
	{
		m_RelativeMatrix= matrix * m_RelativeMatrix;
		return this;
	}

	//! Translate Instance
	inline GLC_StructInstance* translate(double Tx, double Ty, double Tz)
	{
		m_RelativeMatrix= GLC_Matrix4x4(Tx, Ty, Tz) * m_RelativeMatrix;
		return this;
	}

	//! Translate Instance
	inline GLC_StructInstance* translate(const GLC_Vector4d& v)
	{
		return translate(v.X(), v.Y(), v.Z());
	}

	//! Replace the instance Matrix
	inline GLC_StructInstance* setMatrix(const GLC_Matrix4x4 &SetMat)
	{
		m_RelativeMatrix= SetMat;
		return this;
	}

	//! Reset the instance Matrix
	inline GLC_StructInstance* resetMatrix()
	{
		m_RelativeMatrix = GLC_Matrix4x4();
		return this;
	}

	//! Set the instance name
	inline void setName(const QString& name)
	{m_Name= name;}

	//! Set the instance attributes
	void setAttributes(const GLC_Attributes& attr)
	{
		delete m_pAttributes;
		m_pAttributes= new GLC_Attributes(attr);
	}


//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Number of this Instance
	int* m_pNumberOfInstance;

	//! The Struct reference of this instance
	GLC_StructReference* m_pStructReference;

	//! The list of instance's occurences
	QList<GLC_StructOccurence*> m_ListOfOccurences;

	//! The relative matrix
	GLC_Matrix4x4 m_RelativeMatrix;

	//! The instance Name
	QString m_Name;

	//! The Reference attributes
	GLC_Attributes* m_pAttributes;
};

#endif /* GLC_STRUCTINSTANCE_H_ */
