import tkinter as tk
from fractions import Fraction
import re

# 분수 계산 함수
def calculate():
    expression = entry.get().replace(" ", "")

    try:
        # 1/2 -> Fraction(1,2)
        expression = re.sub(
            r'(?<![\d.])(\d+)/(\d+)(?![\d.])',
            r'Fraction(\1,\2)',
            expression
        )

        result = eval(expression, {"Fraction": Fraction})

        if isinstance(result, Fraction):
            if result.denominator == 1:
                result_label.config(text=f"결과 : {result.numerator}")
            else:
                result_label.config(
                    text=f"결과 : {result} ({float(result)})"
                )
        else:
            result_label.config(text=f"결과 : {result}")

    except ZeroDivisionError:
        result_label.config(text="0으로 나눌 수 없습니다.")
    except Exception as e:
        result_label.config(text=f"오류 : {e}")
        
# 버튼 클릭
def click(value):
    entry.insert(tk.END, value)

# 입력 지우기
def clear():
    entry.delete(0, tk.END)
    result_label.config(text="결과 : ")

# GUI
root = tk.Tk()
root.title("분수 계산기")
root.geometry("360x500")
root.resizable(False, False)

entry = tk.Entry(root, font=("Arial", 20), justify="right")
entry.pack(fill="x", padx=10, pady=10)

result_label = tk.Label(root, text="결과 : ", font=("Arial", 16))
result_label.pack(pady=10)

buttons = [
    ['7','8','9','/'],
    ['4','5','6','*'],
    ['1','2','3','-'],
    ['0','.','+','='],
    ['(',')','C']
]

for row in buttons:
    frame = tk.Frame(root)
    frame.pack(expand=True, fill="both")

    for btn in row:
        if btn == "=":
            command = calculate
        elif btn == "C":
            command = clear
        else:
            command = lambda x=btn: click(x)

        tk.Button(
            frame,
            text=btn,
            font=("Arial", 18),
            command=command
        ).pack(side="left", expand=True, fill="both")

root.mainloop()