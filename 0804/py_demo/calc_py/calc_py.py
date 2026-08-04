import customtkinter as ctk

# 기본 테마 설정 (System, Dark, Light 중 선택 가능)
ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

class ModernCalculator(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("Calculator")
        self.geometry("285x380")
        self.resizable(False, False)

        # 상태 변수
        self.first_num = 0.0
        self.current_op = ""
        self.is_new_input = True

        # 1. 화면 (Display) - CTkEntry 생성자 안에 width와 height를 함께 전달
        self.display_var = ctk.StringVar(value="0")
        self.display = ctk.CTkEntry(
            self,
            textvariable=self.display_var,
            font=("Consolas", 20, "bold"),
            justify="right",
            width=245,  # 👈 너비 지정
            height=50,   # 👈 높이 지정
            state="readonly"
        )
        self.display.place(x=20, y=20) # 👈 좌표만 지정

        # 2. 버튼 그리드
        btn_labels = [
            ['7', '8', '9', '/'],
            ['4', '5', '6', '*'],
            ['1', '2', '3', '-'],
            ['C', '0', '=', '+']
        ]

        for i in range(4):
            for j in range(4):
                text = btn_labels[i][j]
                
                # 버튼 특성에 맞춘 색상 구분 (연산자, C 버튼 강조)
                fg_color = "#3B82F6" if text in ['+', '-', '*', '/', '='] else None
                if text == 'C':
                    fg_color = "#EF4444"

                # CTkButton 역시 width, height를 생성자 내부에서 전달
                btn = ctk.CTkButton(
                    self,
                    text=text,
                    font=("Arial", 16, "bold"),
                    fg_color=fg_color,
                    width=55,   # 👈 너비
                    height=55,  # 👈 높이
                    corner_radius=8,
                    command=lambda t=text: self.on_button_click(t)
                )
                btn.place(x=20 + j * 62, y=90 + i * 62)

    def on_button_click(self, char):
        current_text = self.display_var.get()

        if char.isdigit():
            if self.is_new_input or current_text == "0":
                self.display_var.set(char)
                self.is_new_input = False
            else:
                self.display_var.set(current_text + char)

        elif char in ['+', '-', '*', '/']:
            try:
                self.first_num = float(current_text)
            except ValueError:
                self.first_num = 0.0
            self.current_op = char
            self.is_new_input = True

        elif char == '=':
            if not self.current_op:
                return
            try:
                second_num = float(current_text)
            except ValueError:
                second_num = 0.0

            result = 0.0
            if self.current_op == '+': result = self.first_num + second_num
            elif self.current_op == '-': result = self.first_num - second_num
            elif self.current_op == '*': result = self.first_num * second_num
            elif self.current_op == '/':
                if second_num == 0:
                    self.display_var.set("0으로 나눌 수 없음")
                    self.is_new_input = True
                    self.current_op = ""
                    return
                result = self.first_num / second_num

            result_str = str(int(result)) if result.is_integer() else f"{result:g}"
            self.display_var.set(result_str)
            self.is_new_input = True
            self.current_op = ""

        elif char == 'C':
            self.display_var.set("0")
            self.first_num = 0.0
            self.current_op = ""
            self.is_new_input = True

if __name__ == "__main__":
    app = ModernCalculator()
    app.mainloop()