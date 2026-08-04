#include <windows.h> // Windows GUI 프로그래밍을 위한 핵심 헤더 파일
#include <string>    // std::wstring 등 문자열 처리를 위한 헤더
#include <cstdlib>   // _wtof 등 문자열-숫자 변환 함수용
#include <cwchar>    // swprintf_s 등 와이드 문자열 포맷팅용

// ============================================================================
// [1. 매크로 상의 정의 (ID 값 부여)]
// 각 버튼과 입력창을 식별하기 위한 고유한 ID 번호입니다.
// 버튼 클릭 이벤트가 발생했을 때 "어떤 버튼이 눌렸는지" 구분하는 기준이 됩니다.
// ============================================================================
#define ID_EDIT 100

// 숫자 버튼 ID (1000 ~ 1009)
#define ID_BTN_0 1000
#define ID_BTN_1 1001
#define ID_BTN_2 1002
#define ID_BTN_3 1003
#define ID_BTN_4 1004
#define ID_BTN_5 1005
#define ID_BTN_6 1006
#define ID_BTN_7 1007
#define ID_BTN_8 1008
#define ID_BTN_9 1009

// 연산자 버튼 ID (2000번대)
#define ID_BTN_ADD 2001 // + (더하기)
#define ID_BTN_SUB 2002 // - (빼기)
#define ID_BTN_MUL 2003 // * (곱하기)
#define ID_BTN_DIV 2004 // / (나누기)
#define ID_BTN_EQU 2005 // = (결과 계산)
#define ID_BTN_CLR 2006 // C (초기화)

// ============================================================================
// [2. 계산기 상태를 저장하는 전역 변수]
// ============================================================================
HWND hEdit;                     // 화면 텍스트 상자(Edit Control)의 핸들(조종간)
std::wstring currentInput = L""; // 현재 입력창에 표시되는 문자열 (유니코드 사용)
double firstNum = 0;            // 첫 번째로 입력된 숫자 저장 (예: '5' + '3' 에서 '5')
wchar_t currentOp = 0;          // 현재 선택된 연산자 저장 (L'+', L'-', L'*', L'/')
bool isNewInput = true;         // 새로운 숫자를 입력받을 타이밍인지 여부 (플래그 변수)

// ============================================================================
// [3. 윈도우 프로시저 (WndProc) - 이벤트 처리기]
// 버튼 클릭, 창 생성, 창 닫기 등 사용자 및 시스템 이벤트(메시지)를 처리하는 핵심 함수입니다.
// ============================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        // ------------------------------------------------------------------------
        // A. WM_CREATE: 창이 처음 생성될 때 1회 실행 (화면 구성요소 배치)
        // ------------------------------------------------------------------------
    case WM_CREATE: {
        // 1. 계산기 숫자 출력용 Edit 컨트롤(텍스트 상자) 생성
        hEdit = CreateWindow(
            L"EDIT", L"0",                                                    // 클래스 이름, 초기 텍스트
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_READONLY,      // 스타일 (자식 창, 보임, 테두리, 우측 정렬, 읽기 전용)
            20, 20, 230, 40,                                                  // 위치 (x=20, y=20) 및 크기 (너비=230, 높이=40)
            hwnd, (HMENU)(UINT_PTR)ID_EDIT, NULL, NULL                        // 부모 창, 컨트롤 ID, 인스턴스, 파라미터
        );

        // 2. 버튼 생성을 위한 배열 데이터 준비 (4행 4열 그리드 배치용)
        wchar_t btnLabels[4][4][2] = {
            {L"7", L"8", L"9", L"/"},
            {L"4", L"5", L"6", L"*"},
            {L"1", L"2", L"3", L"-"},
            {L"C", L"0", L"=", L"+"}
        };

        int btnIds[4][4] = {
            {ID_BTN_7, ID_BTN_8, ID_BTN_9, ID_BTN_DIV},
            {ID_BTN_4, ID_BTN_5, ID_BTN_6, ID_BTN_MUL},
            {ID_BTN_1, ID_BTN_2, ID_BTN_3, ID_BTN_SUB},
            {ID_BTN_CLR, ID_BTN_0, ID_BTN_EQU, ID_BTN_ADD}
        };

        // 3. 이중 for문을 이용해 16개의 버튼을 반복 생성
        for (int i = 0; i < 4; i++) {       // 행 반복 (세로)
            for (int j = 0; j < 4; j++) {   // 열 반복 (가로)
                CreateWindow(
                    L"BUTTON", btnLabels[i][j],                     // 버튼 종류, 버튼 겉면에 보일 텍스트
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,           // 기본 버튼 스타일
                    20 + j * 60, 80 + i * 60, 50, 50,               // 좌표 자동 계산 (간격 60px, 버튼 크기 50x50)
                    hwnd, (HMENU)(UINT_PTR)btnIds[i][j], NULL, NULL  // 부모 창 및 ID 설정
                );
            }
        }
        break;
    }

                  // ------------------------------------------------------------------------
                  // B. WM_COMMAND: 버튼을 클릭하는 등 사용자가 명령을 내렸을 때 실행
                  // ------------------------------------------------------------------------
    case WM_COMMAND: {
        int id = LOWORD(wParam); // 눌린 버튼/컨트롤의 ID 값을 가져옴

        // --- B-1. 숫자 버튼(0~9)이 눌린 경우 ---
        if (id >= ID_BTN_0 && id <= ID_BTN_9) {
            int num = id - ID_BTN_0; // ID 값 계산을 통해 눌린 숫자값 추출 (예: 1003 - 1000 = 3)

            // 연산자 클릭 직후이거나 초기 상태("0")인 경우 새 숫자로 덮어씀
            if (isNewInput || currentInput == L"0") {
                currentInput = std::to_wstring(num);
                isNewInput = false; // 숫자 입력 중 상태로 전환
            }
            else {
                currentInput += std::to_wstring(num); // 기존 숫자에 이어 붙이기 (예: "1" + "2" = "12")
            }
            SetWindowText(hEdit, currentInput.c_str()); // 텍스트 상자 화면 갱신
        }

        // --- B-2. 연산자 버튼(+, -, *, /)이 눌린 경우 ---
        else if (id == ID_BTN_ADD || id == ID_BTN_SUB || id == ID_BTN_MUL || id == ID_BTN_DIV) {
            firstNum = _wtof(currentInput.c_str()); // 현재 화면의 문자열을 실수(double)로 변환하여 저장
            isNewInput = true;                       // 다음 입력될 숫자는 새로 작성되도록 플래그 설정

            // 눌린 연산자 기호 기억
            if (id == ID_BTN_ADD) currentOp = L'+';
            if (id == ID_BTN_SUB) currentOp = L'-';
            if (id == ID_BTN_MUL) currentOp = L'*';
            if (id == ID_BTN_DIV) currentOp = L'/';
        }

        // --- B-3. '=' 버튼이 눌린 경우 (실제 연산 진행) ---
        else if (id == ID_BTN_EQU) {
            double secondNum = _wtof(currentInput.c_str()); // 두 번째 숫자 가공
            double result = 0;

            // 기억된 연산자에 따라 해당 수학 연산 수행
            if (currentOp == L'+') result = firstNum + secondNum;
            else if (currentOp == L'-') result = firstNum - secondNum;
            else if (currentOp == L'*') result = firstNum * secondNum;
            else if (currentOp == L'/') {
                if (secondNum != 0) {
                    result = firstNum / secondNum;
                }
                else {
                    // 예외 처리: 0으로 나눌 수 없음
                    SetWindowText(hEdit, L"0으로 나눌 수 없음");
                    isNewInput = true;
                    return 0;
                }
            }

            // 결과를 깔끔하게 문자열로 변환 (소수점이 필요 없을 땐 정수로 출력)
            wchar_t buffer[64];
            if (result == (long long)result)
                swprintf_s(buffer, 64, L"%lld", (long long)result); // 정수 형태 포맷팅
            else
                swprintf_s(buffer, 64, L"%g", result);               // 소수점 형태 포맷팅

            currentInput = buffer;
            SetWindowText(hEdit, currentInput.c_str()); // 결과 화면 출력
            isNewInput = true; // 다음 입력을 위해 플래그 설정
            currentOp = 0;      // 연산자 초기화
        }

        // --- B-4. 'C' (Clear) 버튼이 눌린 경우 (모든 상태 리셋) ---
        else if (id == ID_BTN_CLR) {
            currentInput = L"0";
            firstNum = 0;
            currentOp = 0;
            isNewInput = true;
            SetWindowText(hEdit, L"0");
        }
        break;
    }

                   // ------------------------------------------------------------------------
                   // C. WM_DESTROY: 사용자가 X 버튼을 눌러 창을 닫을 때 실행
                   // ------------------------------------------------------------------------
    case WM_DESTROY:
        PostQuitMessage(0); // 프로그램의 메시지 루프를 종료시킴
        break;

    default:
        // 처리하지 않은 다른 모든 기본 윈도우 이벤트는 OS 기본 처리기에 위임
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ============================================================================
// [4. WinMain 메인 함수]
// Windows GUI 프로그램의 실제 시작점(Entry Point)입니다.
// ============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"CalculatorWindowClass";

    // 1. 윈도우 클래스 구조체 정의 (창의 기본 스타일 및 속성 설정)
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;                  // 위에서 만든 메시지 처리 함수 등록
    wc.hInstance = hInstance;                  // 프로그램 인스턴스 핸들
    wc.lpszClassName = CLASS_NAME;            // 클래스 이름
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 배경색 (윈도우 기본 배경)
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // 마우스 커서 모양 (화살표)

    // 2. 정의한 윈도우 클래스를 시스템에 등록
    RegisterClass(&wc);

    // 3. 실제 화면에 떠오를 윈도우 창 생성
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Calculator",                               // 클래스 이름, 창 제목
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,    // 스타일 (크기 변경 불가능한 고정창)
        CW_USEDEFAULT, CW_USEDEFAULT, 285, 370,                      // 초기 위치(기본값) 및 창 크기 (너비 285, 높이 370)
        NULL, NULL, hInstance, NULL                                 // 부모 창, 메뉴, 인스턴스 등
    );

    if (hwnd == NULL) return 0; // 창 생성 실패 시 즉시 종료

    // 4. 생성된 창을 화면에 보여줌
    ShowWindow(hwnd, nCmdShow);

    // 5. 메시지 루프 (사용자 입력을 24시간 감시하는 메인 무한 루프)
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) { // 종료 메시지(WM_QUIT)가 올 때까지 무한 반복
        TranslateMessage(&msg);            // 키보드 입력을 문자 메시지로 변환
        DispatchMessage(&msg);             // 메세지를 WndProc 함수로 전달
    }

    return 0;
}

// ============================================================================
// [5. main 함수]
// Visual Studio에서 "콘솔 앱" 프로젝트 설정으로 시작되었을 때도 
// 에러 없이 WinMain으로 연결해주기 위한 포워딩 진입점 함수입니다.
// ============================================================================
int main() {
    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
}