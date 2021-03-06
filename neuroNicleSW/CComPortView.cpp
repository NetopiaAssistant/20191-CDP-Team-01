// CComPortView.cpp: 구현 파일
//

#include "stdafx.h"
#include "neuroNicleSW.h"
#include "CComPortView.h"
#include "afxdialogex.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include "sqlite3.h"

// CComPortView 대화 상자


IMPLEMENT_DYNAMIC(CComPortView, CDialogEx)

CComPortView::CComPortView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

	memset(stA_Buf, 0, sizeof(stA_Buf));

}

CComPortView::~CComPortView()
{
}

void CComPortView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CComPortView, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCANCEL, &CComPortView::OnBnClickedCancel)
END_MESSAGE_MAP()


// CComPortView 메시지 처리기


void CComPortView::OnPaint()
{
	CString string;



	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	if (m_PortNum == 0)
		dc.TextOut(70, 100, _T("아래 칸에 기기가 연결된 포트번호를 입력하세요."));

	else
	{
		string.Format(_T("COM%d에 연결합니다."), m_PortNum);
		dc.TextOut(100, 100, string);
	}
}


BOOL CComPortView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.





	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.

}



double CComPortView::ToDouble(unsigned char A, unsigned char B)
{
	return (((double)A) * 256 + (double)B);
}


//test
double Power_Spectrum1(unsigned char A) {
	return (double)A;
}

double Power_Spectrum2(unsigned char B) {
	return (double)B;
}


void CComPortView::OnOK() // 엔터 이벤트 받아서 시작하는 함수
{

	m_PortNum = GetDlgItemInt(IDC_INPUT_PORTNUM);
	Invalidate(); //포트 정보 받고 한번더 페인트 호출

	///A 스레드 실행///
	if (m_A_Thread == NULL) //A 스레드 할당되지 않은 경우
	{
		m_A_Thread = AfxBeginThread(Thread_A_Function, this, 0, 0, 0);

		if (m_A_Thread == NULL)
		{
			printf("A 스래드가 생성하는 중 에러가 발생하였습니다.\n");
		}
		else
		{
			m_A_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
			m_A_ThreadWorkType = RUNNING;

		}
	}
	else //스레드 재시작
	{
		m_A_Thread->ResumeThread();
		if (m_A_Thread == NULL)
		{
			printf("A 스래드가 재시작하는 중 에러가 발생하였습니다.\n");
		}
		else
		{
			m_A_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
			m_A_ThreadWorkType = RUNNING;

		}

	}


	///B 스레드 실행////
	if (m_A_Thread != NULL)
	{
		if (m_B_Thread == NULL) //B 스레드 할당되지 않은 경우
		{
			m_B_Thread = AfxBeginThread(Thread_B_Function, this, 0, 0, 0);

			if (m_B_Thread == NULL)
			{
				printf("B 스래드가 생성하는 중 에러가 발생하였습니다.\n");
			}
			else
			{
				m_B_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
				m_B_ThreadWorkType = RUNNING;

			}
		}
		else //스레드 재시작
		{
			m_B_Thread->ResumeThread();
			if (m_B_Thread == NULL)
			{
				printf("B 스래드가 재시작하는 중 에러가 발생하였습니다.\n");
			}
			else
			{
				m_B_Thread->m_bAutoDelete = FALSE; //개체 자동삭제 방지
				m_B_ThreadWorkType = RUNNING;

			}

		}
	}



	//CDialog::OnOK();
}

static sqlite3* db = NULL;
static UINT WriteDataToDb(int PPD, int PUD0, int PC, int PUD1, int PCD, int PUD2, int CH1, int CH2, int CH3, int CH4, int CH5, int CH6)
// DB에 데이터 저장
{
	
	char *zErrMsg = 0;
	int rc;

	if (!db)
	{
		rc = sqlite3_open("C:\\Users\\Public\\Downloads\\neuro_data.idx", &db);
		if (rc) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			return 0;
		}
		//else {
		//	fprintf(stdout, "Opened database successfully\n");
		//}

	}
	//파일열기


	// 데이터 입력
	char sql[10000] = { 0, };
	sprintf(sql, "insert into neuro_data(ppd, pud0, pc, pud1, pcd, pud2, ch1, ch2, ch3, ch4, ch5, ch6) values (%d, %d, %d, %d, %d, %d,%d, %d, %d, %d, %d, %d)",
		PPD, PUD0, PC, PUD1, PCD, PUD2, CH1, CH2, CH3, CH4, CH5, CH6);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	//else {
	//	fprintf(stdout, "Records created successfully\n");
	//}

	return 0;
}

static UINT Thread_A_Function(LPVOID object)
{

	printf("A 쓰레드 시작되었습니다. \n");

	///////변수 영역////////////
	CComPortView* pDlg = (CComPortView*)object;
	CString ComPortName;
	int PortNum = pDlg->m_PortNum;
	///////////
	unsigned char Tbuf = 0;   // 1Byte 씩 받아오기 위한 임시 버퍼
	memset(pDlg->stA_Buf, 0, sizeof(pDlg->stA_Buf) / sizeof(unsigned char)); //버퍼 초기화
																			 //////// T2A 패킷 확인 변수
	int is_255 = 0;
	int is_254 = 0; //T2A 패킷 확인 변수
	int is_continue = 0; // 255 바로 다음 바이트인가?
						 ////////변수 영역 끝////////////

	int cycle = 1; //디버그용

						 ///포트 이름 형성
	if (0 != PortNum)
	{
		int PortNum = pDlg->m_PortNum;


		if (PortNum < 10)
			ComPortName.Format(_T("COM%d"), PortNum);
		else
			ComPortName.Format(_T("\\\\.\\COM%d"), PortNum); //포트 번호가 10이상이 되면 \\\\.\\COM%d 이런 식으로 적어줘야 합니다.
	}
	else
	{
		printf("포트 번호가 입력되지 않았습니다. \n");
	}

	///////////포트 할당 및 설정//////////////////
	HANDLE m_hComm;

	//포트 핸들러 할당
	m_hComm = CreateFile(ComPortName,
		GENERIC_READ | GENERIC_WRITE,             // 접근모드 읽기쓰기인가 읽기 전용인가
		0,                                                           // 다른 프로그램과 공유를 할것인가 아닌가.
		NULL,                                                       // 보안에 관한 속성
		OPEN_EXISTING,                                        // 어떻게 열것인가

		0,                                                             // 파일 속성(보통 파일이면서 overlapped 되게 한다.)
		NULL);                                                      // 템플릿 파일 핸들러


	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		printf("Com port %d 연결하지 못했습니다 \n", PortNum);
		pDlg->m_A_ThreadWorkType = PAUSE;
		return 0; //스레드를 빠져나온다.
	}
	else
		printf("Com port %d 연결성공 하였습니다.\n", PortNum);



	// EV_RXCHAR event 설정
	SetCommMask(m_hComm, EV_BREAK | EV_ERR | EV_RXCHAR | EV_TXEMPTY);
	// InQueue, OutQueue 크기 설정.
	SetupComm(m_hComm, 4096, 4096);
	printf("event Q 설정 끝\n");

	// 포트 비우기.
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);


	DCB dcb = { 0 };
	dcb.DCBlength = sizeof(DCB);

	//data control blcok 설정
	if (!::GetCommState(m_hComm, &dcb))
	{
		TRACE("CSerialCommHelper : Failed to Get Com State Reason :  %d", GetLastError());
		//pDlg->m_A_ThreadWorkType = PAUSE;
		return 0; //스레드를 빠져나온다
	}
	dcb.BaudRate = CBR_115200;      //BaudRate 비트/초
	dcb.ByteSize = 8;               //데이터비트
	dcb.Parity = NOPARITY;          // 패리티
	dcb.StopBits = ONESTOPBIT;      // 정지비트
	dcb.fParity = FALSE;
	dcb.fNull = FALSE;

	if (!::SetCommState(m_hComm, &dcb))
	{
		ASSERT(0);
		TRACE("CSerialCommHelper : Failed to Set Comm State Reason:	%d", GetLastError());
		//	pDlg->m_A_ThreadWorkType = PAUSE;
		return 0; //스레드를 빠져나온다
	}
	else
		printf("data control block 설정 완료\n");


	//시간 설정
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(m_hComm, &timeouts))
	{
		printf("Com Port %d 시간 설정이 잘못 되었습니다. \n", PortNum);
		//pDlg->m_A_ThreadWorkType = PAUSE;
		return 0; //스레드를 빠져나온다.
	}
	else
		printf("시간 설정 완료\n");

	///////////////////// 포트 설정 완료////////////////////////////
	DWORD NumberOfBytesRead = 0; //읽은 바이트 수 
	int total_Byte = 0; // A 버퍼로 넘긴 파이트 수 


	while (pDlg->m_A_ThreadWorkType == RUNNING) //스레드가 러닝 타입인 경우 계속해서 돌아갈 것
	{
		if (pDlg->m_stA_isFull == 0) //A 버퍼가 비면 다시 읽어오기 시작한다.
		{

			ReadFile(m_hComm, &Tbuf, 1, &NumberOfBytesRead, NULL);
			printf("1byte 읽었습니다. %d\n", Tbuf);
			printf("A is Full %d \n", pDlg->m_stA_isFull);
			if (is_255 == 0 && is_254 == 0) //T2A 패킷이라는 것이 확인이 안 되었을 경우
			{
				if (Tbuf == 255)
				{
					//printf("255 확인하였습니다. \n");
					is_255 = 1; //check1    ->  생태 is_255 = 1      is_254 = 0
					is_continue = 1;
					printf("total_Byte = %d\n", total_Byte);
					pDlg->stA_Buf[total_Byte++] = Tbuf;

				}

			}
			else if (is_255 == 1 && is_254 == 0)
			{
				if (is_continue == 1) // 255 다음인 경우
				{
					if (Tbuf == 254)
					{
						//printf("연속해서 254 확인하였습니다. \n");
						is_254 = 1; //check2    ->  생태 is_255 = 1      is_254 = 1
						printf("total_Byte = %d\n", total_Byte);
						pDlg->stA_Buf[total_Byte++] = Tbuf;

					}
					else
					{
						is_255 = 0; // 상태 초기화
						is_continue = 0;
						total_Byte = 0;
					}
				}
			}
			else //is_255 == 1 && is_254 == 1
			{
				if (total_Byte < 20)
				{
					printf("total_Byte = %d\n", total_Byte);
					pDlg->stA_Buf[total_Byte++] = Tbuf;

					if (total_Byte == 20) //<- 여기서 초기화 됨 아래쪽 else 보험
					{
						is_255 = 0; // 상태 초기화
						is_254 = 0; // 상태 초기화
						is_continue = 0;
						total_Byte = 0;
						//가득 찬 상태
						pDlg->m_stA_isFull = 1; //A buf가 가득찼다는 뜻이고 이 값을 0으로 만들어주는 것은 B Thread 에서 정보를 읽어줄 때 한다.

						printf("cycle = %d 번째 입니다. \n", cycle++);

						// DB에 데이터 저장
						int PPD = (int)pDlg->stA_Buf[2];
						int PUD0 = (int)pDlg->stA_Buf[3];
						int PC = (int)pDlg->stA_Buf[4];
						int PUD1 = (int)pDlg->stA_Buf[5];
						int PCD = (int)pDlg->stA_Buf[6];
						int PUD2 = (int)pDlg->stA_Buf[7];
						int CH1 = pDlg->ToDouble(pDlg->stA_Buf[8], pDlg->stA_Buf[9]);
						int CH2 = pDlg->ToDouble(pDlg->stA_Buf[10], pDlg->stA_Buf[11]);
						int CH3 = pDlg->ToDouble(pDlg->stA_Buf[12], pDlg->stA_Buf[13]);
						int CH4 = pDlg->ToDouble(pDlg->stA_Buf[14], pDlg->stA_Buf[15]);
						int CH5 = pDlg->ToDouble(pDlg->stA_Buf[16], pDlg->stA_Buf[17]);
						int CH6 = pDlg->ToDouble(pDlg->stA_Buf[18], pDlg->stA_Buf[19]);
						WriteDataToDb(PPD, PUD0, PC, PUD1, PCD, PUD2, CH1, CH2, CH3, CH4, CH5, CH6);
					}
				}
				else
				{
					is_255 = 0; // 상태 초기화
					is_254 = 0; // 상태 초기화
					is_continue = 0;
					total_Byte = 0;
					//가득 찬 상태
					pDlg->m_stA_isFull = 1; //A buf가 가득찼다는 뜻이고 이 값을 0으로 만들어주는 것은 B Thread 에서 정보를 읽어줄 때 한다.
					
					// DB에 데이터 저장
					int PPD = (int)pDlg->stA_Buf[2];
					int PUD0 = (int)pDlg->stA_Buf[3];
					int PC = (int)pDlg->stA_Buf[4];
					int PUD1 = (int)pDlg->stA_Buf[5];
					int PCD = (int)pDlg->stA_Buf[6];
					int PUD2 = (int)pDlg->stA_Buf[7];
					int CH1 = pDlg->ToDouble(pDlg->stA_Buf[8], pDlg->stA_Buf[9]);
					int CH2 = pDlg->ToDouble(pDlg->stA_Buf[10], pDlg->stA_Buf[11]);
					int CH3 = pDlg->ToDouble(pDlg->stA_Buf[12], pDlg->stA_Buf[13]);
					int CH4 = pDlg->ToDouble(pDlg->stA_Buf[14], pDlg->stA_Buf[15]);
					int CH5 = pDlg->ToDouble(pDlg->stA_Buf[16], pDlg->stA_Buf[17]);
					int CH6 = pDlg->ToDouble(pDlg->stA_Buf[18], pDlg->stA_Buf[19]);
					WriteDataToDb(PPD, PUD0, PC, PUD1, PCD, PUD2, CH1, CH2, CH3, CH4, CH5, CH6);

				}
			}
		}
	}// while end
	if (db)
		sqlite3_close(db); // 파일 닫기

	return 0;
}


static UINT Thread_B_Function(LPVOID object)
{
	int cycle = 1; //디버그용
	stDataAbs_nnAPI* FX2_Stream_Data;
	printf("B 쓰레드 시작되었습니다. \n");

	CComPortView* pDlg = (CComPortView*)object;
	FX2_Stream_Data = &(pDlg->stB_Buf); //B 버퍼 주소값을 받는다.
	unsigned char* Abuf = pDlg->stA_Buf;

	FX2_Stream_Data->NumX = pDlg->m_SampleNum; //우뇌파 셈플링할 수 입력
	FX2_Stream_Data->NumY = pDlg->m_SampleNum; //좌뇌파 셈플링할 수 입력
	FX2_Stream_Data->Ad_X = (double*)malloc(sizeof(double)*pDlg->m_SampleNum); //셈플 수 만큼 메모리 할당 //32
	FX2_Stream_Data->Ad_Y = (double*)malloc(sizeof(double)*pDlg->m_SampleNum); //셈플 수 만큼 메모리 할당 //32

	unsigned int total_packet = 0; //A로부터 받은 패킷 셈플링 수만큼 채우고 다시 초기화
	int L = 0; //좌뇌파 데이터 index 값
	int R = 0; //우뇌파 데이터 index 값
	int temp = 1;
	while (pDlg->m_A_ThreadWorkType == RUNNING) //A 스레드가 돌아가야함 B 스레드 단독은 의미없음
	{																						//A 스레드 상태 RUNNING은 A스레드가 존재한다는 것과 같은 의미	
		while (pDlg->m_B_ThreadWorkType == RUNNING)
		{
			while (pDlg->m_stB_isFull == 0) //B버퍼가 비었을 동안
			{
				if (pDlg->m_stA_isFull == 1) //A버퍼가 가득차면
				{
					printf("B 쓰레드 A로부터 packet 정보 가저옴  %d \n", cycle++);

					//A 버퍼에서 채널값을 가져와 넣는다. 
					FX2_Stream_Data->Ad_X[L++] = pDlg->ToDouble(Abuf[8], Abuf[9]); //더블형으로 바꿔서 스트림 구조체에 넣어준다.  index 8, 9 <좌뇌>
					FX2_Stream_Data->Ad_Y[R++] = pDlg->ToDouble(Abuf[10], Abuf[11]); //index 10, 11 <우뇌>

					printf("B쓰레드입니다\n");
					pDlg->pud0 = Abuf[3];

					printf("pud0 값 : %d\n", pDlg->pud0);

					printf("bit0 값 : %d\n", pDlg->getAbit(pDlg->pud0, 0));
					printf("n 값 %d\n", pDlg->n);
					printf("temp 값 %d\n", pDlg->temp);

					if ((pDlg->getAbit(pDlg->pud0, 0)) == 1) {

						pDlg->is_pud0 = 1;

					}

					if (pDlg->is_pud0 == 1) {

						printf("test2");
						if (pDlg->n > 256) {
							pDlg->m_ch1_isFull = 1;
							pDlg->m_ch2_isFull = 1;
						}

						else {
							if (pDlg->n <= 128)
								pDlg->Power_Spectrum_CH1[(pDlg->n)++] = ((pDlg->ToDouble(Abuf[12], Abuf[13])) / (double)10);
							else if (pDlg->n <= 256)
								pDlg->Power_Spectrum_CH2[((pDlg->n)++) - 128] = ((pDlg->ToDouble(Abuf[12], Abuf[13])) / (double)10);

						}

						if (pDlg->m_ch1_isFull == 1 && pDlg->m_ch2_isFull == 1) {

							//pDlg->m_stB_isFull = 1;
							/*pDlg->m_ch1_isFull = 0;
							pDlg->m_ch2_isFull = 0;*/
							/*pDlg->n = 1;*/
							/*pDlg->is_pud0 = 0;*/

							printf("test3");
							printf("******power specturm ch1 받은 정보를 나열합니다.********\n");
							for (unsigned int y = 1; y < 129; y++)
							{
								printf("ch_1[%d] = %.2f  ", y, pDlg->Power_Spectrum_CH1[y]);
								if (y % 2 == 1 && y != 0)
									printf("\n");
							}
							printf("*******power spectrum ch2 받은 정보를 나열합니다.********\n");
							for (unsigned int y = 1; y <= 128; y++)
							{
								printf("ch_2[%d] = %.2f  ", y, pDlg->Power_Spectrum_CH2[y]);
								if (y % 2 == 1 && y != 0)
									printf("\n");
							}

							
						}
					}



					pDlg->m_stA_isFull = 0; //A 버퍼를 비었음으로 바꾼다. A스레드 다시 동작

					total_packet++; //L R 값의 증가량과 사실 같다. 

					printf("total_packet = %d  L = %d  R = %d \n", total_packet, L, R); // 셈플 수량과 같은 index를 가지는 순간 다음 if문에 들어간다.

					//여기 아래쪽이 아니다.
					if (total_packet >= pDlg->m_SampleNum)  //셈플 수를 채울경우 
					{
						
						total_packet = 0; //초기화
						L = 0; //초기화
						R = 0;
						//printf 문은 디버깅용
						printf("*******좌뇌값 받은 정보를 나열합니다.********\n");
						for (unsigned int y = 0; y < pDlg->m_SampleNum; y++)
						{
							printf("Ad_X[%d] = %.2f  ", y, FX2_Stream_Data->Ad_X[y]);
							if (y % 2 == 1 && y != 0)
								printf("\n");
						}
						printf("*******우뇌값 받은 정보를 나열합니다.********\n");
						for (unsigned int y = 0; y < pDlg->m_SampleNum; y++)
						{
							printf("Ad_Y[%d] = %.2f  ", y, FX2_Stream_Data->Ad_Y[y]);
							if (y % 2 == 1 && y != 0)
								printf("\n");
						}
						pDlg->m_stB_isFull = 1; //B 버퍼 가득찬 상태로 변경
					}

				}
			}
		}
		//외부에서 B 버퍼값을 읽고 m_stB_isFull 값을 0으로 바꿔주면 된다. <-- 다음 개발자분 보세요 !!!!!!!!!!!


	}

	return 0;
}



BOOL CComPortView::PreTranslateMessage(MSG* pMsg) // esc키 들어올 경우 이벤트 조작
{
	DWORD A_RESULT, B_RESULT, C_RESULT, D_RESULT; //스레드 정지할 때 상태정보
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			if (m_A_Thread != NULL)
			{

				m_A_Thread->SuspendThread();
				::GetExitCodeThread(m_A_Thread->m_hThread, &A_RESULT);
				printf("스레드 A가 정지하였습니다. \n");
			}

			if (m_B_Thread != NULL)
			{

				m_B_Thread->SuspendThread();
				::GetExitCodeThread(m_B_Thread->m_hThread, &B_RESULT);
				printf("스레드 B가 정지하였습니다. \n");
			}


			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			CneuroNicleSWView* pView = (CneuroNicleSWView*)pFrame->GetActiveView();
			pView->is_dlg1 = NULL; //아마 닫기 버튼을 눌렀을 때 해당 Dlg객체 메모리가 반환되리라 예상됨



			if (pView->m_C_Thread != NULL)
			{
				pView->m_C_Thread->SuspendThread();
				::GetExitCodeThread(pView->m_C_Thread->m_hThread, &C_RESULT);
				printf("스레드 C가 정지하였습니다. \n");

				pView->m_C_Thread = NULL;
			}
			pView->m_C_ThreadWorkType = PAUSE;

			return TRUE;
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


void CComPortView::OnBnClickedCancel() //Device 연결 Dlg에서 취소버튼을 누르는 경우
{
	DWORD A_RESULT, B_RESULT; //스레드 정지할 때 상태정보
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CneuroNicleSWView* pView = (CneuroNicleSWView*)pFrame->GetActiveView();

	if (m_A_Thread != NULL)
	{

		m_A_Thread->SuspendThread();
		::GetExitCodeThread(m_A_Thread->m_hThread, &A_RESULT);
		printf("스레드 A가 정지하였습니다. \n");
	}

	if (m_B_Thread != NULL)
	{

		m_B_Thread->SuspendThread();
		::GetExitCodeThread(m_B_Thread->m_hThread, &B_RESULT);
		printf("스레드 B가 정지하였습니다. \n");
	}

	pView->is_dlg1 = NULL; // 객체와 연결되어있던 포인터 초기화


	if (pView->m_C_Thread != NULL)
	{
		pView->m_C_Thread->SuspendThread();
		::GetExitCodeThread(pView->m_C_Thread->m_hThread, &A_RESULT);
		printf("스레드 C가 정지하였습니다. \n");

		pView->m_C_Thread = NULL;
	}
	pView->m_C_ThreadWorkType = PAUSE;

	CDialogEx::OnCancel();
}

int CComPortView::getAbit(unsigned char x, int n) { // getbit()
	return (x & (1 << n)) >> n;
}
