//Registry
#pragma once

class CRegistryUtil
{
private:
	HKEY	m_hKey;
	CRegKey	m_reg;
	
public:
	CRegistryUtil(HKEY hKey)
	{
		m_hKey = hKey;
	}
	
	CString GetStringValue(HKEY hKey, CString strKeyName, CString strValueName, CString strDefaultValue = _T(""))
	{
		if(m_reg.Open(m_hKey, strKeyName) == ERROR_SUCCESS)
		{
			TCHAR szValue[128];
			ULONG ulValueLen = 128;
			if(m_reg.QueryStringValue(strValueName, szValue, &ulValueLen) == ERROR_SUCCESS)
			{
				m_reg.Close();
				return szValue;
			}
			m_reg.Close();
		}
		return strDefaultValue;
	}

	DWORD GetDWordValue(HKEY hKey, CString strKeyName, CString strValueName, DWORD dwDefaultValue = 0)
	{
		if(m_reg.Open(m_hKey, strKeyName) == ERROR_SUCCESS)
		{
			DWORD dwValue;
			if(m_reg.QueryDWORDValue(strValueName, dwValue) == ERROR_SUCCESS)
			{
				m_reg.Close();
				return dwValue;
			}
			m_reg.Close();
		}
		return dwDefaultValue;
	}

	ULONGLONG GetQWordValue(HKEY hKey, CString strKeyName, CString strValueName, ULONGLONG qwDefaultValue = 0)
	{
		if(m_reg.Open(m_hKey, strKeyName) == ERROR_SUCCESS)
		{
			ULONGLONG qwValue;
			if(m_reg.QueryQWORDValue(strValueName, qwValue) == ERROR_SUCCESS)
			{
				m_reg.Close();
				return qwValue;
			}
			m_reg.Close();
		}
		return qwDefaultValue;
	}

	BOOL SetStringValue(HKEY hKey, CString strKeyName, CString strValueName, CString strValue)
	{
		if(m_reg.Open(m_hKey, strKeyName) != ERROR_SUCCESS)
		{
			m_reg.Create(hKey, strKeyName);
			if(m_reg.Open(m_hKey, strKeyName) != ERROR_SUCCESS)
			{
				return FALSE;
			}
		}
		m_reg.SetStringValue(strValueName, strValue, REG_SZ);
		m_reg.Close();
		return TRUE;
	}

	BOOL SetDWordValue(HKEY hKey, CString strKeyName, CString strValueName, DWORD dwValue)
	{
		if(m_reg.Open(m_hKey, strKeyName) != ERROR_SUCCESS)
		{
			m_reg.Create(hKey, strKeyName);
			if(m_reg.Open(m_hKey, strKeyName) != ERROR_SUCCESS)
			{
				return FALSE;
			}
		}
		m_reg.SetDWORDValue(strValueName, dwValue);
		m_reg.Close();
		return TRUE;
	}

	BOOL SetQWordValue(HKEY hKey, CString strKeyName, CString strValueName, ULONGLONG qwValue)
	{
		if(m_reg.Open(m_hKey, strKeyName) != ERROR_SUCCESS)
		{
			m_reg.Create(hKey, strKeyName);
			if(m_reg.Open(m_hKey, strKeyName) != ERROR_SUCCESS)
			{
				return FALSE;
			}
		}
		m_reg.SetQWORDValue(strValueName, qwValue);
		m_reg.Close();
		return TRUE;
	}
};
