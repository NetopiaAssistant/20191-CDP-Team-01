
// neuroNicleSWView.h: CneuroNicleSWView 클래스의 인터페이스
//

#pragma once
#define STOP 0
#define RUNNING 1
#define PAUSE 2

#include "neuroNicleSWDoc.h"
#include "CComPortView.h"
#include "sqlite3.h"

class CComPortView;

class CneuroNicleSWView : public CView
{
protected: // serialization에서만 만들어집니다.
	CneuroNicleSWView();
	DECLARE_DYNCREATE(CneuroNicleSWView)

// 특성입니다.
public:
	
	CneuroNicleSWDoc* GetDocument() const;
	CComPortView *  is_dlg1 = NULL; //dlg 활성화 검사 //dlg1이 할당되면, 여기에 접근해서 dlg1의 변수 버퍼값을 읽어오면 됩니다.
	
// 작업입니다.
	//C D스레드 포인터
	CWinThread* m_C_Thread = NULL;
	//C D스레드 함수
public:
	static UINT Thread_C_Function(LPVOID object); //좌뇌 우뇌 뇌파 그래프를 그린다.
	//C 스레드 워킹 타입
	int m_C_ThreadWorkType = 0;
	
	//D스레드 정보처리 여부 
	int is_D = 0; //1이되면 정보처리 완료  //0이되면 정보처리 안 됨 //B스레드에서 is_D조건을 보는 경우는 D스레드가 생성된 경우에 한함

	//저장 여부 조건
	int is_Save = 0; //1이 되면 세이브 시작
	int end_Save = 0; //1이 되면 세이브 정지
	int file_path_check = 0; // 1이 되면 파일 경로가 지정됨
	int is_bito0 = 1;
	int y_function(double A, int upPoint); //A는 뇌파값 //upPoint는 상자 상단 끝점 y좌표
	

	//Power specturm
	int m_ch1_isFull = 0;
	int m_ch2_isFull = 0;
	double Power_Spectrum_CH1[129];
	double Power_Spectrum_CH2[129];
	unsigned char pud0;
	int n = 1;
	int m_Power_Data = 0;
	int getAbit(unsigned char x, int n);
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
	afx_msg void OnDevicedisconnect();
	afx_msg void OnStartstream();
	afx_msg void OnUpdateStartstream(CCmdUI *pCmdUI);
	afx_msg void OnStopstream();
	afx_msg void OnUpdateStopstream(CCmdUI *pCmdUI);
	afx_msg void OnTextSave();
	afx_msg void OnTextSaveEnd();
	afx_msg void OnLoadDB();
};

#ifndef _DEBUG  // neuroNicleSWView.cpp의 디버그 버전
inline CneuroNicleSWDoc* CneuroNicleSWView::GetDocument() const
   { return reinterpret_cast<CneuroNicleSWDoc*>(m_pDocument); }
#endif

