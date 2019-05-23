
// neuroNicleSWView.cpp: CneuroNicleSWView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "neuroNicleSW.h"
#endif

#include "neuroNicleSWDoc.h"
#include "neuroNicleSWView.h"
#include "CComPortView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CneuroNicleSWView

IMPLEMENT_DYNCREATE(CneuroNicleSWView, CView)

BEGIN_MESSAGE_MAP(CneuroNicleSWView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_DeviceConnect, &CneuroNicleSWView::OnDeviceconnect)
	ON_UPDATE_COMMAND_UI(ID_DeviceConnect, &CneuroNicleSWView::OnUpdateDeviceconnect)
END_MESSAGE_MAP()

// CneuroNicleSWView 생성/소멸

CneuroNicleSWView::CneuroNicleSWView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CneuroNicleSWView::~CneuroNicleSWView()
{
}

BOOL CneuroNicleSWView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CneuroNicleSWView 그리기

void CneuroNicleSWView::OnDraw(CDC* pDC)
{
	CneuroNicleSWDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//그래프 출력 부분은 화면 포멧이 그려지고 난 뒤에 구현해주세요.
	
	////화면 포멧 시작///
	//비트맵 리소스를 로드한 후 크기 정보를 얻는다.
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP1);
	BITMAP bmpinfo;
	bitmap.GetBitmap(&bmpinfo);

	//메모리 DC를 만든 후 비트맵을 선택해 넣는다.
	CDC dcmem;
	dcmem.CreateCompatibleDC(pDC);
	dcmem.SelectObject(&bitmap);

	//비트맵을 화면에 출력한다.
	SetStretchBltMode(*pDC,  HALFTONE); //출력 보정
	pDC->StretchBlt(0, 0, bmpinfo.bmWidth/4, bmpinfo.bmHeight/4,  &dcmem,0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY); //출력
	///화면 포멧 끝///


	//사용한 자원 반환//
	DeleteObject(bitmap);
	DeleteDC(dcmem); //CreateCompatibleDC로 생성한 DC는 DeleteDC함수로 제거
	::ReleaseDC(m_hWnd, *pDC);
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CneuroNicleSWView 인쇄

BOOL CneuroNicleSWView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CneuroNicleSWView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CneuroNicleSWView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CneuroNicleSWView 진단

#ifdef _DEBUG
void CneuroNicleSWView::AssertValid() const
{
	CView::AssertValid();
}

void CneuroNicleSWView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CneuroNicleSWDoc* CneuroNicleSWView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CneuroNicleSWDoc)));
	return (CneuroNicleSWDoc*)m_pDocument;
}
#endif //_DEBUG


// CneuroNicleSWView 메시지 처리기


void CneuroNicleSWView::OnDeviceconnect()
{
	CComPortView * dlg;
	//dlg.DoModal(); -> //Modal에서 Modaless로 변경하였습니다.
	dlg = new CComPortView;
	dlg->Create(IDD_DIALOG1);
	dlg->ShowWindow(SW_SHOW);
	is_dlg1 = dlg; //포인터 전달
	//Dlg뜬 와중에도 메뉴 버튼 선택이 가능합니다.
}

//Device 연결 버튼을 눌렀을 때 회색으로 비활성화 되는 기능 (두 번 클릭 방지)
void CneuroNicleSWView::OnUpdateDeviceconnect(CCmdUI *pCmdUI)
{
	if(is_dlg1 != NULL) //창이 뜬 경우
		pCmdUI->Enable(0); //비활성하
	else
		pCmdUI->Enable(1); //활성화
	
	//is_dlg1 이 NULL이 아닌 경우, 즉 대화상자가 열린경우 버튼을 비활성화 한다.
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}
