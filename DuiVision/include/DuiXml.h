
#pragma once

#include "../pugixml/pugixml.hpp"

using namespace pugi;

/*
class DuiXmlParseResult
{
private:
	xml_parse_result m_result;
public:
	DuiXmlParseResult();
	~DuiXmlParseResult();

	operator bool() const;

	DuiXmlParseResult& operator = (const xml_parse_result rhs)
    {
        m_result = rhs;
        return *this;
    }
};
*/

typedef xml_parse_result DuiXmlParseResult;

class DuiXmlAttribute : public xml_attribute
{
protected:
	xml_attribute m_attribute;
	typedef void (*unspecified_bool_type)(xml_attribute***);
public:
	DuiXmlAttribute();
	~DuiXmlAttribute();

	operator unspecified_bool_type() const;
	DuiXmlAttribute& operator = (const xml_attribute rhs);

	// Get attribute name/value, or "" if attribute is empty
	const LPCTSTR name() const;
	const LPCTSTR value() const;
	DuiXmlAttribute next_attribute() const;
};

class DuiXmlNode : public xml_node
{
protected:
	xml_node m_node;
	typedef void (*unspecified_bool_type)(xml_node***);
public:
	DuiXmlNode();
	~DuiXmlNode();

	operator unspecified_bool_type() const;
	DuiXmlNode& operator = (const xml_node rhs);

	// Get node name, or "" if node is empty or it has no name
	const LPCTSTR name() const;

	// Get node value, or "" if node is empty or it has no value
    // Note: For <node>text</node> node.value() does not return "text"! Use child_value() or text() methods to access text inside nodes.
	const LPCTSTR value() const;

	DuiXmlNode first_child() const;
	DuiXmlNode last_child() const;
	DuiXmlNode next_sibling() const;

	DuiXmlNode child( LPCTSTR name) const;
	DuiXmlNode next_sibling( LPCTSTR name) const;

	DuiXmlAttribute first_attribute() const;
	DuiXmlAttribute attribute( LPCTSTR name) const;
};

class DuiXmlDocument : public xml_document
{
protected:
	xml_document m_document;
public:
	DuiXmlDocument();
	~DuiXmlDocument();

	//operator bool() const;
	//DuiXmlDocument& operator = (const xml_document rhs);

	DuiXmlNode child( LPCTSTR name) const;

	// Load document from zero-terminated string. No encoding conversions are applied.
	DuiXmlParseResult load(const LPCTSTR contents, unsigned int options = parse_default);

	// Load document from file
	DuiXmlParseResult load_file(const LPCTSTR path, unsigned int options = parse_default, xml_encoding encoding = encoding_auto);

};
