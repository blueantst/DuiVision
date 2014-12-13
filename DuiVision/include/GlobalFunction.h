#pragma once

// 读取图片(从文件读)
BOOL LoadBitmapFromFile(const CString strPathFile, CBitmap &bitmap, CSize &size);
// 读取图片(从资源读)
BOOL LoadBitmapFromIDResource(UINT nID, CBitmap &bitmap, CSize &size, CString strType);
// 读取图片(从内存中加载)
BOOL LoadBitmapFromMem(BYTE* pByte, DWORD dwSize, CBitmap &bitmap, CSize &size);
// 读取图标(从文件读)
BOOL LoadIconFromFile(CString strPathFile, HICON &hIcon);
// 读取图标(从资源读)
BOOL LoadIconFromIDResource(UINT nID, HICON &hIcon);
// 读取图标(从内存中加载)
BOOL LoadIconFromMem(BYTE* pByte, DWORD dwSize, HICON &hIcon);
// 加载图片文件到内存中
BOOL LoadImageFromFile(CString strFile, BOOL useEmbeddedColorManagement, Image*& pImg);
// 从资源中加载图片
BOOL LoadImageFromIDResource(UINT nID, CString strType, BOOL useEmbeddedColorManagement, Image*& pImg);
// 从内存中加载图片文件
BOOL LoadImageFromMem(BYTE* pByte, DWORD dwSize, BOOL useEmbeddedColorManagement, Image*& pImg);

// 取得图片平均颜色
BOOL GetAverageColor(CDC *pDC, CBitmap &bitmap, const CSize &sizeImage, COLORREF &clrImage);
// 取得图片大小
BOOL GetSize(CBitmap &bitmap, CSize &size);
// 取得字体大小
Size GetTextBounds(const Gdiplus::Font& font,const StringFormat& strFormat,const CString& strText);
// 取得字体大小
Size GetTextBounds(const Gdiplus::Font& font,const StringFormat& strFormat, int nWidth, const CString& strText);
// 取得字体大小
Size GetTextBounds(const Gdiplus::Font& font,const CString& strText);
// 取得位置
CPoint GetOriginPoint(int nWidth, int nHeight, int nChildWidth, int nChildHeight, UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER);
// 取得位置
CPoint GetOriginPoint(CRect rc, int nChildWidth, int nChildHeight, UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER);
// 转换数字
CString DecimalFormat(int nNumber);


// 绘画函数

// 画垂直过渡
int DrawVerticalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent = 0, int nEndTransparent = 100);
// 画水平过渡
int DrawHorizontalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent = 0, int nEndTransparent = 100);
// 画右下角过渡
void DrawRightBottomTransition(CDC &dc, CDC &dcTemp, CRect rc, const int nOverRegio, const COLORREF clrBackground);
// 画图片边框
void DrawImageFrame(Graphics &graphics, Image *pImage, const CRect &rcControl, int nX, int nY, int nW, int nH, int nFrameSide = 4);
// 画图片边框(可指定九宫格的位置,nX/nY是原图的左上角,nWH/nHL是左上角点坐标,nWR/nHR是右下角点坐标)
void DrawImageFrameMID(Graphics &graphics, Image *pImage, const CRect &rcControl, int nX, int nY, int nW, int nH, int nWL, int nHL, int nWR, int nHR);
// 画过渡圆角矩形
void DrawRectangle(CDC &dcDes, const CRect &rcDes, BOOL bUp = TRUE, int nBeginTransparent = 60, int nEndTransparent = 90);

// 创建位图
HBITMAP	DuiCreateCompatibleBitmap(CRect& rcClient);
