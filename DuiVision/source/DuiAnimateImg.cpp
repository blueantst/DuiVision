#include "StdAfx.h"
#include "DuiAnimateImg.h"

inline int CDuiAnimateImage::TGIFControlExt::GetPackedValue(enum ControlExtValues Value)
{
	int nRet = (int)m_cPacked;
	switch (Value)
	{
	case GCX_PACKED_DISPOSAL:
		nRet = (nRet & 28) >> 2;
		break;

	case GCX_PACKED_USERINPUT:
		nRet = (nRet & 2) >> 1;
		break;

	case GCX_PACKED_TRANSPCOLOR:
		nRet &= 1;
		break;
	};

	return nRet;
}

inline int CDuiAnimateImage::TGIFLSDescriptor::GetPackedValue(enum LSDPackedValues Value)
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
	case LSD_PACKED_GLOBALCT:
		nRet = nRet >> 7;
		break;

	case LSD_PACKED_CRESOLUTION:
		nRet = ((nRet & 0x70) >> 4) + 1;
		break;

	case LSD_PACKED_SORT:
		nRet = (nRet & 8) >> 3;
		break;

	case LSD_PACKED_GLOBALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}

inline int CDuiAnimateImage::TGIFImageDescriptor::GetPackedValue(enum IDPackedValues Value)
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
	case ID_PACKED_LOCALCT:
		nRet >>= 7;
		break;

	case ID_PACKED_INTERLACE:
		nRet = ((nRet & 0x40) >> 6);
		break;

	case ID_PACKED_SORT:
		nRet = (nRet & 0x20) >> 5;
		break;

	case ID_PACKED_LOCALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}

CDuiAnimateImage::CDuiAnimateImage(HWND hWnd, CDuiObject* pDuiObject)
	: CControlBaseFont(hWnd, pDuiObject)
{
	m_bRunTime = false;
	m_nIndex = 0;
	m_nMaxIndex = 10;
	m_nCount = 0;
	m_nTimerCount = 5;
	SetBitmapCount(m_nMaxIndex);

	m_bIsGIF = FALSE;
	m_pGIFLSDescriptor	= NULL;
	m_pGIFHeader		= NULL;
	m_pGIFRawData		= NULL;
	m_clrGIFBackground	= RGB(255,255,255); // white by default
	m_nGlobalCTSize		= 0;
	m_nCurrOffset		= 0;
	m_nGIFDataSize			= 0;
}

CDuiAnimateImage::CDuiAnimateImage(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, 
				 UINT uAlignment/* = DT_CENTER*/, UINT uVAlignment/* = DT_VCENTER*/, BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/)
	: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, TEXT(""), bIsVisible, bIsDisable)
{
	m_bRunTime = false;
	m_nIndex = 0;
	m_nMaxIndex = 10;
	m_nCount = 0;
	m_nTimerCount = 5;
	SetBitmapCount(m_nMaxIndex);

	m_bIsGIF = FALSE;
	m_pGIFLSDescriptor	= NULL;
	m_pGIFHeader		= NULL;
	m_pGIFRawData		= NULL;
	m_clrGIFBackground	= RGB(255,255,255); // white by default
	m_nGlobalCTSize		= 0;
	m_nCurrOffset		= 0;
	m_nGIFDataSize			= 0;
}

CDuiAnimateImage::~CDuiAnimateImage(void)
{
	// 删除GIF帧
	std::vector<TFrame>::iterator it;
	for (it=m_arrGIFFrames.begin(); it<m_arrGIFFrames.end(); it++)
	{
		delete (*it).m_pPicture;
	}
	m_arrGIFFrames.clear();
}

// 从XML设置GIF图片信息属性
HRESULT CDuiAnimateImage::OnAttributeImageGIF(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(!SetImageGIF(strSkin))
	{
		return E_FAIL;
	}

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置图片个数属性
HRESULT CDuiAnimateImage::OnAttributeMaxIndex(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty() || (m_pImage == NULL)) return E_FAIL;

	m_nMaxIndex = _ttoi(strValue);
	if(m_bIsGIF && ((UINT)m_nMaxIndex > m_arrGIFFrames.size()))
	{
		m_nMaxIndex = m_arrGIFFrames.size();
	}

	SetBitmapCount(m_nMaxIndex);

	if(!m_bIsGIF)
	{
		m_sizeImage.SetSize(m_pImage->GetWidth() / m_nImagePicCount, m_pImage->GetHeight());
	}

	UpdateControl(true);

	return bLoading?S_FALSE:S_OK;
}

// 从XML设置动画属性
HRESULT CDuiAnimateImage::OnAttributeRun(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	BOOL bRun = (strValue == _T("1") ? TRUE : FALSE);
	SetRun(bRun);

	return bLoading?S_FALSE:S_OK;
}

BOOL CDuiAnimateImage::SetRun(BOOL bRun, int nIndex/* = -1*/)
{
	if(GetDisable())
	{
		return FALSE;
	}

	BOOL bOldRunTime = m_bRunTime;
	int nOldIndex = m_nIndex;
	m_bRunTime = bRun;

	if(nIndex >= 0 && nIndex < m_nMaxIndex)
	{
		m_nIndex = nIndex;
	}

	if(GetVisible() && ((bOldRunTime != m_bRunTime) || (nOldIndex != m_nIndex)))
	{
		UpdateControl();
	}

	return m_bRunTime;
}

BOOL CDuiAnimateImage::OnControlTimer()
{
	if(!m_bRunTime || !m_bIsVisible)
	{
		return FALSE;
	}

	if(++m_nCount >= m_nTimerCount)
	{
		m_nCount = 0;
		if(++m_nIndex >= m_nMaxIndex)
		{
			m_nIndex = 0;
		}
		
		if(GetVisible())
		{
			UpdateControl();
		}

		if(m_bIsGIF)
		{
			// 如果是GIF动画图片,需要根据当前帧计算下一次的定时时间
			TFrame* pGIFFrame = &(m_arrGIFFrames.at(m_nIndex));
			if(pGIFFrame)
			{
				int nGIFFrameDelay = pGIFFrame->m_nDelay;
				if(nGIFFrameDelay > 1)
				{
					nGIFFrameDelay--;
				}
				m_nTimerCount = (nGIFFrameDelay / 3) +1;
			}
		}

		return true;
	}

	return false;
}

// 设置GIF图片
BOOL CDuiAnimateImage::SetImageGIF(CString strImage)
{
	if(strImage.Find(_T(".")) != -1)	// 加载图片文件
	{
		CString strImgFile = strImage;
		if(strImage.Find(_T(":")) != -1)
		{
			strImgFile = strImage;
		}
		if(!LoadGIFFile(strImgFile))
		{
			return FALSE;
		}
	}else	// 加载图片资源
	{
		// 暂不支持
	}

	return TRUE;
}

// 加载GIF图片
BOOL CDuiAnimateImage::LoadGIFFile(CString strImage)
{
	m_bIsGIF = FALSE;
	// 删除GIF帧
	std::vector<TFrame>::iterator it;
	for (it=m_arrGIFFrames.begin(); it<m_arrGIFFrames.end(); it++)
	{
		delete (*it).m_pPicture;
	}
	m_arrGIFFrames.clear();

	// 加载整个文件到内存
	BYTE* pBufferData = NULL;
	DWORD dwSize = 0;
	if(!DuiSystem::Instance()->LoadFileToBuffer(strImage, pBufferData, dwSize))
	{
		return FALSE;
	}

	// 解析GIF文件
	m_pGIFRawData = pBufferData;
	m_nGIFDataSize = dwSize;
	m_pGIFHeader = reinterpret_cast<TGIFHeader *>(pBufferData);
	SetBitmapCount(1);

	if ((memcmp(&m_pGIFHeader->m_cSignature,"GIF",3) != 0) &&
		((memcmp(&m_pGIFHeader->m_cVersion,"87a",3) != 0) ||
		 (memcmp(&m_pGIFHeader->m_cVersion,"89a",3) != 0)) )
	{
		// it's neither GIF87a nor GIF89a
		// do the default processing
		SetBitmap(strImage);
	}
	else
	{
		// it's a GIF
		m_bIsGIF = TRUE;
		m_pGIFLSDescriptor = reinterpret_cast<TGIFLSDescriptor *>
			(pBufferData + sizeof(TGIFHeader));
		if (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT) == 1)
		{
			// calculate the globat color table size
			m_nGlobalCTSize = static_cast<int>
				(3* (1 << (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE)+1)));
			// get the background color if GCT is present
			unsigned char *pBkClr = pBufferData + sizeof(TGIFHeader) + 
				sizeof(TGIFLSDescriptor) + 3*m_pGIFLSDescriptor->m_cBkIndex;
			m_clrGIFBackground = RGB(pBkClr[0],pBkClr[1],pBkClr[2]);
		};

		// store the picture's size
		m_sizeImage.cx = m_pGIFLSDescriptor->m_wWidth;
		m_sizeImage.cy = m_pGIFLSDescriptor->m_wHeight;

		// determine frame count for this picture
		UINT nFrameCount=0;
		ResetDataPointer();
		while (SkipNextGraphicBlock())
			nFrameCount++;

#ifdef GIF_TRACING
		TRACE(
			_T(" -= GIF encountered\n"
			   "Logical Screen dimensions = %dx%d\n"
			   "Global color table = %d\n"
			   "Color depth = %d\n"
			   "Sort flag = %d\n"
			   "Size of Global Color Table = %d\n"
			   "Background color index = %d\n"
			   "Pixel aspect ratio = %d\n"
			   "Frame count = %d\n"
			   "Background color = %06Xh\n\n"
			  ),
			m_pGIFLSDescriptor->m_wWidth,
			m_pGIFLSDescriptor->m_wHeight,
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_CRESOLUTION),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_SORT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE),
			m_pGIFLSDescriptor->m_cBkIndex,
			m_pGIFLSDescriptor->m_cPixelAspect,
			nFrameCount,
			m_clrGIFBackground
			);
		EnumGIFBlocks();
#endif

		if (nFrameCount == 0) // it's an empty GIF!
		{
			delete pBufferData;
			pBufferData = NULL;
			m_bIsGIF = FALSE;
			return FALSE;
		};

		// now check the frame count
		// if there's only one frame, no need to animate this GIF
		// therefore, treat it like any other pic
		if (nFrameCount == 1)
		{
			// clear GIF variables
			SetBitmap(strImage);
			m_bIsGIF = FALSE;
		}
		else
		{
			// if, on the contrary, there are several frames
			// then store separate frames in an array
			TFrame frame;
			UINT nBlockLen;
			HGLOBAL hFrameData;
			UINT nCurFrame = 0;
			int nGIFFrameDelay = 0;	// 第一帧的定时时间

			ResetDataPointer();
			while (hFrameData = GetNextGraphicBlock(&nBlockLen,
				&frame.m_nDelay, &frame.m_frameSize,
				&frame.m_frameOffset, &frame.m_nDisposal) )
			{
				char *pFrameData = reinterpret_cast<char*>(GlobalLock(hFrameData));
				BOOL bLoadFrmae = LoadImageFromMem((BYTE*)pFrameData, nBlockLen, m_bImageUseECM, frame.m_pPicture);

				#ifdef GIF_TRACING	// Trace log
				TRACE(
					_T("CDuiAnimateImage::LoadGIFFile %s, frame=%d, len=%d, load=%s"),
					strImage, nCurFrame, nBlockLen, bLoadFrmae ? _T("succ") : _T("fail"));
				#endif // GIF_TRACING

				GlobalUnlock( hFrameData );

				if(nCurFrame == 0)
				{
					nGIFFrameDelay = frame.m_nDelay;
				}
			
				// everything went well, add this frame
				m_arrGIFFrames.push_back(frame);
				nCurFrame++;
			};

			if (m_arrGIFFrames.empty()) // couldn't load any frames
			{
				delete pBufferData;
				pBufferData = NULL;
				m_bIsGIF = FALSE;
				return FALSE;
			}

			// 设置动画的最大帧
			m_nMaxIndex = nFrameCount;

			// 设置动画的定时时间
			// 1.根据GIF第一帧的定时时间初始化定时器
			// 2.DuiVision动画定时器默认30毫秒,GIF定时器单位为10毫秒,向上取整
			if(nGIFFrameDelay > 1)
			{
				nGIFFrameDelay--;
			}
			m_nTimerCount = (nGIFFrameDelay / 3) +1;
		};
	}; // if (!IsGIF...

	delete pBufferData;
	pBufferData = NULL;

	return TRUE;
}

void CDuiAnimateImage::ResetDataPointer()
{
	// skip header and logical screen descriptor
	m_nCurrOffset = 
		sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;
}

BOOL CDuiAnimateImage::SkipNextGraphicBlock()
{
	if (!m_pGIFRawData) return FALSE;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	enum GIFBlockTypes nBlock;

	nBlock = GetNextBlock();

	while ((nBlock != BLOCK_CONTROLEXT) &&
		   (nBlock != BLOCK_IMAGE) &&
		   (nBlock != BLOCK_PLAINTEXT) &&
		   (nBlock != BLOCK_UNKNOWN) &&
		   (nBlock != BLOCK_TRAILER) )
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return FALSE;

	// it's either a control ext.block, an image or a plain text

	if (GetNextBlockLen() <= 0) return FALSE;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		if (!SkipNextBlock()) return FALSE;
		nBlock = GetNextBlock();

		// skip everything until we meet an image block or a plain-text block
		while ((nBlock != BLOCK_IMAGE) &&
			   (nBlock != BLOCK_PLAINTEXT) &&
			   (nBlock != BLOCK_UNKNOWN) &&
			   (nBlock != BLOCK_TRAILER) )
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
		};

		if ((nBlock == BLOCK_UNKNOWN) ||
			(nBlock == BLOCK_TRAILER))
			return FALSE;
	};

	// skip the found data block (image or plain-text)
	if (!SkipNextBlock()) return FALSE;

	return TRUE;
}

UINT CDuiAnimateImage::GetSubBlocksLen(UINT nStartingOffset) const
{
	UINT nRet = 0;
	UINT nCurOffset = nStartingOffset;
	
	while (m_pGIFRawData[nCurOffset] != 0)
	{
		nRet += m_pGIFRawData[nCurOffset]+1;
		nCurOffset += m_pGIFRawData[nCurOffset]+1;
	};

	return nRet+1;
}

enum CDuiAnimateImage::GIFBlockTypes CDuiAnimateImage::GetNextBlock() const
{
	switch(m_pGIFRawData[m_nCurrOffset])
	{
	case 0x21:
	// extension block
		switch(m_pGIFRawData[m_nCurrOffset+1])
		{
		case 0x01:
		// plain text extension
			return BLOCK_PLAINTEXT;
			break;

		case 0xF9:
		// graphic control extension
			return BLOCK_CONTROLEXT;
			break;

		case 0xFE:
		// comment extension
			return BLOCK_COMMEXT;
			break;

		case 0xFF:
		// application extension
			return BLOCK_APPEXT;
			break;
		};
		break;
	
	case 0x3B:
	// trailer
		return BLOCK_TRAILER;
		break;

	case 0x2C:
	// image data
		return BLOCK_IMAGE;
		break;
	};

	return BLOCK_UNKNOWN;
}

BOOL CDuiAnimateImage::SkipNextBlock()
{
	if (!m_pGIFRawData) return FALSE;

	int nLen = GetNextBlockLen();
	if ((nLen <= 0) || ((m_nCurrOffset+nLen) > m_nGIFDataSize))
		return FALSE;

	m_nCurrOffset += nLen;
	return TRUE;
}

int CDuiAnimateImage::GetNextBlockLen() const
{
	GIFBlockTypes nBlock = GetNextBlock();

	int nTmp;

	switch(nBlock)
	{
	case BLOCK_UNKNOWN:
		return -1;
		break;

	case BLOCK_TRAILER:
		return 1;
		break;

	case BLOCK_APPEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFAppExtension));
		if (nTmp > 0)
			return sizeof(TGIFAppExtension)+nTmp;
		break;

	case BLOCK_COMMEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFCommentExt));
		if (nTmp > 0)
			return sizeof(TGIFCommentExt)+nTmp;
		break;

	case BLOCK_CONTROLEXT:
		return sizeof(TGIFControlExt);
		break;

	case BLOCK_PLAINTEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFPlainTextExt));
		if (nTmp > 0)
			return sizeof(TGIFPlainTextExt)+nTmp;
		break;

	case BLOCK_IMAGE:
		TGIFImageDescriptor *pIDescr = 
			reinterpret_cast<TGIFImageDescriptor *> (&m_pGIFRawData[m_nCurrOffset]);
		int nLCTSize = (int)
			(pIDescr->GetPackedValue(ID_PACKED_LOCALCT)*3*
			(1 << (pIDescr->GetPackedValue(ID_PACKED_LOCALCTSIZE)+1)));

		int nTmp = GetSubBlocksLen(m_nCurrOffset+
			sizeof(TGIFImageDescriptor) + nLCTSize + 1);
		if (nTmp > 0)
			return sizeof(TGIFImageDescriptor) + nLCTSize + 1 + nTmp;
		break;
	};

	return 0;
}

HGLOBAL CDuiAnimateImage::GetNextGraphicBlock(UINT *pBlockLen, 
	UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, 
	UINT *pDisposal)
{
	if (!m_pGIFRawData) return NULL;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	*pDisposal = 0;
	enum GIFBlockTypes nBlock;
	nBlock = GetNextBlock();

	while (
		(nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER)
		)
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return NULL;

	// it's either a control ext.block, an image or a plain text

	int nStart = m_nCurrOffset;
	int nBlockLen = GetNextBlockLen();

	if (nBlockLen <= 0) return NULL;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		// get the following data
		TGIFControlExt *pControl = 
			reinterpret_cast<TGIFControlExt *> (&m_pGIFRawData[m_nCurrOffset]);
		// store delay time
		*pDelay = pControl->m_wDelayTime;
		// store disposal method
		*pDisposal = pControl->GetPackedValue(GCX_PACKED_DISPOSAL);

		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
		
		// skip everything until we find data to display 
		// (image block or plain-text block)
		
		while (
			(nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER)
			)
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
			nBlockLen += GetNextBlockLen();
		};

		if ((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
			return NULL;
		nBlockLen += GetNextBlockLen();
	}
	else
		*pDelay = -1; // to indicate that there was no delay value

	if (nBlock == BLOCK_IMAGE)
	{
		// store size and offsets
		TGIFImageDescriptor *pImage = 
			reinterpret_cast<TGIFImageDescriptor *> (&m_pGIFRawData[m_nCurrOffset]);
		pBlockSize->cx = pImage->m_wWidth;
		pBlockSize->cy = pImage->m_wHeight;
		pBlockOffset->cx = pImage->m_wLeftPos;
		pBlockOffset->cy = pImage->m_wTopPos;
	};

	if (!SkipNextBlock()) return NULL;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED,
		sizeof(TGIFHeader) +
		sizeof(TGIFLSDescriptor) +
		m_nGlobalCTSize +
		nBlockLen + 
		1);  // for the trailer

	if (!hGlobal) return NULL;

	int nOffset = 0; 

	// GMEM_FIXED means we get a pointer
	unsigned char *pGlobal = reinterpret_cast<unsigned char *> (hGlobal);

	CopyMemory(pGlobal, m_pGIFRawData, 
		sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize);
	nOffset += sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;

	CopyMemory(pGlobal + nOffset, &m_pGIFRawData[nStart], nBlockLen);
	nOffset += nBlockLen;

	pGlobal[nOffset] = 0x3B; // trailer
	nOffset++;

	*pBlockLen = nOffset;

	return hGlobal;
}

void CDuiAnimateImage::DrawControl(CDC &dc, CRect rcUpdate)
{
	int nWidth = m_rc.Width();
	int nHeight = m_rc.Height();

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth * m_nMaxIndex, nHeight);

		Graphics graphics(m_memDC);
		
		CPoint point = GetOriginPoint(nWidth, nHeight, m_sizeImage.cx, m_sizeImage.cy,
						GetGDIAlignment(m_uAlignment), GetGDIVAlignment(m_uVAlignment));

		for(int i = 0; i < m_nMaxIndex; i++)
		{
			m_memDC.BitBlt(i * nWidth, 0, nWidth, nHeight, &dc, m_rc.left ,m_rc.top, SRCCOPY);

			if(m_bIsGIF)
			{
				// GIF图片,画指定的帧图片
				TFrame* pGIFFrame = &(m_arrGIFFrames.at(i));
				if(pGIFFrame)
				{
					// 根据Disposal决定画图的方式
					//if((pGIFFrame->m_nDisposal == 2) || (pGIFFrame->m_nDisposal == 3))
					{
						// 正常画图
						graphics.DrawImage(pGIFFrame->m_pPicture,
							Rect(point.x + pGIFFrame->m_frameOffset.cx,
							point.y + pGIFFrame->m_frameOffset.cy,
							pGIFFrame->m_frameSize.cx, pGIFFrame->m_frameSize.cy),
							pGIFFrame->m_frameOffset.cx, pGIFFrame->m_frameOffset.cy,
							pGIFFrame->m_frameSize.cx, pGIFFrame->m_frameSize.cy, UnitPixel);
						//graphics.DrawImage(pGIFFrame->m_pPicture, Rect(point.x , point.y,  m_sizeImage.cx, m_sizeImage.cy),
						//	0, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					}
				}
			}else
			{
				// 非GIF,按照普通图片方式画图
				graphics.DrawImage(m_pImage, Rect(point.x , point.y,  m_sizeImage.cx, m_sizeImage.cy),
					i * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
			}

			point.x += nWidth;
		}
	}

	dc.BitBlt(m_rc.left,m_rc.top, m_rc.Width(), m_rc.Height(), &m_memDC, m_nIndex * nWidth, 0, SRCCOPY);
}