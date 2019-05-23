
// neuroNicleSWView.h: CneuroNicleSWView 클래스의 인터페이스
//

#pragma once

#include "neuroNicleSWDoc.h"
#include "CComPortView.h"

class CComPortView;

class CneuroNicleSWView : public CView
{
protected: // serialization에서만 만들어집니다.
	CneuroNicleSWView();
	DECLARE_DYNCREATE(CneuroNicleSWView)

// 특성입니다.
public:
	
	CneuroNicleSWDoc* GetDocument() const;
	CComPortView *  is_dlg1 = NULL; //dlg 활성화 검사
	
// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CneuroNicleSWView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeviceconnect();
	afx_msg void OnUpdateDeviceconnect(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // neuroNicleSWView.cpp의 디버그 버전
inline CneuroNicleSWDoc* CneuroNicleSWView::GetDocument() const
   { return reinterpret_cast<CneuroNicleSWDoc*>(m_pDocument); }
#endif

