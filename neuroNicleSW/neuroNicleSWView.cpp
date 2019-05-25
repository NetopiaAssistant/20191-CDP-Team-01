
// neuroNicleSWView.cpp: CneuroNicleSWView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "neuroNicleSW.h"
#include <fstream>
#include <iostream>
using namespace::std;
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
	ON_COMMAND(ID_DeviceDisConnect, &CneuroNicleSWView::OnDevicedisconnect)
	ON_COMMAND(ID_StartStream, &CneuroNicleSWView::OnStartstream)
	ON_UPDATE_COMMAND_UI(ID_StartStream, &CneuroNicleSWView::OnUpdateStartstream)
	ON_COMMAND(ID_StopStream, &CneuroNicleSWView::OnStopstream)
	ON_UPDATE_COMMAND_UI(ID_StopStream, &CneuroNicleSWView::OnUpdateStopstream)
	ON_COMMAND(ID_TEXT_SAVE, &CneuroNicleSWView::OnTextSave)
	ON_COMMAND(ID_TEXT_SAVE_END, &CneuroNicleSWView::OnTextSaveEnd)
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

//Device 해제 버튼이 눌렸을 때 실행되는 함수
void CneuroNicleSWView::OnDevicedisconnect() 
{
	if (is_dlg1 != NULL) //dlg1이 생성된 상태라면
	{
		DWORD A_RESULT, B_RESULT; //스레드 정지할 때 상태정보
		if (is_dlg1->m_A_Thread != NULL)
		{

			is_dlg1->m_A_Thread->SuspendThread();
			::GetExitCodeThread(is_dlg1->m_A_Thread->m_hThread, &A_RESULT);
			printf("스레드 A가 정지하였습니다. \n");
		}

		if (is_dlg1->m_B_Thread != NULL)
		{

			is_dlg1->m_B_Thread->SuspendThread();
			::GetExitCodeThread(is_dlg1->m_B_Thread->m_hThread, &B_RESULT);
			printf("스레드 B가 정지하였습니다. \n");
		}

		delete is_dlg1;
		is_dlg1 = NULL;
	}
	else
		printf("Device 연결이 되어있지 않습니다. \n");

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CneuroNicleSWView::OnStartstream()
{
	///B 스레드 실행////
	if (is_dlg1 != NULL)//dlg1 객체가 존재할 경우 //즉 스레드 A와 B의 부모객체가 존재할 경우 
	{
		if (is_dlg1->m_A_Thread != NULL) //A스레드 객체가 존재할 경우
		{
			if (is_dlg1->m_B_Thread != NULL) //B스레드 객체가 존재할 경우
			{



				////////////C 스레드 시작///////////////
				if (m_C_Thread == NULL) //C 스레드 할당되지 않은 경우
				{
					m_C_Thread = AfxBeginThread(Thread_C_Function, this, 0, 0, 0);

					if (m_C_Thread == NULL)
					{
						printf("C 스래드가 생성하는 중 에러가 발생하였습니다.\n");
					}
					else
					{
						m_C_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
						m_C_ThreadWorkType = RUNNING;

					}
				}
				else //C스레드가 존재할 경우 C 스레드 재시작
				{
					m_C_Thread->ResumeThread();
					if (m_C_Thread == NULL)
					{
						printf("C 스래드가 재시작하는 중 에러가 발생하였습니다.\n");
					}
					else
					{
						m_C_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
						m_C_ThreadWorkType = RUNNING;

					}
				}


					/////////D스레드 시작/////////////

					if (m_D_Thread == NULL) //D 스레드 할당되지 않은 경우
					{
						m_D_Thread = AfxBeginThread(Thread_D_Function, this, 0, 0, 0);

						if (m_D_Thread == NULL)
						{
							printf("D 스래드가 생성하는 중 에러가 발생하였습니다.\n");
						}
						else
						{
							m_D_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
							m_D_ThreadWorkType = RUNNING;

						}
					}
					else //C스레드가 존재할 경우 C 스레드 재시작
					{
						m_D_Thread->ResumeThread();
						if (m_D_Thread == NULL)
						{
							printf("D 스래드가 재시작하는 중 에러가 발생하였습니다.\n");
						}
						else
						{
							m_D_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
							m_D_ThreadWorkType = RUNNING;

						}
					}

			}// B스레드 객체가 존재할 경우 end
		}//A스레드 객체가 존재할 경우 end
	}//dlg1 객체가 존재할 경우 //즉 스레드 A와 B의 부모객체가 존재할 경우  end
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CneuroNicleSWView::OnUpdateStartstream(CCmdUI *pCmdUI)
{

	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CneuroNicleSWView::OnStopstream() //스레드 C와 D만 정지
{
	DWORD C_RESULT, D_RESULT; //스레드 정지할 때 상태정보
	if (m_C_Thread != NULL)
	{
		m_C_Thread->SuspendThread();
		::GetExitCodeThread(m_C_Thread->m_hThread, &C_RESULT);
		printf("스레드 C가 정지하였습니다. \n");

		m_C_Thread = NULL;
	}
	m_C_ThreadWorkType = PAUSE;

	if (m_D_Thread != NULL)
	{
		m_D_Thread->SuspendThread();
		::GetExitCodeThread(m_D_Thread->m_hThread, &D_RESULT);
		printf("스레드 D가 정지하였습니다. \n");

	  m_D_Thread = NULL;
	}
	 m_D_ThreadWorkType = PAUSE;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CneuroNicleSWView::OnUpdateStopstream(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}





/////////////////스레드 c 함수 구현/////////////////////
UINT CneuroNicleSWView::Thread_C_Function(LPVOID object)
{
	
	printf("C 쓰레드 시작되었습니다. \n");

	//더미파일
	/*
	stDataAbs_nnAPI temp_C_Buf;
	temp_C_Buf.Ad_X = (double*)malloc(sizeof(double) * 95); //메모리 할당
	for (int t = 0; t < 95; t++)
	{
		if(t%2 == 0)
		temp_C_Buf.Ad_X[t] = 16000 + 5000;
		else
			temp_C_Buf.Ad_X[t] = 16000 - 5000;
	}
	*/
	///////////////
	stDataAbs_nnAPI* temp_B_Buf;

	CneuroNicleSWView* pDlg = (CneuroNicleSWView*)object; //스레드를 생성한 객체 주소에 접근

	temp_B_Buf = &pDlg->is_dlg1->stB_Buf; //dlg1에 있는 B 버퍼의 포인터를 가져온다

	int SampleNum;

	SampleNum = pDlg->is_dlg1->m_SampleNum; // dlg1에 있던 SampleNum 수를 가져온다.
											//스레드 c는 한 번에 셈플링 수만큼 그래프를 그릴 것이다.
	
	while (pDlg->is_dlg1 != NULL) //dlg1이 존재할 경우
	{
		while (pDlg->is_dlg1->m_A_ThreadWorkType == RUNNING) //A 스레드가 돌아가야함
		{
			while (pDlg->is_dlg1->m_B_ThreadWorkType == RUNNING) //B 스레드가 돌아가야함
			{
				
				while (pDlg->m_C_ThreadWorkType == RUNNING) // C스레드가 워킹타입일 경우
				{
					//printf("C 쓰레드 돌아가는 중입니다. \n");
				
					if (pDlg->is_dlg1->m_stB_isFull == 1) //B버퍼가 가득 찼다면
					{
						HWND hWnd = pDlg->m_hWnd;
						HDC hdc = ::GetDC(hWnd);
						CDC dc;
						dc.Attach(hdc);



						CPoint points[32];
						CPen pen;
						pen.CreatePen(PS_SOLID, 1, RGB(200, 0, 0));    // 빨간색 펜 생성
						CPen* oldPen = dc.SelectObject(&pen);
						
						/*
						//A상자 좌표
						dc.MoveTo(84, 34);
						dc.LineTo(930, 34);         // 빨간색으로 선을 그림
						dc.LineTo(930, 160);
						dc.LineTo(84, 160);
						int y = 148;
						//B 상자 좌표
						dc.MoveTo(84, 34 + y);
						dc.LineTo(930, 34 + y);         // 빨간색으로 선을 그림
						dc.LineTo(930, 160 + y);
						dc.LineTo(84, 160 + y);
						*/

						
						////x좌표 계산///
						int interval = (930 - 84) / (SampleNum - 1); // SampleNum = 95
						///////////////////// 좌뇌 그래프 그리기
					   /*
					   dc.MoveTo(84, 97); //A상자 처음으로 이동
					   for (int k = 0; k < SampleNum; k++)
					   {
						   dc.LineTo(84+k*interval, pDlg->y_function(temp_B_Buf->Ad_X[k], 34));
					   }
					   */

						dc.MoveTo(84, 97); //A상자 처음으로 이동
						for (int k = 0; k < SampleNum; k++)
						{
							dc.LineTo(84 + k * interval, int(-(0.0041)*(temp_B_Buf->Ad_X[k] - 16000) + 34 + 63));
						}

						///////////////////// 우뇌 그래프 그리기


						dc.MoveTo(84, 245); //B상자 처음으로 이동
						for (int k = 0; k < 95; k++)
						{
							dc.LineTo(84 + k * interval, int(-(0.0041)*(temp_B_Buf->Ad_X[k] - 16000) + 182 + 63));
						}


						//자원 반환//
						dc.SelectObject(oldPen);
						pen.DeleteObject();
						::ReleaseDC(hWnd, hdc);
						dc.DeleteDC();


						pDlg->is_dlg1->m_stB_isFull == 0; // B 버퍼가 비었음 //스레드 B다시 동작시작 
					}//B버퍼가 가득 찼다면 end


				}//C 스레드 while end
				
			}// B 스레드 while end

		}//A스레드 while end
	}// dlg1이 존재할 경우 while end

	return 0;
}


///스레드 D 함수
//StartStream 버튼을 누르면 C스레드와 동시에 돌아가기 시작함
UINT CneuroNicleSWView::Thread_D_Function(LPVOID object)
{
	printf("D 쓰레드 시작되었습니다. \n");

	CneuroNicleSWView* pDlg = (CneuroNicleSWView*)object; //스레드를 생성한 객체 주소에 접근

	unsigned char* temp_A_Buf; //20바이트 포인터

	temp_A_Buf = pDlg->is_dlg1->stA_Buf; //A 버퍼 포인터를 가져온다.
	
	/* 더미파일 생성부분 디버그용
	temp_A_Buf = (unsigned char *)malloc(sizeof(unsigned char) * 20);
	for (int y = 0; y < 20; y++)
	{
		temp_A_Buf[y] = 10 * y;
		printf("temp_A_Buf[%d] = %u\n", y, temp_A_Buf[y]);
	}
	*/

	//temp_A_Buf 에서 T2A 정보처리하면 됩니다.

	CString path, filename; //파일 저장 경로
	CFileDialog dlg(false, "txt"); // true -> 불러오기 대화상자, false -> 저장 대화상자
	CFile file;
	int count = 0; //저장 횟수 제한

	while (pDlg->is_dlg1 != NULL) //dlg1이 존재할 경우
	{
		while (pDlg->is_dlg1->m_A_ThreadWorkType == RUNNING) //A 스레드가 돌아가야함
		{
			while (pDlg->m_D_ThreadWorkType == RUNNING) //D 스레드가 돌아가는 상태라면
			{
				///////////////////text 추출부분 시작/////////////////////////

				if (pDlg->is_dlg1->m_stA_isFull == 0)//A버퍼가 비었다면
					pDlg->is_D = 0; //D 정보 처리 상태 초기화

				if (pDlg->is_D == 0 && pDlg->is_dlg1->m_stA_isFull == 1) //A버퍼가 가득찼는데 정보 처리가 안 되었다면
				{                 

				 // printf("D스레드에서 정보처리 완료\n");

				  if (count < 500)
				  {
					
					  if (pDlg->is_Save == 1) //저장 버튼을 눌렀다면
					  {
						  if (pDlg->file_path_check == 0) //파일 경로가 지정되지 않았다면
						  {
							  count = 0; //count 0으로 초기화
							  if (dlg.DoModal() == IDOK) {
								  path = dlg.GetPathName(); //경로를 가져온다
								  filename = dlg.GetFileName(); //파일 이름을 가져온다. *.txt
								  printf("경로 -> %s\n", path);
								  printf("이름 -> %s\n", filename);
								  pDlg->file_path_check = 1; //경로 설정을 한 상태로 만든다.
								  /*
								  ofstream out(path, ios::app); // ios::app 이어쓰기 모드
								
									  out << "PPD" << " "; //2
									  out << "PUD0" << " ";
									  out << "PC" << " "; //4
									  out << "PUD1" << " ";
									  out << "PCD" << " "; //6
									  out << "PUD2" << " ";
									  out << "CH1_h" << " ";//8
									  out << "CH1_l" << " ";
									  out << "CH2_h" << " ";//10
									  out << "CH2_l" << " ";
									  out << "CH3_h" << " ";//12
									  out << "CH3_l" << " ";
									  out << "CH4_h" << " ";//14
									  out << "CH4_l" << " ";
									  out << "CH5_h" << " ";//16
									  out << "CH5_l" << " ";
									  out << "CH6_h" << " ";//18
									  out << "CH6_l" << " ";
									  out << "\n";
						
								  out.close();*/

							  }
							  else
								  printf("파일 경로 지정을 실패하였습니다. \n");
						  }

						  ofstream out(path,ios::app);
						  for (int i = 2; i < 20; i++) //index 0 1 은 제외
						  {
							  out << (float)temp_A_Buf[i] <<"  ";
							  if (i == 19)
								  out << "\n";
						  }
						  out.close();
						  count++;

					  }

				  }//count 500 if end

				  if (count >= 500)
				  {
					  //count = 0; //count 초기화
					  pDlg->file_path_check =0; //저장경로 지정이 안 된 상태로 초기화
					  pDlg->is_Save = 0; //자동으로 저장상태 0
				  }

					pDlg->is_D = 1; //정보처리 완료

				}// if is_D

				///////////////////text 추출부분 끝///////////////////////

				///////여기에 파워스펙트럼 구현////////

			}//D 스레드 while end

		}//A스레드 while end
	}

	return 0;
}

void CneuroNicleSWView::OnTextSave() //저장하기 버튼을 눌렀을 경우
{
	is_Save = 1; //1이 되면 세이브 시작
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CneuroNicleSWView::OnTextSaveEnd()//저장 멈추기 버튼을 눌렀을 경우
{
	is_Save = 0; //0이 되면 세이브 정지
	file_path_check = 0; //파일 경로 체크 초기화
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

int CneuroNicleSWView::y_function(double A, int upPoint) //A는 뇌파값 //upPoint는 상자 상단 끝점 y좌표
{
	//return ((-(126/ 30000)*((int)A - 16000)) + upPoint + 63);
	return 567;
}