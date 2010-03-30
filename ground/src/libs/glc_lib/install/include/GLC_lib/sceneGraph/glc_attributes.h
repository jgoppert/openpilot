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
//! \file glc_attributes.h interface for the GLC_Attributes class.

#ifndef GLC_ATTRIBUTES_H_
#define GLC_ATTRIBUTES_H_

#include <QString>
#include <QList>
#include <QHash>

//////////////////////////////////////////////////////////////////////
//! \class GLC_Attributes
/*! \brief GLC_Attributes : User attributes of instance and reference */
//////////////////////////////////////////////////////////////////////
class GLC_Attributes
{

//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Default Constructor
	GLC_Attributes();

	//! Copy Constructor
	GLC_Attributes(const GLC_Attributes&);

	//! Destructor
	virtual ~GLC_Attributes();

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Return true if attributes is empty
	inline bool isEmpty() const
	{return m_AttributesHash.isEmpty();}

	//! Return the size of attributes
	inline int size() const
	{return m_AttributesHash.size();}

	//! Return true if the specified attribute exist
	bool contains(const QString& name) const
	{return m_AttributesHash.contains(name);}

	//! Return the list of attribute name
	inline QList<QString> names() const
	{return m_AttributesHash.keys();}

	//! Return the value of the specified attributes
	/*! Return NULL String if attribute doesn't exist*/
	inline QString value(const QString& name) const
	{return m_AttributesHash.value(name);}

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Insert an attribute
	inline void insert(const QString& name, const QString& value)
	{m_AttributesHash.insert(name, value);}

	//! Remove an attribute
	inline void remove(const QString& name)
	{m_AttributesHash.remove(name);}

//@}

//////////////////////////////////////////////////////////////////////
/*! @name Operator Overload */
//@{
//////////////////////////////////////////////////////////////////////
public:
	//! Assignement operator overload
	inline GLC_Attributes& operator=(const GLC_Attributes& attr)
	{
		m_AttributesHash= attr.m_AttributesHash;
		return *this;
	}

	//! Equal operator overload
	inline bool operator==(const GLC_Attributes& attr) const
	{return m_AttributesHash == attr.m_AttributesHash;}

//@}
//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
	//! Attributes Hash table
	QHash<QString, QString> m_AttributesHash;
};

#endif /* GLC_ATTRIBUTES_H_ */
