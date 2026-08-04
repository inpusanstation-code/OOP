/*
================================================================================
					도서관 관리 시스템 (Library Management System)
================================================================================
프로그램 설명:
  - Win32 API를 사용한 Windows GUI 기반 도서관 관리 프로그램
  - 도서 추가, 삭제, 대출/반납, 파일 저장 기능 제공
  - CSV 형식으로 데이터 저장

학습 포인트:
  1. 객체지향 프로그래밍 (OOP) - 클래스 설계
  2. Win32 API - 윈도우 프로그래밍
  3. 파일 I/O - CSV 형식 데이터 읽고 쓰기
  4. 문자 인코딩 - UTF-8과 UTF-16 변환
  5. GUI 컨트롤 - ListBox, EditBox, Button 사용

작성자: 도서관 관리 시스템
작성일: 2026-08-04
================================================================================
*/

// UTF-8 소스 파일 인코딩 설정 (한글 문자 처리)
#pragma execution_character_set("utf-8")

// Windows API 헤더 파일 포함
#include <windows.h>

// 공용 컨트롤(ListView 등) 사용을 위한 헤더
#include <commctrl.h>

// 파일탐색기 열기 기능 사용을 위한 헤더
#include <shellapi.h>

// 표준 입출력 (cout, cin 등)
#include <iostream>

// 동적 배열(vector) 사용
#include <vector>

// 문자열(string) 사용
#include <string>

// 파일 입출력 (ifstream, ofstream)
#include <fstream>

// 문자열 스트림 처리
#include <sstream>

// Windows 공용 컨트롤 라이브러리 링크
#pragma comment(lib, "comctl32.lib")

// std 네임스페이스 사용 (cout, vector, string 등을 std:: 없이 사용)
using namespace std;

// ============================================================================
// UTF-8을 UTF-16으로 변환하는 헬퍼 함수
// ============================================================================
/*
설명: Win32 API는 내부적으로 UTF-16(유니코드) 사용
	  파일/입력은 UTF-8이므로, GUI에 표시하기 위해 변환 필요

매개변수:
  utf8Str: UTF-8 형식의 문자열

반환값:
  변환된 UTF-16 wstring 객체

예시:
  string input = "안녕하세요";
  wstring output = UTF8ToUTF16(input);  // UTF-8 → UTF-16 변환
*/
wstring UTF8ToUTF16(const string& utf8Str) {
	// 빈 문자열이면 빈 wstring 반환
	if (utf8Str.empty()) return wstring();

	// 1단계: 필요한 버퍼 크기 계산
	// CP_UTF8: UTF-8 인코딩
	// -1: null 종료 문자 포함
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);

	// 2단계: 변환된 문자열을 저장할 메모리 할당
	wchar_t* wstr = new wchar_t[len];

	// 3단계: 실제 변환 수행
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wstr, len);

	// 4단계: 메모리 할당 문제 방지하기 위해 wstring 객체로 변환
	wstring result(wstr);

	// 5단계: 동적 할당 메모리 해제 (메모리 누수 방지)
	delete[] wstr;

	return result;
}

// ============================================================================
// 1. Book 클래스 (데이터 모델)
// ============================================================================
/*
설명: 도서 정보를 저장하는 클래스
	  고유 ID, 제목, 저자, 대출 상태를 관리

설계 원칙:
  - 모든 데이터는 private (캡슐화)
  - getter/setter를 통해서만 데이터 접근
  - toFileFormat(): CSV 파일 저장 형식 반환
*/
class Book {
private:
	// 도서 고유 ID (1, 2, 3, ...)
	int id;

	// 도서 제목 (예: "C++ 완벽 가이드")
	string title;

	// 도서 저자 (예: "비야르네 스트롭스트룹")
	string author;

	// 대출 상태 (true: 대출 중, false: 대출 가능)
	bool isBorrowed;

public:
	/*
	생성자: Book 객체를 초기화

	매개변수:
	  id: 도서 ID (필수)
	  title: 도서 제목 (필수)
	  author: 도서 저자 (필수)
	  isBorrowed: 대출 상태 (선택사항, 기본값 false)

	예시:
	  Book b1(1, "C++ 완벽 가이드", "비야르네");
	  Book b2(2, "파이썬", "저자명", true);
	*/
	Book(int id, string title, string author, bool isBorrowed = false)
		: id(id), title(title), author(author), isBorrowed(isBorrowed) {
	}

	// Getter 메서드: 각 멤버 변수의 값을 반환 (const: 데이터 수정 불가)
	int getId() const { return id; }
	string getTitle() const { return title; }
	string getAuthor() const { return author; }
	bool getIsBorrowed() const { return isBorrowed; }

	// Setter 메서드: 대출 상태 변경 (다른 데이터는 변경 불가)
	void setBorrowed(bool status) { isBorrowed = status; }

	/*
	CSV 파일 형식으로 변환

	CSV 형식: id, title, author, status
	예시 출력:
	  1,C++ 완벽 가이드,비야르네,대출 가능
	  2,파이썬,저자,대출 중

	특수문자 처리:
	  - CSV에서 쉼표(,) 또는 따옴표(")가 포함되면 따옴표로 감싸기
	  - 예: "제목, 포함"은 "\"제목, 포함\""으로 저장
	*/
	string toFileFormat() const {
		// 대출 상태를 한글 텍스트로 변환
		string status = isBorrowed ? "대출 중" : "대출 가능";

		// CSV에서 특수문자 처리: 쉼표(,) 또는 따옴표(") 포함 여부 확인
		// find()는 찾은 위치 반환, string::npos는 찾지 못한 경우
		string titleEscaped = (title.find(',') != string::npos || title.find('"') != string::npos) 
							  ? "\"" + title + "\"" : title;
		string authorEscaped = (author.find(',') != string::npos || author.find('"') != string::npos) 
							   ? "\"" + author + "\"" : author;

		// CSV 한 줄 형식으로 조합하여 반환
		return to_string(id) + "," + titleEscaped + "," + authorEscaped + "," + status;
	}
};

// ============================================================================
// 2. BookManager 클래스 (데이터 및 파일 로직)
// ============================================================================
/*
설명: 모든 도서 데이터를 관리하고 파일 입출력을 담당

책임:
  - 도서 추가/삭제/검색
  - CSV 파일에서 데이터 로드
  - CSV 파일로 데이터 저장

단일 책임 원칙(Single Responsibility Principle):
  데이터 관리와 파일 I/O에만 집중
*/
class BookManager {
private:
	// 모든 도서 정보를 저장하는 vector (동적 배열)
	// vector: 크기가 가변적인 배열
	vector<Book> books;

	// CSV 파일명 (상수로 변경 불가)
	const string filename = "books_data.csv";

public:
	/*
	생성자: BookManager 생성 시 자동으로 CSV 파일에서 데이터 로드
	*/
	BookManager() { loadFromFile(); }

	/*
	설명: CSV 파일에서 도서 데이터를 읽어 메모리에 로드

	파일 형식:
	  ID,제목,저자,상태
	  1,C++ 완벽 가이드,비야르네,대출 가능
	  2,이펙티브 C++,스콧 메이어스,대출 중

	과정:
	  1. CSV 파일 열기
	  2. 첫 번째 줄(헤더) 건너뛰기
	  3. 각 줄을 쉼표(,)로 분리
	  4. Book 객체 생성 후 vector에 추가
	*/
	void loadFromFile() {
		// ifstream: 파일을 읽기 모드로 열기
		ifstream inFile(filename);

		// 파일이 없거나 열 수 없으면 함수 종료
		if (!inFile.is_open()) return;

		// 기존 데이터 모두 삭제 (새로 로드하기 전)
		books.clear();

		// 파일에서 한 줄씩 읽기
		string line;
		bool isFirstLine = true;  // 첫 줄(헤더) 여부 판단

		while (getline(inFile, line)) {
			// 첫 줄(헤더)은 건너뛰기
			// 첫 번째 반복: isFirstLine = true → false로 변경하고 continue
			if (isFirstLine) {
				isFirstLine = false;
				continue;  // 이 줄 건너뛰고 다음으로
			}

			// 빈 줄이면 건너뛰기
			if (line.empty()) continue;

			// stringstream: 문자열을 스트림처럼 처리
			// 예: "1,제목,저자,대출 중" → 각 필드로 분리
			stringstream ss(line);
			string idStr, title, author, borrowedStr;

			// getline(스트림, 저장변수, 구분자)
			// 쉼표(,)를 구분자로 하여 각 필드 추출
			getline(ss, idStr, ',');        // "1"
			getline(ss, title, ',');        // "제목"
			getline(ss, author, ',');       // "저자"
			getline(ss, borrowedStr, ',');  // "대출 중" 또는 "대출 가능"

			// 문자열 ID를 정수로 변환 (stoi = string to integer)
			int id = stoi(idStr);

			// 대출 상태 문자열을 bool로 변환
			bool isBorrowed = (borrowedStr == "대출 중");

			// Book 객체 생성하여 vector에 추가
			books.push_back(Book(id, title, author, isBorrowed));
		}

		// 파일 닫기 (메모리 해제 및 파일 동기화)
		inFile.close();
	}

	/*
	설명: 메모리의 모든 도서 정보를 CSV 파일로 저장

	파일 형식:
	  ID,제목,저자,상태
	  1,C++ 완벽 가이드,비야르네,대출 가능

	반환값:
	  bool - 저장 성공(true) 또는 실패(false)
	*/
	bool saveToFile() const {
		// ofstream: 파일을 쓰기 모드로 열기
		// 기존 파일이 있으면 덮어쓰기
		ofstream outFile(filename);

		// 파일 열기 실패하면 false 반환
		if (!outFile.is_open()) return false;

		// CSV 헤더 행 작성
		// 첫 줄은 각 열의 의미를 표시 (데이터가 아님)
		outFile << "ID,제목,저자,상태\n";

		// 모든 도서를 순회하며 CSV 형식으로 작성
		// const auto&: 복사 없이 참조로 순회 (성능 최적화)
		for (const auto& book : books) {
			outFile << book.toFileFormat() << "\n";
		}

		// 파일 닫기 및 메모리 플러시
		outFile.close();

		return true;
	}

	/*
	설명: 새로운 도서 추가

	매개변수:
	  id: 도서 ID (중복 불가)
	  title: 도서 제목
	  author: 도서 저자

	반환값:
	  bool - 추가 성공(true) 또는 ID 중복으로 실패(false)

	동작:
	  1. 같은 ID가 있는지 확인
	  2. 없으면 새 Book 객체 생성
	  3. 파일에 저장
	*/
	bool addBook(int id, const string& title, const string& author) {
		// 기존 도서 중 같은 ID가 있는지 확인
		for (const auto& book : books) {
			if (book.getId() == id) {
				// 중복 ID이면 false 반환 (추가 실패)
				return false;
			}
		}

		// 새로운 도서 객체 생성하여 추가
		// isBorrowed 기본값은 false (신규 도서는 대출 가능)
		books.push_back(Book(id, title, author));

		// 변경사항 파일에 저장
		saveToFile();

		return true;
	}

	/*
	설명: 도서의 대출/반납 상태 토글 (반대로 전환)

	매개변수:
	  id: 대출/반납 상태를 변경할 도서 ID

	반환값:
	  bool - 상태 변경 성공(true) 또는 해당 ID 없음(false)

	동작:
	  대출 중 → 대출 가능
	  대출 가능 → 대출 중
	  파일 자동 저장
	*/
	bool toggleBorrow(int id) {
		// 모든 도서 순회
		for (auto& book : books) {
			if (book.getId() == id) {
				// ! 연산자: 논리 반전
				// true → false, false → true
				book.setBorrowed(!book.getIsBorrowed());

				// 변경사항 파일에 저장
				saveToFile();

				return true;
			}
		}

		// ID를 찾지 못하면 false 반환
		return false;
	}

	/*
	설명: 도서 삭제

	매개변수:
	  id: 삭제할 도서 ID

	반환값:
	  bool - 삭제 성공(true) 또는 ID 없음(false)

	동작:
	  1. 해당 ID의 도서 찾기
	  2. vector에서 제거 (erase)
	  3. 파일에 저장
	*/
	bool deleteBook(int id) {
		// iterator(반복자): vector의 각 요소를 순회하는 포인터 같은 개념
		// begin(): 첫 번째 요소
		// end(): 마지막 요소 다음
		for (auto it = books.begin(); it != books.end(); ++it) {
			if (it->getId() == id) {
				// it->: 반복자가 가리키는 객체의 멤버 접근
				// erase(): 해당 위치의 요소 제거
				books.erase(it);

				// 변경사항 파일에 저장
				saveToFile();

				return true;
			}
		}

		// ID를 찾지 못하면 false 반환
		return false;
	}

	// Getter: 모든 도서 반환 (const& 로 복사 없이 참조)
	const vector<Book>& getBooks() const { return books; }
};

// ============================================================================
// 3. Win32 GUI 제어 (전역 변수 및 이벤트 핸들러)
// ============================================================================

// 전역 BookManager 객체: 프로그램 전체에서 공유
BookManager g_manager;

// 각 GUI 컨트롤의 ID 정의 (고유한 번호로 구분)
// 예: 버튼을 클릭하면 어느 버튼인지 알기 위해 필요
#define IDC_LISTVIEW   101          // 도서 목록을 표시하는 ListView
#define IDC_EDIT_ID    102          // ID 입력 필드
#define IDC_EDIT_TITLE 103          // 제목 입력 필드
#define IDC_EDIT_AUTHOR 104         // 저자 입력 필드
#define IDC_BTN_ADD    105          // "도서 등록" 버튼
#define IDC_BTN_BORROW 106          // "대출/반납" 버튼
#define IDC_BTN_DELETE 107          // "삭제" 버튼
#define IDC_BTN_SAVE   108          // "파일 저장" 버튼
#define IDC_BTN_OPEN_FOLDER 109     // "폴더 열기" 버튼

// 전역 윈도우 핸들(Handle): GUI 컨트롤 조작에 필요
// HWND = Handle to Window
HWND hListView,     // 도서 목록 표시
	 hEditId,       // ID 입력 필드
	 hEditTitle,    // 제목 입력 필드
	 hEditAuthor;   // 저자 입력 필드

/*
설명: ListView 컨트롤을 메모리의 도서 데이터로 업데이트
	  프로그램 시작 시, 도서 추가/삭제/대출 후 호출

동작:
  1. ListView의 모든 항목 삭제
  2. g_manager의 모든 도서를 순회
  3. 각 도서를 ListView의 새 행으로 추가
*/
void UpdateListView() {
	// ListView의 모든 항목 삭제하여 초기화
	ListView_DeleteAllItems(hListView);

	// g_manager에서 도서 목록 가져오기 (복사 아닌 참조)
	const auto& books = g_manager.getBooks();

	// 모든 도서를 순회하며 ListView에 추가
	// size_t: unsigned long (음수 불가능한 정수)
	for (size_t i = 0; i < books.size(); ++i) {
		// LVITEMW: ListView의 한 항목
		// W: Wide Character (유니코드) 버전
		LVITEMW lvi = { 0 };  // 모든 멤버를 0으로 초기화

		lvi.mask = LVIF_TEXT;  // 텍스트 필드 설정 표시
		lvi.iItem = static_cast<int>(i);  // 행 번호 (0부터 시작)

		// ID를 문자열로 변환 후 UTF-16으로 변환
		wstring idStr = UTF8ToUTF16(to_string(books[i].getId()));
		lvi.pszText = const_cast<LPWSTR>(idStr.c_str());

		// ListView에 새 항목(행) 삽입
		SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

		// 부가 정보(서브 항목): 같은 행의 다른 열들
		LVITEMW lviSub = { 0 };
		lviSub.iItem = static_cast<int>(i);  // 같은 행

		// 컬럼 1: 제목
		lviSub.iSubItem = 1;  // 열 번호 (0=ID, 1=제목, ...)
		wstring titleStr = UTF8ToUTF16(books[i].getTitle());
		lviSub.pszText = const_cast<LPWSTR>(titleStr.c_str());
		SendMessageW(hListView, LVM_SETITEMTEXTW, i, (LPARAM)&lviSub);

		// 컬럼 2: 저자
		lviSub.iSubItem = 2;
		wstring authorStr = UTF8ToUTF16(books[i].getAuthor());
		lviSub.pszText = const_cast<LPWSTR>(authorStr.c_str());
		SendMessageW(hListView, LVM_SETITEMTEXTW, i, (LPARAM)&lviSub);

		// 컬럼 3: 대출 상태
		string statusStr = books[i].getIsBorrowed() ? "대출 중" : "대출 가능";
		wstring statusStrW = UTF8ToUTF16(statusStr);
		lviSub.iSubItem = 3;
		lviSub.pszText = const_cast<LPWSTR>(statusStrW.c_str());
		SendMessageW(hListView, LVM_SETITEMTEXTW, i, (LPARAM)&lviSub);
	}
}

/*
설명: 윈도우 이벤트 처리 콜백 함수
	  사용자의 마우스 클릭, 키보드 입력 등의 이벤트를 처리

매개변수:
  hWnd: 이벤트를 발생시킨 윈도우의 핸들
  message: 이벤트 종류 (WM_CREATE, WM_COMMAND, WM_DESTROY 등)
  wParam: 이벤트 추가 정보 1
  lParam: 이벤트 추가 정보 2

반환값:
  LRESULT: 이벤트 처리 결과
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	// WM_CREATE: 윈도우가 생성될 때 호출 (프로그램 시작 시)
	case WM_CREATE: {
		// ListView 컨트롤 생성
		// CreateWindowExW: W=Wide character (유니코드)
		// 매개변수:
		//   0: 확장 스타일 없음
		//   WC_LISTVIEWW: ListView 클래스
		//   L"": 초기 텍스트 없음
		//   WS_CHILD | WS_VISIBLE | LVS_REPORT | ...: 윈도우 스타일
		//   20, 20: 위치 (x, y)
		//   740, 250: 크기 (가로, 세로)
		//   hWnd: 부모 윈도우
		//   (HMENU)IDC_LISTVIEW: 컨트롤 ID
		hListView = CreateWindowExW(0, WC_LISTVIEWW, L"",
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
			20, 20, 740, 250, hWnd, (HMENU)IDC_LISTVIEW, GetModuleHandle(NULL), NULL);

		// ListView 스타일 설정
		// LVS_EX_FULLROWSELECT: 행 전체 선택
		// LVS_EX_GRIDLINES: 격자선 표시
		ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		// ListView 열(컬럼) 추가
		LVCOLUMNW lvc = { 0 };
		lvc.mask = LVCF_TEXT | LVCF_WIDTH;

		// 컬럼 0: ID
		wstring col1 = UTF8ToUTF16("ID");
		lvc.pszText = const_cast<LPWSTR>(col1.c_str());
		lvc.cx = 80;  // 너비
		SendMessageW(hListView, LVM_INSERTCOLUMNW, 0, (LPARAM)&lvc);

		// 컬럼 1: 제목
		wstring col2 = UTF8ToUTF16("제목");
		lvc.pszText = const_cast<LPWSTR>(col2.c_str());
		lvc.cx = 300;
		SendMessageW(hListView, LVM_INSERTCOLUMNW, 1, (LPARAM)&lvc);

		// 컬럼 2: 저자
		wstring col3 = UTF8ToUTF16("저자");
		lvc.pszText = const_cast<LPWSTR>(col3.c_str());
		lvc.cx = 200;
		SendMessageW(hListView, LVM_INSERTCOLUMNW, 2, (LPARAM)&lvc);

		// 컬럼 3: 상태
		wstring col4 = UTF8ToUTF16("상태");
		lvc.pszText = const_cast<LPWSTR>(col4.c_str());
		lvc.cx = 120;
		SendMessageW(hListView, LVM_INSERTCOLUMNW, 3, (LPARAM)&lvc);

		// 하단 입력 폼 생성
		// STATIC: 텍스트 레이블
		CreateWindowW(L"STATIC", UTF8ToUTF16("ID:").c_str(), WS_CHILD | WS_VISIBLE,
			20, 290, 40, 20, hWnd, NULL, NULL, NULL);

		// EDIT: 텍스트 입력 필드
		// ES_NUMBER: 숫자만 입력 가능
		hEditId = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			60, 285, 80, 25, hWnd, (HMENU)IDC_EDIT_ID, NULL, NULL);

		// 제목 입력 필드
		CreateWindowW(L"STATIC", UTF8ToUTF16("제목:").c_str(), WS_CHILD | WS_VISIBLE,
			160, 290, 40, 20, hWnd, NULL, NULL, NULL);
		hEditTitle = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
			200, 285, 200, 25, hWnd, (HMENU)IDC_EDIT_TITLE, NULL, NULL);

		// 저자 입력 필드
		CreateWindowW(L"STATIC", UTF8ToUTF16("저자:").c_str(), WS_CHILD | WS_VISIBLE,
			420, 290, 40, 20, hWnd, NULL, NULL, NULL);
		hEditAuthor = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
			460, 285, 150, 25, hWnd, (HMENU)IDC_EDIT_AUTHOR, NULL, NULL);

		// 버튼 생성
		// "도서 등록" 버튼
		CreateWindowW(L"BUTTON", UTF8ToUTF16("도서 등록").c_str(), WS_CHILD | WS_VISIBLE,
			630, 285, 130, 25, hWnd, (HMENU)IDC_BTN_ADD, NULL, NULL);

		// "대출/반납" 버튼
		CreateWindowW(L"BUTTON", UTF8ToUTF16("선택 도서 대출 / 반납").c_str(), WS_CHILD | WS_VISIBLE,
			20, 330, 160, 35, hWnd, (HMENU)IDC_BTN_BORROW, NULL, NULL);

		// "삭제" 버튼
		CreateWindowW(L"BUTTON", UTF8ToUTF16("선택 도서 삭제").c_str(), WS_CHILD | WS_VISIBLE,
			190, 330, 160, 35, hWnd, (HMENU)IDC_BTN_DELETE, NULL, NULL);

		// "파일 저장" 버튼
		CreateWindowW(L"BUTTON", UTF8ToUTF16("파일 저장").c_str(), WS_CHILD | WS_VISIBLE,
			360, 330, 160, 35, hWnd, (HMENU)IDC_BTN_SAVE, NULL, NULL);

		// "폴더 열기" 버튼
		CreateWindowW(L"BUTTON", UTF8ToUTF16("폴더 열기").c_str(), WS_CHILD | WS_VISIBLE,
			530, 330, 160, 35, hWnd, (HMENU)IDC_BTN_OPEN_FOLDER, NULL, NULL);

		// UpdateListView(): CSV 파일에서 로드한 도서 목록을 화면에 표시
		UpdateListView();
		break;
	}

	// WM_COMMAND: 버튼 클릭, 메뉴 선택 등의 명령 이벤트
	case WM_COMMAND: {
		// LOWORD(wParam): 컨트롤/메뉴 ID 추출
		int wmId = LOWORD(wParam);

		switch (wmId) {

		// "도서 등록" 버튼 클릭
		case IDC_BTN_ADD: {
			// 입력 필드에서 텍스트 획득
			wchar_t idBuf[32], titleBuf[256], authorBuf[256];
			GetWindowTextW(hEditId, idBuf, 32);        // ID 가져오기
			GetWindowTextW(hEditTitle, titleBuf, 256);  // 제목 가져오기
			GetWindowTextW(hEditAuthor, authorBuf, 256); // 저자 가져오기

			// 입력값 검증: 모두 입력했는지 확인
			if (wcslen(idBuf) == 0 || wcslen(titleBuf) == 0 || wcslen(authorBuf) == 0) {
				// wcslen: wstring의 길이 (w=wide character)
				MessageBoxW(hWnd, UTF8ToUTF16("모든 항목을 입력하세요.").c_str(),
					UTF8ToUTF16("경고").c_str(), MB_OK | MB_ICONWARNING);
				break;
			}

			// Wide character(UTF-16)를 멀티바이트(UTF-8)로 변환
			// GUI 입력은 UTF-16, 파일 저장은 UTF-8이므로 변환 필요
			int idSize = WideCharToMultiByte(CP_UTF8, 0, idBuf, -1, NULL, 0, NULL, NULL);
			int titleSize = WideCharToMultiByte(CP_UTF8, 0, titleBuf, -1, NULL, 0, NULL, NULL);
			int authorSize = WideCharToMultiByte(CP_UTF8, 0, authorBuf, -1, NULL, 0, NULL, NULL);

			char idStr[32], titleStr[256], authorStr[256];
			WideCharToMultiByte(CP_UTF8, 0, idBuf, -1, idStr, idSize, NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, titleBuf, -1, titleStr, titleSize, NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, authorBuf, -1, authorStr, authorSize, NULL, NULL);

			// ID를 정수로 변환
			int id = atoi(idStr);  // atoi: ASCII to integer

			// BookManager에 도서 추가
			if (g_manager.addBook(id, titleStr, authorStr)) {
				// 추가 성공
				MessageBoxW(hWnd, UTF8ToUTF16("도서가 등록되었습니다.").c_str(),
					UTF8ToUTF16("알림").c_str(), MB_OK);

				// 입력 필드 초기화 (다음 입력 준비)
				SetWindowTextW(hEditId, L"");
				SetWindowTextW(hEditTitle, L"");
				SetWindowTextW(hEditAuthor, L"");

				// ListView 업데이트하여 새 도서 표시
				UpdateListView();
			}
			else {
				// 이미 같은 ID의 도서가 있음
				MessageBoxW(hWnd, UTF8ToUTF16("이미 존재하는 ID입니다.").c_str(),
					UTF8ToUTF16("에러").c_str(), MB_OK | MB_ICONERROR);
			}
			break;
		}

		// "대출/반납" 버튼 클릭
		case IDC_BTN_BORROW: {
			// ListView에서 선택된 행 찾기
			// -1: 검색 시작 위치 (맨 처음부터)
			// LVNI_SELECTED: 선택된 항목 찾기
			int selectedRow = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

			if (selectedRow == -1) {
				// 선택된 항목 없음
				MessageBoxW(hWnd, UTF8ToUTF16("선택된 도서가 없습니다.").c_str(),
					UTF8ToUTF16("경고").c_str(), MB_OK | MB_ICONWARNING);
				break;
			}

			// 선택된 행의 ID 컬럼 텍스트 추출
			wchar_t idBuf[32];
			LVITEMW lvi = { 0 };
			lvi.iSubItem = 0;  // 첫 번째 컬럼 (ID)
			lvi.pszText = idBuf;
			lvi.cchTextMax = 32;
			SendMessageW(hListView, LVM_GETITEMTEXTW, selectedRow, (LPARAM)&lvi);

			// 대출/반납 상태 토글
			g_manager.toggleBorrow(_wtoi(idBuf));  // _wtoi: wide to integer

			// ListView 갱신
			UpdateListView();
			break;
		}

		// "삭제" 버튼 클릭
		case IDC_BTN_DELETE: {
			// 선택된 행 찾기
			int selectedRow = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

			if (selectedRow == -1) {
				MessageBoxW(hWnd, UTF8ToUTF16("삭제할 도서를 선택하세요.").c_str(),
					UTF8ToUTF16("경고").c_str(), MB_OK | MB_ICONWARNING);
				break;
			}

			// 선택된 행의 ID 추출
			wchar_t idBuf[32];
			LVITEMW lvi = { 0 };
			lvi.iSubItem = 0;
			lvi.pszText = idBuf;
			lvi.cchTextMax = 32;
			SendMessageW(hListView, LVM_GETITEMTEXTW, selectedRow, (LPARAM)&lvi);

			// 도서 삭제
			g_manager.deleteBook(_wtoi(idBuf));

			// ListView 갱신
			UpdateListView();

			// 삭제 완료 메시지
			MessageBoxW(hWnd, UTF8ToUTF16("도서가 삭제되었습니다.").c_str(),
				UTF8ToUTF16("알림").c_str(), MB_OK);
			break;
		}

		// "파일 저장" 버튼 클릭
		case IDC_BTN_SAVE: {
			// 메모리의 모든 데이터를 CSV 파일로 저장
			if (g_manager.saveToFile()) {
				MessageBoxW(hWnd, UTF8ToUTF16("books_data.csv 파일에 저장되었습니다.").c_str(),
					UTF8ToUTF16("성공").c_str(), MB_OK);
			}
			break;
		}

		// "폴더 열기" 버튼 클릭
		case IDC_BTN_OPEN_FOLDER: {
			// ShellExecuteW를 사용하여 파일탐색기 열기
			// NULL: 현재 윈도우가 부모
			// L"explore": 파일탐색기로 열기
			// L".": 현재 디렉토리
			// NULL, NULL: 매개변수 없음
			// SW_SHOW: 일반 크기로 표시
			ShellExecuteW(NULL, L"explore", L".", NULL, NULL, SW_SHOW);
			break;
		}
		}
		break;
	}

	// WM_DESTROY: 윈도우가 닫힐 때 호출 (프로그램 종료)
	case WM_DESTROY:
		// PostQuitMessage: 프로그램 종료 신호 발생
		// 0: 종료 코드 (0=정상 종료)
		PostQuitMessage(0);
		break;

	// 기타 이벤트: 기본 윈도우 처리
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
설명: 프로그램의 진입점 (main 함수 같은 역할)
	  Windows GUI 프로그램은 WinMain에서 시작

매개변수:
  hInstance: 현재 프로그램 인스턴스
  hPrevInstance: 이전 인스턴스 (항상 NULL)
  lpCmdLine: 명령행 인자
  nCmdShow: 윈도우를 어떻게 표시할지 (SW_SHOW 등)

반환값:
  int - 프로그램 종료 코드
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// 공용 컨트롤(ListView 등) 초기화
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;  // ListView 클래스 초기화
	InitCommonControlsEx(&icex);

	// 윈도우 클래스 이름 정의 (고유한 이름)
	const wchar_t CLASS_NAME[] = L"BookManagerWindowClass";

	// 윈도우 클래스 정보 설정
	// WNDCLASSW: W=Wide character (유니코드)
	WNDCLASSW wc = { 0 };  // 모든 멤버를 0으로 초기화

	// 윈도우 메시지를 처리할 콜백 함수
	wc.lpfnWndProc = WndProc;

	// 현재 프로그램 인스턴스
	wc.hInstance = hInstance;

	// 윈도우 클래스 이름
	wc.lpszClassName = CLASS_NAME;

	// 배경색 (COLOR_WINDOW + 1 = 밝은 회색)
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	// 마우스 커서 (기본 화살표)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// 윈도우 클래스 등록
	// 이 정보를 OS에 등록해야 나중에 세부 윈도우 생성 가능
	RegisterClassW(&wc);

	// 실제 윈도우 생성
	HWND hWnd = CreateWindowExW(
		0,                              // 확장 스타일 없음
		CLASS_NAME,                     // 등록한 클래스 이름
		UTF8ToUTF16("도서관 관리 시스템").c_str(),  // 윈도우 제목
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,  // 스타일
		CW_USEDEFAULT, CW_USEDEFAULT,   // 위치 (기본값)
		800, 430,                        // 크기 (가로, 세로)
		NULL,                           // 부모 윈도우 없음
		NULL,                           // 메뉴 없음
		hInstance,                      // 프로그램 인스턴스
		NULL);                          // 추가 정보 없음

	// 윈도우 생성 실패 확인
	if (hWnd == NULL) return 0;

	// 윈도우를 화면에 표시
	ShowWindow(hWnd, nCmdShow);

	// 윈도우 내용 업데이트 (화면에 그리기)
	UpdateWindow(hWnd);

	// 메시지 루프: 프로그램 종료 때까지 계속 실행
	// Windows는 이벤트 기반: 사용자 입력 대기
	MSG msg = { 0 };  // 메시지 구조체 초기화

	// GetMessageW: 메시지 큐에서 메시지 가져오기
	// 0을 반환할 때까지 계속 반복 (WM_QUIT 받을 때)
	while (GetMessageW(&msg, NULL, 0, 0)) {
		// TranslateMessage: 키보드 입력 처리
		TranslateMessage(&msg);

		// DispatchMessageW: 메시지를 윈도우의 WndProc에 전달
		DispatchMessageW(&msg);
	}

	// 프로그램 정상 종료
	return 0;
}
