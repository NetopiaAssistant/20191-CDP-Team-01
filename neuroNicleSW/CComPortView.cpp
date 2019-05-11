// CComPortView.cpp: 구현 파일
//

#include "stdafx.h"
#include "neuroNicleSW.h"
#include "CComPortView.h"
#include "afxdialogex.h"


// CComPortView 대화 상자

IMPLEMENT_DYNAMIC(CComPortView, CDialogEx)

CComPortView::CComPortView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

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
END_MESSAGE_MAP()


// CComPortView 메시지 처리기


void CComPortView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}


BOOL CComPortView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	//////////////////포토 탐색 구현부분//////////////////////
	int ComPortNum = 1;

	     ///원래는 ComPortNum 1에서부터 50 정도까지 for문을 돌려서 탐색하나
	    /// 코딩 편의상 현재 for문을 만들지 않고 컴퓨터 상에서 뉴로니클 시리얼 포트번호를 확인하 후
	   /// 값을 입력하여 돌려보고 있습니다. 저희집 컴퓨터 상에서는 뉴로니클이 12번 포트입니다.

			printf("\n\n\n////////////%d///////////////////%d/////////////////%d////////////////////////\n", ComPortNum, ComPortNum, ComPortNum);
			printf("Com Port %d 연결중입니다.\n", ComPortNum);
			CString ComPortName;

			/*
			if (ComPortNum < 0)
				ComPortName.Format(_T("COM%d"), ComPortNum);
			else
				ComPortName.Format(_T("\\\\.\\COM%d"), ComPortNum); //포트 번호가 10이상이 되면 \\\\.\\COM%d 이런 식으로 적어줘야 합니다.
				*/
			ComPortName.Format(_T("\\\\.\\COM%d"), 12);


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
				printf("Com port %d 연결하지 못했습니다 \n", ComPortNum);
				//continue; //다음포트 탐색
			}
			else
				printf("Com port %d 연결성공 하였습니다.\n", ComPortNum);


			printf("EV    Q    포트 비우기 설정 시작\n");
			// EV_RXCHAR event 설정
			SetCommMask(m_hComm, EV_BREAK | EV_ERR | EV_RXCHAR | EV_TXEMPTY);
			// InQueue, OutQueue 크기 설정.
			SetupComm(m_hComm, 4096, 4096);
			// 포트 비우기.
			PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

			printf("EV    Q    포트 비우기 설정 끝\n");

			DCB dcb = { 0 };
			dcb.DCBlength = sizeof(DCB);

			
			//data control blcok 설정
			if (!::GetCommState(m_hComm, &dcb))
			{
				TRACE("CSerialCommHelper : Failed to Get Com State Reason :  %d", GetLastError());
				//break;
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
			//	break;
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
				printf("Com Port %d 시간 설정이 잘못 되었습니다. \n", ComPortNum);
				//break;
			}
			else
				printf("시간 설정 완료\n");


			unsigned char* buf = new unsigned char[400];   // 통신에 사용될 버퍼
			memset(buf, 0, sizeof(buf) / sizeof(buf[0])); //버퍼 초기화
	        
			printf("버퍼 초기화 다음\n");

			for (int k = 0; k < 30; k++)
				printf("초기화 직후 buf[%d] = %u\n", k, buf[k]);
		

			for (; ComPortNum < 20; ComPortNum++)
			{

			DWORD toRead = 30;
			DWORD NumberOfBytesRead = 0;

			int T = 0;

			printf("to Read, NumberOfBytesRead 변수 초기화 \n");
			printf("Com Port 핸들값은 %d \n", m_hComm);

			COMSTAT comstat; //통신 장치의 현재 상태 구조체
			DWORD dwRead; //DWORD는 unsigned long 형태를 typedef한거
			DWORD dwErrorFlags;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);

			 
			
			printf("Com Port %d         comstat.cbInQue = %d\n", ComPortNum, comstat.cbInQue);
	
				for (int k = 0; k < 20; k++)
					printf("읽기 전 buf[%d] = %u\n", k, buf[k]);
			

			while (1)
			{
				dwRead = comstat.cbInQue;
				printf("for 문 안 앞 \n");
				//ReadFile 세번째 인자 값으로는 dwRead가 들어가야 맞으나 현재 comstat.cbInQue가 제대로 작동하지 않습니다.
				//그래서 20번씩 무조건 읽어라, 라는 의미에서 20을 할당해 놓았고
				//문제점은 포트 Q에 들어온 정보가 ReadFile에서 읽어야하는 값보다 적으면 ReadFile에서 프로그램이 멈춥니다. 
				ReadFile(m_hComm, buf, 20, &NumberOfBytesRead, NULL);// 읽어야 하는 수가 많으면 그 수 채울때 까지 스탑한다
				//그래서 dwRead는 버퍼에 든 저장바이트만큼 설정해야 스탑하지 않고 넘어간다.
				PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
				printf("ReadFile 호출\n");
				T++; //이 인자는 while구문을 적당히 돌려서 탐색을 무한히 하지 않도록 하기 위함입니다.
				ClearCommError(m_hComm, &dwErrorFlags, &comstat);
				printf("while문 안에서 Com Port %d   comstat.cbInQue = %d\n", ComPortNum, comstat.cbInQue);

				
					for (int k = 0; k < 20; k++)
						printf("buf[%d] = %u\n", k, buf[k]);
				

				for (DWORD i = 0; i < 60; i++)
				{
					//printf("for 문에 들어옴 \n");
					if (buf[(int)i] == 255 && buf[(int)i + 1] == 254) // Found the sync spot. 
					{
						printf("Com Port %d는 T2A 패킷입니다.", ComPortNum);
					}
				}

				printf("for 문 안 뒤\n");
				if (5 == T)
				{
					printf("Com Port %d는 T2A 패킷이 아닙니다.", ComPortNum);
					break;
				}


			}

			printf("while 밖 \n");

			


		}

		return TRUE;  // return TRUE unless you set the focus to a control
					  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
	
}
