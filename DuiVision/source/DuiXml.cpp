#include "stdafx.h"

#include "DuiXml.h"

/*
DuiXmlParseResult::DuiXmlParseResult()
{
}

DuiXmlParseResult::~DuiXmlParseResult()
{
}

DuiXmlParseResult::operator bool() const
{
	return m_result.status == status_ok;
}
*/

DuiXmlAttribute::DuiXmlAttribute()
{
}

DuiXmlAttribute::~DuiXmlAttribute()
{
}

DuiXmlAttribute::operator DuiXmlAttribute::unspecified_bool_type() const
{
	return m_attribute;
}

DuiXmlAttribute& DuiXmlAttribute:: operator = (xml_attribute rhs)
{
    m_attribute = rhs;
    return *this;
}

const LPCTSTR DuiXmlAttribute::name() const
{
	return m_attribute.name();
}

const LPCTSTR DuiXmlAttribute::value() const
{
    return m_attribute.value();
}

DuiXmlAttribute DuiXmlAttribute::next_attribute() const
{
	DuiXmlAttribute attrib;
    attrib = m_attribute.next_attribute();
	return attrib;
}

DuiXmlNode::DuiXmlNode()
{
}

DuiXmlNode::~DuiXmlNode()
{
}

//static void unspecified_bool_xml_node(xml_node***)
//{
//}

DuiXmlNode::operator DuiXmlNode::unspecified_bool_type() const
{
	return m_node;
}

DuiXmlNode& DuiXmlNode:: operator = (xml_node rhs)
{
    m_node = rhs;
    return *this;
}

const LPCTSTR DuiXmlNode::name() const
{
	return m_node.name();
}

const LPCTSTR DuiXmlNode::value() const
{
    return m_node.value();
}

DuiXmlNode DuiXmlNode::first_child() const
{
	DuiXmlNode node;
    node = m_node.first_child();
	return node;
}

DuiXmlNode DuiXmlNode::last_child() const
{
	DuiXmlNode node;
    node = m_node.last_child();
	return node;
}

DuiXmlNode DuiXmlNode::next_sibling() const
{
	DuiXmlNode node;
    node = m_node.next_sibling();
	return node;
}

DuiXmlNode DuiXmlNode::child(const LPCTSTR name) const
{
	DuiXmlNode node;
    node = m_node.child(name);
	return node;
}

DuiXmlNode DuiXmlNode::next_sibling(const LPCTSTR name) const
{
	DuiXmlNode node;
    node = m_node.next_sibling(name);
	return node;
}

DuiXmlAttribute DuiXmlNode::first_attribute() const
{
	DuiXmlAttribute attrib;
    attrib = m_node.first_attribute();
	return attrib;
}

DuiXmlAttribute DuiXmlNode::attribute(const LPCTSTR name) const
{
	DuiXmlAttribute attrib;
    attrib = m_node.attribute(name);
	return attrib;
}

DuiXmlDocument::DuiXmlDocument()
{
}

DuiXmlDocument::~DuiXmlDocument()
{
}

/*
DuiXmlDocument& DuiXmlDocument:: operator = (xml_document rhs)
{
    m_document = rhs;
    return *this;
}
*/

DuiXmlNode DuiXmlDocument::child(const LPCTSTR name) const
{
	DuiXmlNode node;
    node = m_document.child(name);
	return node;
}

DuiXmlParseResult DuiXmlDocument::load(const LPCTSTR contents, unsigned int options)
{
	//DuiXmlParseResult result;
	//const char_t* contents = pcontents;
	return m_document.load(contents, options);
	//return result;
}

DuiXmlParseResult DuiXmlDocument::load_file(const LPCTSTR path, unsigned int options, xml_encoding encoding)
{
	//DuiXmlParseResult result;
	return m_document.load_file(path, options, encoding);
	//return result;
}
