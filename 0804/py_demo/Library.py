import os            # 파일/디렉토리 존재 여부 확인 등 운영체제 기능 제공 모듈
import csv           # CSV(Comma-Separated Values) 파일 읽기/쓰기를 위한 모듈
import subprocess    # 외부 프로그램(예: 폴더 열기 명령어) 실행을 위한 모듈
import customtkinter as ctk  # Tkinter 기반의 현대적인 UI 그래픽 라이브러리
from tkinter import messagebox  # 경고/알림 팝업창을 띄우기 위해 기본 Tkinter 모듈 가져오기

# ============================================================================
# 1. Book 클래스 (데이터 모델)
# ============================================================================
class Book:
    """
    [도서 데이터 모델 클래스]
    한 권의 도서가 가져야 할 속성(ID, 제목, 저자, 대출여부)을 관리합니다.
    """
    def __init__(self, book_id: int, title: str, author: str, is_borrowed: bool = False):
        # 객체가 생성될 때 초기화되는 속성들
        self.id = book_id            # 도서 고유 번호 (정수)
        self.title = title          # 도서 제목 (문자열)
        self.author = author        # 저자 이름 (문자열)
        self.is_borrowed = is_borrowed  # 대출 여부 (True: 대출 중, False: 대출 가능)

    def get_status_str(self) -> str:
        """
        현재 대출 상태(Boolean)를 사람이 읽기 쉬운 한글 문자열로 반환하는 메서드
        """
        return "대출 중" if self.is_borrowed else "대출 가능"


# ============================================================================
# 2. BookManager 클래스 (데이터 관리 및 파일 저장/로딩 로직)
# ============================================================================
class BookManager:
    """
    [도서 관리 자 클래스]
    여러 Book 객체들을 리스트로 관리하며, CSV 파일과의 연동(불러오기/저장하기) 및
    도서의 추가, 삭제, 대출 상태 변경 등의 비즈니스 로직을 처리합니다.
    """
    def __init__(self, filename="books_data.csv"):
        self.filename = filename  # 저장 및 불러올 CSV 파일 이름
        self.books = []           # Book 객체들을 담을 리스트 (메모리 내 데이터 저장소)
        self.load_from_file()     # 프로그램 시작 시 파일에서 기존 데이터를 불러옴

    def load_from_file(self):
        """
        books_data.csv 파일에서 데이터를 읽어와 self.books 리스트에 Book 객체로 저장합니다.
        """
        # 파일이 존재하지 않으면 읽지 않고 메서드 종료
        if not os.path.exists(self.filename):
            return

        self.books.clear()  # 기존 메모리에 있던 데이터 초기화
        try:
            # utf-8 인코딩으로 파일 열기
            with open(self.filename, mode='r', encoding='utf-8') as f:
                reader = csv.reader(f)
                header = next(reader, None)  # 첫 번째 줄(헤더: ID,제목,저자,상태) 건너뛰기
                
                # 파일의 각 행을 읽으며 Book 객체 생성
                for row in reader:
                    # 빈 줄이거나 데이터가 부족한 줄은 건너뜀 (예외 방지)
                    if not row or len(row) < 4:
                        continue
                    
                    # 각 열의 데이터를 적절한 타입으로 변환 및 공백 제거(.strip())
                    book_id = int(row[0].strip())
                    title = row[1].strip()
                    author = row[2].strip()
                    is_borrowed = (row[3].strip() == "대출 중")  # 문자열 비교를 통해 bool 값 결정
                    
                    # 객체 생성 후 리스트에 추가
                    self.books.append(Book(book_id, title, author, is_borrowed))
        except Exception as e:
            print(f"파일 읽기 오류: {e}")

    def save_to_file(self) -> bool:
        """
        메모리(self.books)의 도서 데이터를 books_data.csv 파일에 덮어씁니다.
        """
        try:
            # newline='' 옵션은 Windows 환경에서 줄바꿈이 두 번 되는 현상을 방지함
            with open(self.filename, mode='w', encoding='utf-8', newline='') as f:
                writer = csv.writer(f)
                # 1. 파일 최상단에 헤더(컬럼명) 쓰기
                writer.writerow(["ID", "제목", "저자", "상태"])
                # 2. 모든 도서 정보를 행 단위로 쓰기
                for book in self.books:
                    writer.writerow([book.id, book.title, book.author, book.get_status_str()])
            return True
        except Exception as e:
            print(f"파일 저장 오류: {e}")
            return False

    def add_book(self, book_id: int, title: str, author: str) -> bool:
        """
        신규 도서를 추가합니다. (중복 ID 검사 포함)
        """
        # any(): 리스트 중 하나라도 조건을 만족하면 True 반환 (ID 중복 체크)
        if any(b.id == book_id for b in self.books):
            return False  # 이미 존재하는 ID면 실패 처리
        
        # 새로운 Book 객체 생성 및 리스트 추가
        self.books.append(Book(book_id, title, author))
        self.save_to_file()  # 변경 사항 파일에 즉시 반영
        return True

    def toggle_borrow(self, book_id: int) -> bool:
        """
        선택한 도서의 대출 상태를 반전시킵니다. (대출 가능 <-> 대출 중)
        """
        for book in self.books:
            if book.id == book_id:
                book.is_borrowed = not book.is_borrowed  # True/False 반전
                self.save_to_file()  # 변경 사항 파일에 자동 저장
                return True
        return False

    def delete_book(self, book_id: int) -> bool:
        """
        ID를 기준으로 도서를 삭제합니다.
        """
        # enumerate(): 인덱스(i)와 요소(book)를 동시에 가져옴
        for i, book in enumerate(self.books):
            if book.id == book_id:
                del self.books[i]    # 리스트에서 해당 요소 제거
                self.save_to_file()  # 변경 사항 파일에 자동 저장
                return True
        return False


# ============================================================================
# 3. CustomTkinter GUI 클래스 (사용자 인터페이스)
# ============================================================================
class BookManagerApp(ctk.CTk):
    """
    [GUI 창 및 화면 제어 클래스]
    ctk.CTk 클래스를 상속받아 프로그램의 메인 윈도우 창을 만들고 이벤트 및 UI 요소를 관리합니다.
    """
    def __init__(self):
        super().__init__()  # 부모 클래스(ctk.CTk)의 생성자 호출

        self.manager = BookManager()  # 데이터 관리를 전담할 BookManager 객체 생성

        # --- UI 창 기본 디자인 설정 ---
        ctk.set_appearance_mode("System")  # 운영체제 설정에 맞춰 다크/라이트 모드 자동 전환
        ctk.set_default_color_theme("blue") # 테마 색상 설정 (blue, green, dark-blue)

        # --- 창 테두리 및 크기 설정 ---
        self.title("도서관 관리 시스템")
        self.geometry("820x460")     # 가로 820px, 세로 460px
        self.resizable(False, False) # 사용자가 창 크기를 조절하지 못하도록 고정

        # --- 위젯(컴포넌트) 생성 및 화면 초기화 ---
        self.create_widgets()
        self.update_list_view()

    def create_widgets(self):
        """
        화면에 배치될 레이아웃 및 위젯(버튼, 입력창, 목록)들을 생성합니다.
        """
        # --------------------------------------------------------------------
        # 영역 1: 도서 목록 출력 영역 (스크롤이 가능한 프레임)
        # --------------------------------------------------------------------
        self.list_frame = ctk.CTkScrollableFrame(self, width=760, height=220)
        self.list_frame.pack(padx=20, pady=(20, 10))

        # 목록의 컬럼 제목(헤더) 생성 [헤더이름, 너비]
        headers = [("ID", 80), ("제목", 320), ("저자", 200), ("상태", 120)]
        header_frame = ctk.CTkFrame(self.list_frame, fg_color="transparent")
        header_frame.pack(fill="x", pady=(0, 5))

        for text, width in headers:
            lbl = ctk.CTkLabel(header_frame, text=text, width=width, font=ctk.CTkFont(weight="bold"))
            lbl.pack(side="left", padx=2)

        # 실제 도서 데이터 행(Row)들이 들어갈 프레임
        self.rows_frame = ctk.CTkFrame(self.list_frame, fg_color="transparent")
        self.rows_frame.pack(fill="x")

        # 현재 사용자가 클릭해서 선택한 도서 정보를 저장할 변수
        self.selected_book_id = None
        self.selected_row_widget = None

        # --------------------------------------------------------------------
        # 영역 2: 도서 입력 폼 영역 (ID, 제목, 저자 입력창 및 등록 버튼)
        # --------------------------------------------------------------------
        input_frame = ctk.CTkFrame(self, fg_color="transparent")
        input_frame.pack(padx=20, pady=10, fill="x")

        # ID 입력 부분
        ctk.CTkLabel(input_frame, text="ID:").pack(side="left", padx=(5, 2))
        self.entry_id = ctk.CTkEntry(input_frame, width=80)
        self.entry_id.pack(side="left", padx=(0, 15))

        # 제목 입력 부분
        ctk.CTkLabel(input_frame, text="제목:").pack(side="left", padx=(0, 2))
        self.entry_title = ctk.CTkEntry(input_frame, width=220)
        self.entry_title.pack(side="left", padx=(0, 15))

        # 저자 입력 부분
        ctk.CTkLabel(input_frame, text="저자:").pack(side="left", padx=(0, 2))
        self.entry_author = ctk.CTkEntry(input_frame, width=150)
        self.entry_author.pack(side="left", padx=(0, 15))

        # 도서 등록 버튼 (클릭 시 self.on_add_book 실행)
        self.btn_add = ctk.CTkButton(input_frame, text="도서 등록", width=120, command=self.on_add_book)
        self.btn_add.pack(side="left", padx=(5, 0))

        # --------------------------------------------------------------------
        # 영역 3: 하단 기능 버튼 영역 (대출/반납, 삭제, 저장, 폴더 열기)
        # --------------------------------------------------------------------
        btn_frame = ctk.CTkFrame(self, fg_color="transparent")
        btn_frame.pack(padx=20, pady=10, fill="x")

        self.btn_borrow = ctk.CTkButton(btn_frame, text="선택 도서 대출 / 반납", width=170, height=35, command=self.on_toggle_borrow)
        self.btn_borrow.pack(side="left", padx=(0, 10))

        # 삭제 버튼은 주의가 필요하므로 빨간색 계열(fg_color)로 강조 디자인
        self.btn_delete = ctk.CTkButton(btn_frame, text="선택 도서 삭제", width=170, height=35, fg_color="#D32F2F", hover_color="#9A0007", command=self.on_delete_book)
        self.btn_delete.pack(side="left", padx=(0, 10))

        self.btn_save = ctk.CTkButton(btn_frame, text="파일 저장", width=170, height=35, command=self.on_save_file)
        self.btn_save.pack(side="left", padx=(0, 10))

        self.btn_open_folder = ctk.CTkButton(btn_frame, text="폴더 열기", width=170, height=35, command=self.on_open_folder)
        self.btn_open_folder.pack(side="left")

    def update_list_view(self):
        """
        BookManager의 데이터 상태를 바탕으로 화면 목록 UI를 새로 고칩니다.
        (추가, 삭제, 상태 변경 시 호출됨)
        """
        # 기존 화면에 그려져 있던 목록 요소(자식 위젯)들을 전부 삭제
        for child in self.rows_frame.winfo_children():
            child.destroy()

        # 선택 상태 초기화
        self.selected_book_id = None
        self.selected_row_widget = None

        # 데이터 개수만큼 반복하며 행(Row) 생성
        for book in self.manager.books:
            row = ctk.CTkFrame(self.rows_frame)
            row.pack(fill="x", pady=2)

            # [이벤트 바인딩] 행 전체나 행 내부의 라벨 클릭 시 해당 도서가 선택되도록 함
            row.bind("<Button-1>", lambda e, b_id=book.id, r=row: self.select_row(b_id, r))

            # ID 라벨
            lbl_id = ctk.CTkLabel(row, text=str(book.id), width=80)
            lbl_id.pack(side="left", padx=2)
            lbl_id.bind("<Button-1>", lambda e, b_id=book.id, r=row: self.select_row(b_id, r))

            # 제목 라벨 (왼쪽 정렬: anchor="w")
            lbl_title = ctk.CTkLabel(row, text=book.title, width=320, anchor="w")
            lbl_title.pack(side="left", padx=2)
            lbl_title.bind("<Button-1>", lambda e, b_id=book.id, r=row: self.select_row(b_id, r))

            # 저자 라벨
            lbl_author = ctk.CTkLabel(row, text=book.author, width=200, anchor="w")
            lbl_author.pack(side="left", padx=2)
            lbl_author.bind("<Button-1>", lambda e, b_id=book.id, r=row: self.select_row(b_id, r))

            # 대출 상태 라벨
            lbl_status = ctk.CTkLabel(row, text=book.get_status_str(), width=120)
            lbl_status.pack(side="left", padx=2)
            lbl_status.bind("<Button-1>", lambda e, b_id=book.id, r=row: self.select_row(b_id, r))

    def select_row(self, book_id: int, row_widget: ctk.CTkFrame):
        """
        사용자가 목록 중 하나의 행을 클릭했을 때 시각적으로 하이라이트(색상 변경) 처리
        """
        # 이전에 선택되어 있던 행이 있다면 원래 색상으로 복원
        if self.selected_row_widget and self.selected_row_widget.winfo_exists():
            self.selected_row_widget.configure(fg_color=["#F9F9F9", "#2B2B2B"])

        # 현재 선택한 도서 ID 저장 및 행 배경색 변경 (파란색 하이라이트)
        self.selected_book_id = book_id
        self.selected_row_widget = row_widget
        row_widget.configure(fg_color=["#3B8ED0", "#1F538D"])

    # ------------------------------------------------------------------------
    # [버튼 이벤트 핸들러 메서드 모음]
    # ------------------------------------------------------------------------
    def on_add_book(self):
        """'도서 등록' 버튼을 클릭했을 때 처리 로직"""
        # 입력 필드에서 값 가져오기 및 좌우 공백 제거
        id_str = self.entry_id.get().strip()
        title = self.entry_title.get().strip()
        author = self.entry_author.get().strip()

        # 1. 예외 처리: 미입력 값 체크
        if not id_str or not title or not author:
            messagebox.showwarning("경고", "모든 항목을 입력하세요.")
            return

        # 2. 예외 처리: ID가 숫자인지 체크
        if not id_str.isdigit():
            messagebox.showwarning("경고", "ID는 숫자만 입력할 수 있습니다.")
            return

        book_id = int(id_str)
        # 데이터 추가 시도
        if self.manager.add_book(book_id, title, author):
            messagebox.showinfo("알림", "도서가 등록되었습니다.")
            # 입력 칸 비우기 (0번 인덱스부터 끝('end')까지 삭제)
            self.entry_id.delete(0, 'end')
            self.entry_title.delete(0, 'end')
            self.entry_author.delete(0, 'end')
            self.update_list_view()  # 화면 목록 갱신
        else:
            messagebox.showerror("에러", "이미 존재하는 ID입니다.")

    def on_toggle_borrow(self):
        """'대출/반납' 버튼을 클릭했을 때 처리 로직"""
        if self.selected_book_id is None:
            messagebox.showwarning("경고", "선택된 도서가 없습니다.")
            return

        self.manager.toggle_borrow(self.selected_book_id)
        self.update_list_view()  # 대출 상태 변경 반영을 위해 화면 갱신

    def on_delete_book(self):
        """'도서 삭제' 버튼을 클릭했을 때 처리 로직"""
        if self.selected_book_id is None:
            messagebox.showwarning("경고", "삭제할 도서를 선택하세요.")
            return

        self.manager.delete_book(self.selected_book_id)
        self.update_list_view()  # 삭제 반영을 위해 화면 갱신
        messagebox.showinfo("알림", "도서가 삭제되었습니다.")

    def on_save_file(self):
        """'파일 저장' 버튼을 클릭했을 때 처리 로직"""
        if self.manager.save_to_file():
            messagebox.showinfo("성공", "books_data.csv 파일에 저장되었습니다.")

    def on_open_folder(self):
        """'폴더 열기' 버튼을 클릭했을 때 처리 로직"""
        current_dir = os.getcwd()  # 현재 프로그램이 위치한 경로 구하기
        
        # 운영체제(OS)별로 탐색기를 열어주는 명령 분기 처리
        if os.name == 'nt':  # Windows 운영체제
            os.startfile(current_dir)
        else:  # macOS 및 Linux
            subprocess.run(["open" if os.name == "posix" else "xdg-open", current_dir])


# ============================================================================
# 프로그램 진입점 (Main Execution)
# ============================================================================
if __name__ == "__main__":
    # 이 파일이 직접 실행될 때만 GUI 앱을 생성하고 루프 실행
    app = BookManagerApp()
    app.mainloop()  # 사용자가 창을 닫을 때까지 이벤트(클릭, 입력 등)를 대기하는 무한 루프