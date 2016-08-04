#include "stdafx.h"

#include "DuiXml.h"
//////////////////////////////////////////////////////////////////////////
//辅助函数
bool multiToUnicode(const std::string& multiText, std::wstring& unicodeText)
{
	if (0 == multiText.length())
	{
		unicodeText.clear();
		return true;
	}

	//
	int size = ::MultiByteToWideChar(CP_ACP, 0, multiText.c_str(), -1, NULL, 0);
	if (0 == size)
	{
		return false;
	}

	//
	wchar_t* wszBuffer = new wchar_t[size + 1];
	::ZeroMemory(wszBuffer, (size + 1) * sizeof(wchar_t));
	if (0 == ::MultiByteToWideChar(CP_ACP, 0, multiText.c_str(), -1, wszBuffer, size + 1))
	{
		delete[] wszBuffer;
		return false;
	}

	unicodeText = wszBuffer;
	delete[] wszBuffer;
	return true;
}

bool unicodeToMulti(const std::wstring& unicodeText, std::string& multiText)
{
	if (0 == unicodeText.length())
	{
		multiText.clear();
		return true;
	}

	//
	int size = ::WideCharToMultiByte(CP_ACP, 0, unicodeText.c_str(), -1, NULL, 0, NULL, NULL);
	if (0 == size)
	{
		return false;
	}

	//
	char* szBuffer = new char[size + 1];
	::ZeroMemory(szBuffer, (size + 1) * sizeof(char));
	if (0 == ::WideCharToMultiByte(CP_ACP, 0, unicodeText.c_str(), -1, szBuffer, size + 1, NULL, NULL))
	{
		delete[] szBuffer;
		return false;
	}

	multiText = szBuffer;
	delete[] szBuffer;
	return true;
}

bool unicodeToUtf8(const std::wstring& unicodeText, std::string& utf8Text)
{
	if (0 == unicodeText.length())
	{
		utf8Text.clear();
		return true;
	}

	//
	int size = ::WideCharToMultiByte(CP_UTF8, 0, unicodeText.c_str(), -1, NULL, 0, NULL, NULL);
	if (0 == size)
	{
		return false;
	}

	//
	char* szBuffer = new char[size + 1];
	::ZeroMemory(szBuffer, (size + 1) * sizeof(char));
	if (0 == ::WideCharToMultiByte(CP_UTF8, 0, unicodeText.c_str(), -1, szBuffer, size + 1, NULL, NULL))
	{
		delete[] szBuffer;
		return false;
	}

	utf8Text = szBuffer;
	delete[] szBuffer;
	return true;
}

bool utf8ToUnicode(const std::string& utf8Text, std::wstring& unicodeText)
{
	if (0 == utf8Text.length())
	{
		unicodeText.clear();
		return true;
	}

	//
	int size = ::MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, NULL, 0);
	if (0 == size)
	{
		return false;
	}

	//
	wchar_t* wszBuffer = new wchar_t[size + 1];
	::ZeroMemory(wszBuffer, (size + 1) * sizeof(wchar_t));
	if (0 == ::MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, wszBuffer, size + 1))
	{
		delete[] wszBuffer;
		return false;
	}

	unicodeText = wszBuffer;
	delete[] wszBuffer;
	return true;
}
//////////////////////////////////////////////////////////////////////////
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
	return m_document.load(contents, options & ~parse_wconv_attribute);
	//return result;
}

DuiXmlParseResult DuiXmlDocument::load_file(const LPCTSTR path, unsigned int options, xml_encoding encoding)
{
#ifdef _UNICODE
	//DuiXmlParseResult result;
	return m_document.load_file(path, options & ~parse_wconv_attribute, encoding);
	//return result;
#else
	DuiXmlParseResult result;
	//std::locale::global(std::locale("chs"));
	//const std::wstring strFilePath = CEncodingUtil::AnsiToUnicode(path);
	//std::wifstream stream(strFilePath.c_str());
	//pugi::xml_document doc;
	//doc.load(stream);

	/*
		文件头要注意
		// Step 1: 写入 Unicode 档头, 标明这是 Unicode 檔 (关键片段)
		unsigned char u[2]={0xff,0xfe}; // Unicode 档头 = 0xfeff
		fwrite(u,2,1,fp);

		// Step 2: 写入真正 Unicode 数据
		_ftprintf(fp,_T("Record_Filename=%s\n"),recordfilename);

		// Step 3: 关档写入
		fclose(fp);

		// Step 4: 请用 NotePad++ 看看内容
	*/
	FILE* file = fopen(path, "rb");
	if(!file)
	{
		result.status=status_file_not_found;
		return result;
	}
	
	// read file in memory
	fseek(file, 0, SEEK_END);
	int nsize=ftell(file)+1;
	char *contents=new char[nsize];
	fseek(file, 0, SEEK_SET);
	size_t read_size = fread(contents, nsize, 1, file);
	contents[nsize-1]=0;
	fclose(file);

	//////////////////////////////////////////////////////////////////////////
	std::string strSource=contents;
	delete[] contents;

	std::wstring unicodeText;
	utf8ToUnicode(strSource,unicodeText);

	std::string strResult;
	unicodeToMulti(unicodeText,strResult);

	result = m_document.load(strResult.c_str(), options & ~parse_wconv_attribute);
	//result = m_document.load_buffer(szBuffer,size+1);
	
	return result;
#endif
}
