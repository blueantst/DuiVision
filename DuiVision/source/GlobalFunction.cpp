#include "StdAfx.h"
#include <winuser.h>
#include <cmath>
#include "GlobalFunction.h"

// 读取图片(从文件读)
BOOL LoadBitmapFromFile(const CString strPathFile, CBitmap &bitmap, CSize &size)
{	
	HBITMAP hBitmap = NULL;
#ifdef _UNICODE
	Bitmap* pBitmap = Bitmap::FromFile(strPathFile);
#else
	Bitmap* pBitmap = Bitmap::FromFile(CEncodingUtil::AnsiToUnicode(strPathFile));
#endif
	Status status = pBitmap->GetLastStatus();
	if(Ok == status)
	{		
		status = pBitmap->GetHBITMAP(Color(0,0,0), &hBitmap);
		if(Ok == status)
		{
			if(bitmap.m_hObject != NULL)
			{
				bitmap.Detach();
			}
			bitmap.Attach(hBitmap);
			
			BITMAP bmInfo;
			::GetObject( bitmap.m_hObject, sizeof(BITMAP), &bmInfo );
			size.cx = bmInfo.bmWidth;
			size.cy = bmInfo.bmHeight;

			delete pBitmap;

			return TRUE;
		}
	}

	return FALSE;
}

// 读取图片(从资源读)
BOOL LoadBitmapFromIDResource(UINT nID, CBitmap &bitmap, CSize &size, CString strType)
{
	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID), strType);
	if (!hRsrc)  
	{
		return FALSE; 
	}

	DWORD len = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)  
	{
		return FALSE;
	}
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,len);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);  
	Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pstm);  
	GlobalUnlock(m_hMem);  
	GlobalFree(m_hMem);
	pstm->Release();  
	FreeResource(lpRsrc); 

	HBITMAP hBitmap = NULL;
	Status status = pBitmap->GetLastStatus();
	if(Ok == status)
	{		
		status = pBitmap->GetHBITMAP(Color(0,0,0), &hBitmap);
		delete pBitmap;

		if(Ok == status)
		{
			if(bitmap.m_hObject != NULL)
			{
				bitmap.Detach();
			}
			bitmap.Attach(hBitmap);

			BITMAP bmInfo;
			::GetObject( bitmap.m_hObject, sizeof(BITMAP), &bmInfo );
			size.cx = bmInfo.bmWidth;
			size.cy = bmInfo.bmHeight;				

			return TRUE;
		}
	}

	return FALSE;
}

// 读取图片(从内存中加载)
BOOL LoadBitmapFromMem(BYTE* pByte, DWORD dwSize, CBitmap &bitmap, CSize &size)
{
	// 根据文件大小分配HGLOBAL内存
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD, dwSize );
	if ( !hGlobal )
	{
		TRACE( _T( "Load (file): Error allocating memory\n" ) );
		return FALSE;
	};

	char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
	if ( !pData )
	{
		TRACE( _T( "Load (file): Error locking memory\n" ) );
		GlobalFree( hGlobal );
		return FALSE;
	};

	// 将文件内容读到HGLOBAL内存中
	memcpy(pData, pByte, dwSize);
	GlobalUnlock( hGlobal );

	// 利用hGlobal内存中的数据创建stream
	IStream *pStream = NULL;
	if ( CreateStreamOnHGlobal( hGlobal, TRUE, &pStream ) != S_OK )
	{
		return FALSE;
	}

	Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pStream);

	// 要加上这一句，否则由GlobalAlloc得来的hGlobal内存没有被释放，导致内存泄露，由于
	// CreateStreamOnHGlobal第二个参数被设置为TRUE，所以调用pStream->Release()会自动
	// 将hGlobal内存（参见msdn对CreateStreamOnHGlobal的说明）
	pStream->Release();

	HBITMAP hBitmap = NULL;
	Status status = pBitmap->GetLastStatus();
	if(Ok == status)
	{		
		status = pBitmap->GetHBITMAP(Color(0,0,0), &hBitmap);
		delete pBitmap;

		if(Ok == status)
		{
			if(bitmap.m_hObject != NULL)
			{
				bitmap.Detach();
			}
			bitmap.Attach(hBitmap);

			BITMAP bmInfo;
			::GetObject( bitmap.m_hObject, sizeof(BITMAP), &bmInfo );
			size.cx = bmInfo.bmWidth;
			size.cy = bmInfo.bmHeight;				

			return TRUE;
		}
	}

	return FALSE;
}

// 读取图标(从文件读)
BOOL LoadIconFromFile(CString strPathFile, HICON &hIcon)
{
	WORD wIndex = 0;
	hIcon = ::ExtractAssociatedIcon(NULL, strPathFile.GetBuffer(0), &wIndex);
	strPathFile.ReleaseBuffer();

	return (hIcon != NULL);
}

// 读取图标(从资源读)
BOOL LoadIconFromIDResource(UINT nID, HICON &hIcon)
{
	hIcon = AfxGetApp()->LoadIcon(nID);

	return (hIcon != NULL);
}

// 读取图标(从内存中加载)
BOOL LoadIconFromMem(BYTE* pByte, DWORD dwSize, HICON &hIcon)
{
	// 根据文件大小分配HGLOBAL内存
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD, dwSize );
	if ( !hGlobal )
	{
		TRACE( _T( "Load (file): Error allocating memory\n" ) );
		return FALSE;
	};

	char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
	if ( !pData )
	{
		TRACE( _T( "Load (file): Error locking memory\n" ) );
		GlobalFree( hGlobal );
		return FALSE;
	};

	// 将文件内容读到HGLOBAL内存中
	memcpy(pData, pByte, dwSize);
	GlobalUnlock( hGlobal );

	// 利用hGlobal内存中的数据创建stream
	IStream *pStream = NULL;
	if ( CreateStreamOnHGlobal( hGlobal, TRUE, &pStream ) != S_OK )
	{
		return FALSE;
	}

	Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pStream);

	// 要加上这一句，否则由GlobalAlloc得来的hGlobal内存没有被释放，导致内存泄露，由于
	// CreateStreamOnHGlobal第二个参数被设置为TRUE，所以调用pStream->Release()会自动
	// 将hGlobal内存（参见msdn对CreateStreamOnHGlobal的说明）
	pStream->Release();

	HBITMAP hBitmap = NULL;
	Status status = pBitmap->GetLastStatus();
	if(Ok == status)
	{
		status = pBitmap->GetHICON(&hIcon);
		delete pBitmap;

		if(Ok == status)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 加载图片文件到内存中
BOOL LoadImageFromFile(CString strFile, BOOL useEmbeddedColorManagement, Image*& pImg)
{
	//pImg = Image::FromFile(strFile, useEmbeddedColorManagement);
	//return (pImg->GetLastStatus() == Ok);

	CFile file;
	DWORD dwSize;

	// 打开文件
	if ( !file.Open( strFile,
		CFile::modeRead | 
		CFile::shareDenyWrite ) )
	{
		TRACE( _T( "Load (file): Error opening file %s\n" ), strFile );
		pImg = NULL;
		return FALSE;
	};

	// 根据文件大小分配HGLOBAL内存
	dwSize = (DWORD)file.GetLength();
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD, dwSize );
	if ( !hGlobal )
	{
		TRACE( _T( "Load (file): Error allocating memory\n" ) );
		pImg = NULL;
		return FALSE;
	};

	char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
	if ( !pData )
	{
		TRACE( _T( "Load (file): Error locking memory\n" ) );
		GlobalFree( hGlobal );
		pImg = NULL;
		return FALSE;
	};

	// 将文件内容读到HGLOBAL内存中
	TRY
	{
		file.Read( pData, dwSize );
	}
	CATCH( CFileException, e );                                          
	{
		TRACE( _T( "Load (file): An exception occured while reading the file %s\n"),
			strFile );
		GlobalFree( hGlobal );
		e->Delete();
		file.Close();
		pImg = NULL;
		return FALSE;
	}
	END_CATCH

	GlobalUnlock( hGlobal );
	file.Close();

	// 利用hGlobal内存中的数据创建stream
	IStream *pStream = NULL;
	if ( CreateStreamOnHGlobal( hGlobal, TRUE, &pStream ) != S_OK )
	{
		pImg = NULL;
		return FALSE;
	}

	pImg = Gdiplus::Image::FromStream( pStream, useEmbeddedColorManagement );

	// 要加上这一句，否则由GlobalAlloc得来的hGlobal内存没有被释放，导致内存泄露，由于
	// CreateStreamOnHGlobal第二个参数被设置为TRUE，所以调用pStream->Release()会自动
	// 将hGlobal内存（参见msdn对CreateStreamOnHGlobal的说明）
	pStream->Release();

	return TRUE;
}

// 从资源中加载图片
BOOL LoadImageFromIDResource(UINT nID, CString strType, BOOL useEmbeddedColorManagement, Image*& pImg)  
{  
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),strType);
	if (!hRsrc)
	{
		pImg = NULL;
		return FALSE;
	}

	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
	{
		pImg = NULL;
		return FALSE;
	}
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem, lpRsrc, len);
	IStream* pStream = NULL;
	if ( CreateStreamOnHGlobal( m_hMem, FALSE, &pStream ) != S_OK )
	{
		pImg = NULL;
		return FALSE;
	}

	pImg = Gdiplus::Image::FromStream(pStream, useEmbeddedColorManagement);

	GlobalUnlock(m_hMem);
	//GlobalFree(m_hMem);
	pStream->Release();
	FreeResource(lpRsrc);

	return TRUE;
}

// 从内存中加载图片文件
BOOL LoadImageFromMem(BYTE* pByte, DWORD dwSize, BOOL useEmbeddedColorManagement, Image*& pImg)
{
	// 根据文件大小分配HGLOBAL内存
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD, dwSize );
	if ( !hGlobal )
	{
		TRACE( _T( "Load (file): Error allocating memory\n" ) );
		pImg = NULL;
		return FALSE;
	};

	char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
	if ( !pData )
	{
		TRACE( _T( "Load (file): Error locking memory\n" ) );
		GlobalFree( hGlobal );
		pImg = NULL;
		return FALSE;
	};

	// 将文件内容读到HGLOBAL内存中
	memcpy(pData, pByte, dwSize);
	GlobalUnlock( hGlobal );

	// 利用hGlobal内存中的数据创建stream
	IStream *pStream = NULL;
	if ( CreateStreamOnHGlobal( hGlobal, TRUE, &pStream ) != S_OK )
	{
		pImg = NULL;
		return FALSE;
	}

	pImg = Gdiplus::Image::FromStream( pStream, useEmbeddedColorManagement );

	// 要加上这一句，否则由GlobalAlloc得来的hGlobal内存没有被释放，导致内存泄露，由于
	// CreateStreamOnHGlobal第二个参数被设置为TRUE，所以调用pStream->Release()会自动
	// 将hGlobal内存（参见msdn对CreateStreamOnHGlobal的说明）
	pStream->Release();

	return TRUE;
}

// 取得图片平均颜色
BOOL GetAverageColor(CDC *pDC, CBitmap &bitmap, const CSize &sizeImage, COLORREF &clrImage)
{
	BOOL bIsResult = false;

	CDC TempDC;
	TempDC.CreateCompatibleDC(pDC);
	CBitmap *pOldBitmap = TempDC.SelectObject(&bitmap);

	int nWidth = sizeImage.cx;
	int nHeight = sizeImage.cy;
	// 定义位图信息   
	BITMAPINFO bi;  
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);  
	bi.bmiHeader.biWidth = nWidth;  
	bi.bmiHeader.biHeight = nHeight;  
	bi.bmiHeader.biPlanes = 1;  
	bi.bmiHeader.biBitCount = 32;   
	bi.bmiHeader.biCompression = BI_RGB;   
	bi.bmiHeader.biSizeImage = nWidth * nHeight * 4; // 32 bit   
	bi.bmiHeader.biClrUsed = 0;  
	bi.bmiHeader.biClrImportant = 0;  

	// 获取位图数据   
	BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];  
	::ZeroMemory(pBits, bi.bmiHeader.biSizeImage);  

	int nLine = ::GetDIBits(TempDC.m_hDC, (HBITMAP)bitmap.GetSafeHandle(), 0, nHeight, pBits, &bi, DIB_RGB_COLORS);
	if (!nLine)  
	{  
		delete []pBits;  
		pBits = NULL;  
	} 
	else
	{
		long r = 0, g = 0, b = 0;

		for (LONG i = 0; i < nWidth; i++)     
		{     
			for (LONG j = 0; j < nHeight; j++)     
			{     
				b += pBits[(j * nWidth + i) * 4];					
				g += pBits[(j * nWidth + i) * 4 + 1];
				r += pBits[(j * nWidth + i) * 4 + 2];		
			}     
		}  

		delete []pBits;  
		pBits = NULL;  
		int n = nWidth * nHeight;
		clrImage = RGB(r / n, g / n, b / n);
		bIsResult = true;
	}

	TempDC.SelectObject(pOldBitmap);
	TempDC.DeleteDC();

	return bIsResult;
}

// 取得图片大小
BOOL GetSize(CBitmap &bitmap, CSize &size)
{
	if(bitmap.m_hObject)
	{
		BITMAP bmInfo;
		::GetObject( bitmap.m_hObject, sizeof(BITMAP), &bmInfo );
		size.cx = bmInfo.bmWidth;
		size.cy = bmInfo.bmHeight;
		return true;
	}
	return false;
}

// 获取文字需要的显示区域
Size GetTextBounds(const Font& font,const StringFormat& strFormat,const CString& strText)
{
	GraphicsPath path;
	FontFamily fontfamily;
	font.GetFamily(&fontfamily);
	BSTR bsText = strText.AllocSysString();
	path.AddString(bsText,-1,&fontfamily,font.GetStyle(),font.GetSize(),PointF(0,0),&strFormat);
	::SysFreeString(bsText);
	RectF rcBound;
	path.GetBounds(&rcBound);

	REAL rHeight = font.GetHeight(0.0f);

	return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
		(int)(rHeight > (int)rHeight ? rHeight + 4 : rHeight + 1));

	//return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
	//	(int)(rcBound.Height > (int)rcBound.Height ? rcBound.Height + 2 : rcBound.Height + 1));
}

// 获取文字需要的显示区域(指定宽度)
Size GetTextBounds(const Font& font,const StringFormat& strFormat,int nWidth,const CString& strText)
{
	GraphicsPath path;
	FontFamily fontfamily;
	font.GetFamily(&fontfamily);
	BSTR bsText = strText.AllocSysString();
	path.AddString(bsText,-1,&fontfamily,font.GetStyle(),font.GetSize(),RectF(0,0,(Gdiplus::REAL)nWidth,0),&strFormat);
	::SysFreeString(bsText);
	RectF rcBound;
	path.GetBounds(&rcBound);

	REAL rHeight = font.GetHeight(0.0f);
	int nHeight = (int)(rHeight > (int)rHeight ? rHeight + 2 : rHeight + 1);
	int nHeightBound = (int)(rcBound.Height > (int)rcBound.Height ? rcBound.Height + 2 : rcBound.Height + 1);

	return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
		(nHeightBound > nHeight) ? nHeightBound+5 : nHeight+1);

	//return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
	//	(int)(rcBound.Height > (int)rcBound.Height ? rcBound.Height + 2 : rcBound.Height + 1));
}

// 获取文字需要的显示区域(使用非换行的默认格式)
Size GetTextBounds(const Font& font,const CString& strText)
{
	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);   
	strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);

	GraphicsPath path;
	FontFamily fontfamily;
	font.GetFamily(&fontfamily);
	BSTR bsText = strText.AllocSysString();
	path.AddString(bsText,-1,&fontfamily,font.GetStyle(),font.GetSize(),PointF(0,0),&strFormat);
	::SysFreeString(bsText);
	RectF rcBound;
	path.GetBounds(&rcBound);
	REAL rHeight = font.GetHeight(0.0f);

	return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
		(int)(rHeight > (int)rHeight ? rHeight + 4 : rHeight + 1));

	//return Size((int)(rcBound.Width > (int)rcBound.Width ? rcBound.Width + 1 : rcBound.Width),
	//	(int)(rcBound.Height > (int)rcBound.Height ? rcBound.Height + 2 : rcBound.Height + 1));
}

// 取得位置
CPoint GetOriginPoint(int nWidth, int nHeight, int nChildWidth, int nChildHeight, UINT uAlignment, UINT uVAlignment)
{
	int nXPos = 0;
	int nYPos = 0;
	if(uAlignment == DT_CENTER)
	{
		nXPos = (nWidth -  nChildWidth) / 2;
	}
	else if(uAlignment == DT_RIGHT)
	{
		nXPos = nWidth - nChildWidth;
	}

	if(uVAlignment == DT_VCENTER)
	{
		nYPos = (nHeight - nChildHeight) / 2;
	}
	else if(uVAlignment == DT_BOTTOM)
	{
		nYPos = nHeight - nChildHeight;
	}

	return CPoint(nXPos, nYPos);
}

// 取得位置
CPoint GetOriginPoint(CRect rc, int nChildWidth, int nChildHeight, UINT uAlignment, UINT uVAlignment)
{
	CPoint point = GetOriginPoint(rc.Width(), rc.Height(), nChildWidth, nChildHeight, uAlignment, uVAlignment);
	
	return CPoint(rc.left + point.x, rc.top + point.y);
}

// 转换数字
CString DecimalFormat(int nNumber)
{
	CString str;
	CString strTemp;
	int nNumberTemp = nNumber;

	if(nNumberTemp / 1000 > 0)
	{
		strTemp.Format(TEXT("%03d"), nNumberTemp % 1000);
	}
	else
	{
		strTemp.Format(TEXT("%d,"), nNumberTemp % 1000);
	}
	nNumberTemp = nNumberTemp / 1000;
	str = strTemp;

	while(nNumberTemp > 0)
	{
		if(nNumberTemp / 1000 > 0)
		{
			strTemp.Format(TEXT("%03d,"), nNumberTemp % 1000);
		}
		else
		{
			strTemp.Format(TEXT("%d,"), nNumberTemp % 1000);
		}
		nNumberTemp = nNumberTemp / 1000;
		str = strTemp + str;
	}

	return str;
}

// 画垂直过渡
int DrawVerticalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent/* = 0*/, int nEndTransparent/* = 100*/)
{
	//ASSERT(rcDes.Width() == rcSrc.Width() && rcDes.Height() == rcSrc.Height());

	BOOL bIsDownTransition = TRUE;
	if (nEndTransparent <= nBeginTransparent)
	{
		bIsDownTransition = FALSE;
		int nTemp = nBeginTransparent;
		nBeginTransparent = nEndTransparent;
		nEndTransparent = nTemp;
	}

	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;

	int nWidth = rcDes.Width();
	int nHeight =  rcDes.Height();

	int nMinTransition = (int)(255 - 255.0 * nBeginTransparent / 100.0);
	int nMaxTransition = (int)(255.0 * (100 - nEndTransparent) / 100.0);
	double dTransition = (nMinTransition - nMaxTransition) / (double)nHeight;
 
	for (int i = 0; i < nHeight; i++)
	{
		blend.SourceConstantAlpha = (BYTE)(bIsDownTransition ? nMinTransition - (dTransition * i) : nMaxTransition + (dTransition * i)); 
		dcDes.AlphaBlend(rcDes.left, rcDes.top + i,nWidth , 1, &dcSrc, rcSrc.left, rcSrc.top + i, nWidth, 1, blend); 
	}

	return blend.SourceConstantAlpha;
}

// 画水平过渡
int DrawHorizontalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent/* = 0*/, int nEndTransparent/* = 100*/)
{
	ASSERT(rcDes.Width() == rcSrc.Width() && rcDes.Height() == rcSrc.Height());

	BOOL bIsDownTransition = TRUE;
	if (nEndTransparent <= nBeginTransparent)
	{
		bIsDownTransition = FALSE;
		int nTemp = nBeginTransparent;
		nBeginTransparent = nEndTransparent;
		nEndTransparent = nTemp;
	}

	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;

	int nWidth = rcDes.Width();
	int nHeight =  rcDes.Height();

	int nMinTransition = (int)(255 - 255.0 * nBeginTransparent / 100.0);
	int nMaxTransition = (int)(255.0 * (100 - nEndTransparent) / 100.0);
	double dTransition = (nMinTransition - nMaxTransition) / (double)nWidth;

	for (int i = 0; i < nWidth; i++)
	{
		blend.SourceConstantAlpha = (BYTE)(bIsDownTransition ? nMinTransition - (dTransition * i) : nMaxTransition + (dTransition * i)); 
		dcDes.AlphaBlend(rcDes.left + i, rcDes.top , 1, nHeight, &dcSrc, rcSrc.left + i, rcSrc.top, 1, nHeight, blend); 
	}

	return blend.SourceConstantAlpha;
}

// 画右下角过渡
void DrawRightBottomTransition(CDC &dc, CDC &dcTemp, CRect rc, const int nOverRegio, const COLORREF clrBackground)
{
	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp= AC_SRC_OVER;
	blend.SourceConstantAlpha = 255; 

	int nStartX = rc.Width() - nOverRegio;
	int nStartY = rc.Height() - nOverRegio;
	double dRadius = (double)sqrt((double)(nOverRegio * nOverRegio + nOverRegio * nOverRegio));
	double dRadiusTemp = 0.0;

	for (int i = 0; i < nOverRegio; i++)
	{
		for (int j = 0; j < nOverRegio; j++)
		{
			dRadiusTemp  = (double)sqrt((double)(i * i + j * j));
			if (dRadiusTemp > nOverRegio)
			{
				dRadiusTemp = nOverRegio;
			}
			blend.SourceConstantAlpha = (BYTE)(255 - (255.0 / nOverRegio * (dRadiusTemp)));
			dc.AlphaBlend(nStartX + i, nStartY + j, 1, 1, &dcTemp, nStartX + i, nStartY + j, 1, 1, blend);
		} 	
	}
}

// 画图片边框
void DrawImageFrame(Graphics &graphics, Image *pImage, const CRect &rcControl, int nX, int nY, int nW, int nH, int nFrameSide/* = 4*/)
{
	// 左上角
	graphics.DrawImage(pImage, Rect(rcControl.left ,rcControl.top, nFrameSide, nFrameSide),
		nX, nY, nFrameSide, nFrameSide, UnitPixel);

	// 左中边框
	graphics.DrawImage(pImage, Rect(rcControl.left ,rcControl.top + nFrameSide, nFrameSide, rcControl.Height() - 2 * nFrameSide),
		nX, nY + nFrameSide, nFrameSide, nH - 2 * nFrameSide, UnitPixel);

	// 左下角
	graphics.DrawImage(pImage, Rect(rcControl.left ,rcControl.bottom - nFrameSide, nFrameSide, nFrameSide), 
		nX, nY + nH - nFrameSide, nFrameSide, nFrameSide, UnitPixel);

	// 上中边框
	graphics.DrawImage(pImage, Rect(rcControl.left + nFrameSide ,rcControl.top, rcControl.Width() - 2 * nFrameSide, nFrameSide),
		nX + nFrameSide, nY, nW - 2 * nFrameSide, nFrameSide, UnitPixel);

	// 右上角
	graphics.DrawImage(pImage, Rect(rcControl.right - nFrameSide ,rcControl.top, nFrameSide, nFrameSide), 
		nX + nW - nFrameSide, nY, nFrameSide, nFrameSide, UnitPixel);

	// 右中边框
	graphics.DrawImage(pImage, Rect(rcControl.right - nFrameSide ,rcControl.top + nFrameSide, nFrameSide, rcControl.Height() - 2 * nFrameSide), 
		nX + nW - nFrameSide, nY + nFrameSide, nFrameSide, nH - 2 * nFrameSide, UnitPixel);

	// 右下角
	graphics.DrawImage(pImage, Rect(rcControl.right - nFrameSide ,rcControl.bottom - nFrameSide, nFrameSide, nFrameSide), 
		nX + nW - nFrameSide, nY + nH - nFrameSide, nFrameSide, nFrameSide, UnitPixel);	

	 // 下中边框
	graphics.DrawImage(pImage, Rect(rcControl.left + nFrameSide ,rcControl.bottom - nFrameSide, rcControl.Width() - 2 * nFrameSide, nFrameSide), 
		nX + nFrameSide, nY + nH - nFrameSide, nW - 2 * nFrameSide, nFrameSide, UnitPixel);

	// 中间
	graphics.DrawImage(pImage, Rect(rcControl.left + nFrameSide ,rcControl.top + nFrameSide, rcControl.Width() - 2 * nFrameSide, rcControl.Height() - 2 * nFrameSide), 
		nX + nFrameSide, nY + nFrameSide, nW - 2 * nFrameSide, nH - 2 *  nFrameSide, UnitPixel);
}

// 画图片边框(可指定九宫格的位置,nX/nY是原图的左上角,nWH/nHL是左上角点坐标,nWR/nHR是右下角点坐标)
void DrawImageFrameMID(Graphics &graphics, Image *pImage, const CRect &rcControl, int nX, int nY, int nW, int nH, int nWL, int nHL, int nWR, int nHR)
{
	// 左上角
	graphics.DrawImage(pImage, Rect(rcControl.left ,rcControl.top, nWL, nHL),
		nX, nY, nWL, nHL, UnitPixel);

	// 左中边框
	graphics.DrawImage(pImage, Rect(rcControl.left ,rcControl.top + nHL, nWL, rcControl.Height() - nHL - nHR),
		nX, nY + nHL, nWL, nH - nHL - nHR, UnitPixel);

	// 左下角
	graphics.DrawImage(pImage, Rect(rcControl.left ,rcControl.bottom - nHR, nWL, nHR), 
		nX, nY + nH - nHR, nWL, nHR, UnitPixel);

	// 上中边框
	graphics.DrawImage(pImage, Rect(rcControl.left + nWL ,rcControl.top, rcControl.Width() - nWL - nWR, nHL),
		nX + nWL, nY, nW - nWL - nWR, nHL, UnitPixel);

	// 右上角
	graphics.DrawImage(pImage, Rect(rcControl.right - nWR ,rcControl.top, nWR, nHL), 
		nX + nW - nWR, nY, nWR, nHL, UnitPixel);

	// 右中边框
	graphics.DrawImage(pImage, Rect(rcControl.right - nWR ,rcControl.top + nHL, nWR, rcControl.Height() - nHL - nHR), 
		nX + nW - nWR, nY + nHL, nWR, nH - nHL - nHR, UnitPixel);

	// 右下角
	graphics.DrawImage(pImage, Rect(rcControl.right - nWR ,rcControl.bottom - nHR, nWR, nHR), 
		nX + nW - nWR, nY + nH - nHR, nWR, nHR, UnitPixel);	

	 // 下中边框
	graphics.DrawImage(pImage, Rect(rcControl.left + nWL ,rcControl.bottom - nHR, rcControl.Width() - nWL - nWR, nHR), 
		nX + nWL, nY + nH - nHR, nW - nWL - nWR, nHR, UnitPixel);

	// 中间
	graphics.DrawImage(pImage, Rect(rcControl.left + nWL ,rcControl.top + nHL, rcControl.Width() - nWL - nWR, rcControl.Height() - nHL - nHR), 
		nX + nWL, nY + nHL, nW - nWL - nWR, nH - nHL - nHR, UnitPixel);
}

// 画过渡圆角矩加
void DrawRectangle(CDC &dc, const CRect &rc, BOOL bUp/* = TRUE*/, int nBeginTransparent/* = 50*/, int nEndTransparent/* = 90*/)
{
	int nWidth = rc.Width();
	int nHeight = rc.Height();
	int nDesStartX = rc.left;
	int nDesStartY = rc.top;	

	CDC TempDC;
	TempDC.CreateCompatibleDC(&dc);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, nWidth, nHeight);
	CBitmap *pOldmap =  TempDC.SelectObject(&memBmp);

	TempDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(0, 0, 0));
	TempDC.FillSolidRect(1, 1, nWidth - 2, nHeight - 2, RGB(255, 255, 255));

	// 中间
	CRect rcSrc;
	CRect rcDes;
	if(bUp)
	{
		rcSrc.SetRect(1, 1, nWidth - 1, nHeight / 2);
		rcDes.SetRect(nDesStartX + 1, nDesStartY + 1, nDesStartX + nWidth - 1, nDesStartY + nHeight / 2);
	}
	else
	{
		rcSrc.SetRect(1, nHeight / 2, nWidth - 1, nHeight - 1);
		rcDes.SetRect(nDesStartX + 1, nDesStartY + nHeight / 2, nDesStartX + nWidth - 1, nDesStartY + nHeight - 1);		
	}

	DrawVerticalTransition(dc, TempDC, rcDes, rcSrc, nBeginTransparent, nEndTransparent);

	// 边框
	BLENDFUNCTION blend;
	memset( &blend, 0, sizeof( blend) );
	blend.BlendOp= AC_SRC_OVER;
	blend.SourceConstantAlpha = 100;
	// 左
	dc.AlphaBlend(nDesStartX, nDesStartY + 1, 2, nHeight - 2, &TempDC, 0, 1, 2, nHeight - 2, blend);
	// 上
	dc.AlphaBlend(nDesStartX + 1, nDesStartY, nWidth - 2, 2, &TempDC, 1, 0, nWidth - 2, 2, blend);
	// 右
	dc.AlphaBlend(nDesStartX + nWidth - 2, nDesStartY + 1, 2, nHeight - 2, &TempDC, nWidth - 2, 1, 2, nHeight - 2, blend);
	// 下
	dc.AlphaBlend(nDesStartX + 1, rc.bottom - 2, nWidth - 2, 2, &TempDC, 1, nHeight - 2, nWidth - 2, 2, blend);

	blend.SourceConstantAlpha = 12;
	// 左上
	dc.AlphaBlend(nDesStartX, nDesStartY, 2, 2, &TempDC, 0, 0, 1, 1, blend);
	// 右上
	dc.AlphaBlend(nDesStartX + nWidth, nDesStartY, 2, 2, &TempDC, 0, 0, 1, 1, blend);
	// 右下
	dc.AlphaBlend(nDesStartX + nWidth, nDesStartY + nHeight, 2, 2, &TempDC, 0, 0, 1, 1, blend);
	// 左下
	dc.AlphaBlend(nDesStartX , nDesStartY + nHeight, 2, 2, &TempDC, 0, 0, 1, 1, blend);

	TempDC.SelectObject(pOldmap);
	memBmp.DeleteObject();
	TempDC.DeleteDC();
}

// 创建位图,替代CreateCompatibleBitmap函数,用于解决GDI+ 16位色下UpdateLayeredWindow失败
// 一般情况下用CreateCompatibleBitmap来创建,但创建的Bitmap是和DC相关的,如果当前操作系统是16位色模式,
// 就会丢失了Alpha通道导致变成全透明的,无法正确显示
// 注意:创建的位图需要使用DeleteObject来释放
HBITMAP	DuiCreateCompatibleBitmap(CRect& rcClient)
{
	BYTE                      * pBits ; 
	BITMAPINFOHEADER          bmih; 
	ZeroMemory( &bmih, sizeof(BITMAPINFOHEADER)); 

	bmih.biSize                 = sizeof (BITMAPINFOHEADER) ; 
	bmih.biWidth				= rcClient.Width(); 
	bmih.biHeight               = rcClient.Height(); 
	bmih.biPlanes               = 1 ; 
	bmih.biBitCount             = 32;  //这里一定要是32 
	bmih.biCompression          = BI_RGB ; 
	bmih.biSizeImage            = 0 ; 
	bmih.biXPelsPerMeter        = 0 ; 
	bmih.biYPelsPerMeter        = 0 ; 
	bmih.biClrUsed              = 0 ; 
	bmih.biClrImportant          = 0 ; 

	HBITMAP hBitMap = CreateDIBSection (NULL, (BITMAPINFO *)  &bmih, 0, (VOID**)&pBits, NULL, 0) ;
	return hBitMap;
}