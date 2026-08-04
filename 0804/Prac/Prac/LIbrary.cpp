#pragma execution_character_set("utf-8")
// UTF-8 파일 인코딩 지정

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#pragma comment(lib, "comctl32.lib")

using namespace std;

// ============================================================================
// UTF-8을 UTF-16으로 변환하는 헬퍼 함수
// ============================================================================
wstring UTF8ToUTF16(const string& utf8Str) {
    if (utf8Str.empty()) return wstring();

    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wstr, len);

    wstring result(wstr);
    delete[] wstr;
    return result;
}

// ============================================================================
// 1. Book 클래스 (데이터 모델)
// ============================================================================
class Book {
private:
    int id;
    string title;
    string author;
    bool isBorrowed;

public:
    Book(int id, string title, string author, bool isBorrowed = false)
        : id(id), title(title), author(author), isBorrowed(isBorrowed) {
    }

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    bool getIsBorrowed() const { return isBorrowed; }

    void setBorrowed(bool status) { isBorrowed = status; }

    string toFileFormat() const {
        string status = isBorrowed ? "대출 중" : "대출 가능";
        // CSV 형식: 특수문자가 있으면 따옴표로 감싸기
        string titleEscaped = (title.find(',') != string::npos || title.find('"') != string::npos) 
                              ? "\"" + title + "\"" : title;
        string authorEscaped = (author.find(',') != string::npos || author.find('"') != string::npos) 
                               ? "\"" + author + "\"" : author;
        return to_string(id) + "," + titleEscaped + "," + authorEscaped + "," + status;
    }
};

// ============================================================================
// 2. BookManager 클래스 (데이터 및 파일 로직)
// ============================================================================
class BookManager {
private:
    vector<Book> books;
    const string filename = "books_data.csv";

public:
    BookManager() { loadFromFile(); }

    void loadFromFile() {
        ifstream inFile(filename);
        if (!inFile.is_open()) return;

        books.clear();
        string line;
        bool isFirstLine = true;
        while (getline(inFile, line)) {
            // 첫 줄(헤더)은 건너뛰기
            if (isFirstLine) {
                isFirstLine = false;
                continue;
            }

            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, title, author, borrowedStr;

            getline(ss, idStr, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, borrowedStr, ',');

            int id = stoi(idStr);
            bool isBorrowed = (borrowedStr == "대출 중");

            books.push_back(Book(id, title, author, isBorrowed));
        }
        inFile.close();
    }

    bool saveToFile() const {
        ofstream outFile(filename);
        if (!outFile.is_open()) return false;
        // 파일 헤더 추가
        outFile << "ID,제목,저자,상태\n";
        for (const auto& book : books) {
            outFile << book.toFileFormat() << "\n";
        }
        outFile.close();
        return true;
    }

    bool addBook(int id, const string& title, const string& author) {
        for (const auto& book : books) {
            if (book.getId() == id) return false;
        }
        books.push_back(Book(id, title, author));
        saveToFile();
        return true;
    }

    bool toggleBorrow(int id) {
        for (auto& book : books) {
            if (book.getId() == id) {
                book.setBorrowed(!book.getIsBorrowed());
                saveToFile();
                return true;
            }
        }
        return false;
    }

    bool deleteBook(int id) {
        for (auto it = books.begin(); it != books.end(); ++it) {
            if (it->getId() == id) {
                books.erase(it);
                saveToFile();
                return true;
            }
        }
        return false;
    }

    const vector<Book>& getBooks() const { return books; }
};

// ============================================================================
// 3. Win32 GUI 제어 (전역 변수 및 이벤트 핸들러)
// ============================================================================
BookManager g_manager;

#define IDC_LISTVIEW   101
#define IDC_EDIT_ID    102
#define IDC_EDIT_TITLE 103
#define IDC_EDIT_AUTHOR 104
#define IDC_BTN_ADD    105
#define IDC_BTN_BORROW 106
#define IDC_BTN_DELETE 107
#define IDC_BTN_SAVE   108
#define IDC_BTN_OPEN_FOLDER 109

HWND hListView, hEditId, hEditTitle, hEditAuthor;

void UpdateListView() {
    ListView_DeleteAllItems(hListView);
    const auto& books = g_manager.getBooks();

    for (size_t i = 0; i < books.size(); ++i) {
        LVITEMW lvi = { 0 };
        lvi.mask = LVIF_TEXT;
        lvi.iItem = static_cast<int>(i);

        wstring idStr = UTF8ToUTF16(to_string(books[i].getId()));
        lvi.pszText = const_cast<LPWSTR>(idStr.c_str());
        SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

        LVITEMW lviSub = { 0 };
        lviSub.iItem = static_cast<int>(i);

        lviSub.iSubItem = 1;
        wstring titleStr = UTF8ToUTF16(books[i].getTitle());
        lviSub.pszText = const_cast<LPWSTR>(titleStr.c_str());
        SendMessageW(hListView, LVM_SETITEMTEXTW, i, (LPARAM)&lviSub);

        lviSub.iSubItem = 2;
        wstring authorStr = UTF8ToUTF16(books[i].getAuthor());
        lviSub.pszText = const_cast<LPWSTR>(authorStr.c_str());
        SendMessageW(hListView, LVM_SETITEMTEXTW, i, (LPARAM)&lviSub);

        string statusStr = books[i].getIsBorrowed() ? "대출 중" : "대출 가능";
        wstring statusStrW = UTF8ToUTF16(statusStr);
        lviSub.iSubItem = 3;
        lviSub.pszText = const_cast<LPWSTR>(statusStrW.c_str());
        SendMessageW(hListView, LVM_SETITEMTEXTW, i, (LPARAM)&lviSub);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        hListView = CreateWindowExW(0, WC_LISTVIEWW, L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
            20, 20, 740, 250, hWnd, (HMENU)IDC_LISTVIEW, GetModuleHandle(NULL), NULL);
        ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH;

        wstring col1 = UTF8ToUTF16("ID");
        lvc.pszText = const_cast<LPWSTR>(col1.c_str()); lvc.cx = 80; SendMessageW(hListView, LVM_INSERTCOLUMNW, 0, (LPARAM)&lvc);

        wstring col2 = UTF8ToUTF16("제목");
        lvc.pszText = const_cast<LPWSTR>(col2.c_str()); lvc.cx = 300; SendMessageW(hListView, LVM_INSERTCOLUMNW, 1, (LPARAM)&lvc);

        wstring col3 = UTF8ToUTF16("저자");
        lvc.pszText = const_cast<LPWSTR>(col3.c_str()); lvc.cx = 200; SendMessageW(hListView, LVM_INSERTCOLUMNW, 2, (LPARAM)&lvc);

        wstring col4 = UTF8ToUTF16("상태");
        lvc.pszText = const_cast<LPWSTR>(col4.c_str()); lvc.cx = 120; SendMessageW(hListView, LVM_INSERTCOLUMNW, 3, (LPARAM)&lvc);

        CreateWindowW(L"STATIC", UTF8ToUTF16("ID:").c_str(), WS_CHILD | WS_VISIBLE, 20, 290, 40, 20, hWnd, NULL, NULL, NULL);
        hEditId = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 60, 285, 80, 25, hWnd, (HMENU)IDC_EDIT_ID, NULL, NULL);

        CreateWindowW(L"STATIC", UTF8ToUTF16("제목:").c_str(), WS_CHILD | WS_VISIBLE, 160, 290, 40, 20, hWnd, NULL, NULL, NULL);
        hEditTitle = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 200, 285, 200, 25, hWnd, (HMENU)IDC_EDIT_TITLE, NULL, NULL);

        CreateWindowW(L"STATIC", UTF8ToUTF16("저자:").c_str(), WS_CHILD | WS_VISIBLE, 420, 290, 40, 20, hWnd, NULL, NULL, NULL);
        hEditAuthor = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 460, 285, 150, 25, hWnd, (HMENU)IDC_EDIT_AUTHOR, NULL, NULL);

        CreateWindowW(L"BUTTON", UTF8ToUTF16("도서 등록").c_str(), WS_CHILD | WS_VISIBLE, 630, 285, 130, 25, hWnd, (HMENU)IDC_BTN_ADD, NULL, NULL);
        CreateWindowW(L"BUTTON", UTF8ToUTF16("선택 도서 대출 / 반납").c_str(), WS_CHILD | WS_VISIBLE, 20, 330, 160, 35, hWnd, (HMENU)IDC_BTN_BORROW, NULL, NULL);
        CreateWindowW(L"BUTTON", UTF8ToUTF16("선택 도서 삭제").c_str(), WS_CHILD | WS_VISIBLE, 190, 330, 160, 35, hWnd, (HMENU)IDC_BTN_DELETE, NULL, NULL);
        CreateWindowW(L"BUTTON", UTF8ToUTF16("파일 저장").c_str(), WS_CHILD | WS_VISIBLE, 360, 330, 160, 35, hWnd, (HMENU)IDC_BTN_SAVE, NULL, NULL);
        CreateWindowW(L"BUTTON", UTF8ToUTF16("폴더 열기").c_str(), WS_CHILD | WS_VISIBLE, 530, 330, 160, 35, hWnd, (HMENU)IDC_BTN_OPEN_FOLDER, NULL, NULL);

        UpdateListView();
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDC_BTN_ADD: {
            wchar_t idBuf[32], titleBuf[256], authorBuf[256];
            GetWindowTextW(hEditId, idBuf, 32);
            GetWindowTextW(hEditTitle, titleBuf, 256);
            GetWindowTextW(hEditAuthor, authorBuf, 256);

            if (wcslen(idBuf) == 0 || wcslen(titleBuf) == 0 || wcslen(authorBuf) == 0) {
                MessageBoxW(hWnd, UTF8ToUTF16("모든 항목을 입력하세요.").c_str(), UTF8ToUTF16("경고").c_str(), MB_OK | MB_ICONWARNING);
                break;
            }

            // 와이드 문자를 멀티바이트 문자로 변환
            int idSize = WideCharToMultiByte(CP_UTF8, 0, idBuf, -1, NULL, 0, NULL, NULL);
            int titleSize = WideCharToMultiByte(CP_UTF8, 0, titleBuf, -1, NULL, 0, NULL, NULL);
            int authorSize = WideCharToMultiByte(CP_UTF8, 0, authorBuf, -1, NULL, 0, NULL, NULL);

            char idStr[32], titleStr[256], authorStr[256];
            WideCharToMultiByte(CP_UTF8, 0, idBuf, -1, idStr, idSize, NULL, NULL);
            WideCharToMultiByte(CP_UTF8, 0, titleBuf, -1, titleStr, titleSize, NULL, NULL);
            WideCharToMultiByte(CP_UTF8, 0, authorBuf, -1, authorStr, authorSize, NULL, NULL);

            int id = atoi(idStr);
            if (g_manager.addBook(id, titleStr, authorStr)) {
                MessageBoxW(hWnd, UTF8ToUTF16("도서가 등록되었습니다.").c_str(), UTF8ToUTF16("알림").c_str(), MB_OK);
                SetWindowTextW(hEditId, L"");
                SetWindowTextW(hEditTitle, L"");
                SetWindowTextW(hEditAuthor, L"");
                UpdateListView();
            }
            else {
                MessageBoxW(hWnd, UTF8ToUTF16("이미 존재하는 ID입니다.").c_str(), UTF8ToUTF16("에러").c_str(), MB_OK | MB_ICONERROR);
            }
            break;
        }
        case IDC_BTN_BORROW: {
            int selectedRow = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
            if (selectedRow == -1) {
                MessageBoxW(hWnd, UTF8ToUTF16("선택된 도서가 없습니다.").c_str(), UTF8ToUTF16("경고").c_str(), MB_OK | MB_ICONWARNING);
                break;
            }
            wchar_t idBuf[32];
            LVITEMW lvi = { 0 };
            lvi.iSubItem = 0;
            lvi.pszText = idBuf;
            lvi.cchTextMax = 32;
            SendMessageW(hListView, LVM_GETITEMTEXTW, selectedRow, (LPARAM)&lvi);

            g_manager.toggleBorrow(_wtoi(idBuf));
            UpdateListView();
            break;
        }
        case IDC_BTN_DELETE: {
            int selectedRow = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
            if (selectedRow == -1) {
                MessageBoxW(hWnd, UTF8ToUTF16("삭제할 도서를 선택하세요.").c_str(), UTF8ToUTF16("경고").c_str(), MB_OK | MB_ICONWARNING);
                break;
            }
            wchar_t idBuf[32];
            LVITEMW lvi = { 0 };
            lvi.iSubItem = 0;
            lvi.pszText = idBuf;
            lvi.cchTextMax = 32;
            SendMessageW(hListView, LVM_GETITEMTEXTW, selectedRow, (LPARAM)&lvi);

            g_manager.deleteBook(_wtoi(idBuf));
            UpdateListView();
            MessageBoxW(hWnd, UTF8ToUTF16("도서가 삭제되었습니다.").c_str(), UTF8ToUTF16("알림").c_str(), MB_OK);
            break;
        }
        case IDC_BTN_SAVE: {
            if (g_manager.saveToFile()) {
                MessageBoxW(hWnd, UTF8ToUTF16("books_data.csv 파일에 저장되었습니다.").c_str(), UTF8ToUTF16("성공").c_str(), MB_OK);
            }
            break;
        }
        case IDC_BTN_OPEN_FOLDER: {
            // ShellExecuteW를 사용하여 직접 파일탐색기 열기 (빠르고 깔끔함)
            ShellExecuteW(NULL, L"explore", L".", NULL, NULL, SW_SHOW);
            break;
        }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    const wchar_t CLASS_NAME[] = L"BookManagerWindowClass";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, UTF8ToUTF16("도서관 관리 시스템").c_str(),
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 430, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}