#pragma once
#define STOP 0
#define RUNNING 1
#define PAUSE 2

#include "MainFrm.h"
#include "neuroNicleSWDoc.h"
#include "neuroNicleSWView.h"

// struct for data abstracted
typedef struct _LXDataAbs_nnAPI	// 응용프로그램용 구조체 정의
{
	unsigned int	NumAui;			// Aui[]의 배열크기.
	unsigned int	NumAd;			// Ad[]의 배열크기.
	unsigned int	NumX;
	unsigned int	NumY;
	unsigned int	NumZ;
	unsigned int*	Aui;			// Aui[NumAui]				
	unsigned int*	Aui_X; 
	unsigned int*	Aui_Y;
	unsigned int*	Aui_XY;
	unsigned int*	Aui_XYZ;
	double*			Ad;				// Ad[NumAd]
	double*			Ad_X;         //좌뇌 뇌파
	double*			Ad_Y;         //우뇌 뇌파
	double*			Ad_XY;       
	double*			Ad_XYZ;     //PPG  
	_LXDataAbs_nnAPI(){ 
		NumAui = 0;
		NumAd = 0;
		NumX = 0;
		NumY = 0;
		NumZ = 0;
		Aui = NULL;
		Aui_X = NULL;
		Aui_Y = NULL;
		Aui_XY = NULL;
		Aui_XYZ = NULL;
		Ad = NULL;
		Ad_X = NULL;
		Ad_Y = NULL;
		Ad_XY = NULL;
		Ad_XYZ = NULL;
	}
}stDataAbs_nnAPI;

// CComPortView 대화 상자

//스레드 함수 생성
static UINT Thread_A_Function(LPVOID object);
static UINT Thread_B_Function(LPVOID object);
// 쓰레드의 형태는 반드시 제어함수 static 선언해야 하며 리턴형태는 UINT해야 한다.
class CComPortView : public CDialogEx
{
	DECLARE_DYNAMIC(CComPortView)

public:
	CComPortView(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CComPortView();

	CWinThread *m_A_Thread = NULL;
	CWinThread *m_B_Thread = NULL;


	///현재 스레드 워킹 타입////
	int m_A_ThreadWorkType = 0;
	int m_B_ThreadWorkType = 0;

	///사용자가 입력하는 포트 넘버////
	int m_PortNum = 0;

	///구조체에 받아올 데이터 셈플링 수///
	unsigned int m_SampleNum = 95; //95 고정

	///Thread Buf///
	unsigned char stA_Buf[20];
	stDataAbs_nnAPI stB_Buf;  //포인터 타입이 들어간 구조체 변수이기에 Sampling 수에 따라 동적으로 메모리 할당

	

	////DataControler////
	int m_stA_isFull = 0; //버퍼 단위 usigned char 1Byte    //stB로 데이타 전달 단위  T2A 패킷 사이즈 (20byte)
	//0일 경우 stA 버퍼가 가득차지 않음 
	//1일 경우 stA 버퍼가 가득참 -> 0으로 바뀌지 않으면 stA는 포트로부터 바이트를 읽지 않음
	//0로 바꾸어주는 행위는 stB에서 stA의 버퍼 정보를 가저갈 경우 일어남
	int m_stB_isFull = 0; // 버퍼단위 stDataAbs_nnAPI 크기
	//0일 경우 stB 버퍼가 가득차지 않음
	//1일 경우 stB 버퍼가 가득참 -> 0으로 바뀌지 않으면 stB는 stA로부터 정보를 읽지 않음
	//0로 바꾸어주는 행위는 다른 함수에서 stB버퍼의 데이터를 가져가고 일어난다.

	//stA  stB 두개의 스레드가 작동하고
	//Dlg 객체 멤버변수와 멤버버퍼를 사용하여 
	//최종적으로 stDataAbs_nnAPI 구조체 데이터를(하나씩) 얻는다.
	/////////////////////////

	//power spectrum
	int m_ch1_isFull = 0;
	int m_ch2_isFull = 0;
	double Power_Spectrum_CH1[129];
	double Power_Spectrum_CH2[129];
	unsigned char pud0;
	int n = 1;
	int temp = 1;
	int is_temp = 0;
	int is_pud0 = 0;
	int m_Power_Data = 0;


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	void OnOK(); //오버라이딩 //DLG 상에서 엔터키 눌렀을 때 일어나는 이벤트


	//두개의 unsinged char 값을 받아 double 값으로 반환하는 함수
	double ToDouble(unsigned char A, unsigned char B);
	//test

	int getAbit(unsigned char x, int n);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancel();
};
