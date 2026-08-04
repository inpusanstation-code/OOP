import os
import sys
import subprocess

def install_and_build():
    # 1. PyInstaller 설치 확인 및 설치
    print("📦 PyInstaller 확인 중...")
    try:
        import PyInstaller
    except ImportError:
        print("⚡ PyInstaller가 설치되어 있지 않아 설치를 진행합니다...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "pyinstaller"])

    # 2. CustomTkinter 에셋 경로 자동 탐색
    # (CustomTkinter는 테마 json 및 폰트 파일을 포함하므로 PyInstaller에 해당 경로를 전달해야 함)
    import customtkinter
    ctk_path = os.path.dirname(customtkinter.__file__)

    # OS별 경로 구분자 처리 (Windows는 ';')
    add_data_opt = f"{ctk_path}{os.pathsep}customtkinter/"

    print("🚀 .exe 파일 생성(빌드) 시작...")

    # 3. PyInstaller 실행 인자 구성
    pyinstaller_args = [
        "calc_py.py",                  # 1) 변환할 파이썬 파일명
        "--onefile",                   # 2) 단일 .exe 파일로 압축
        "--noconsole",                 # 3) 까만 콘솔 창을 띄우지 않고 GUI만 출력
        f"--add-data={add_data_opt}",  # 4) CustomTkinter 테마 파일 함께 패키징
        "--clean",                     # 5) 빌드 전 임시 파일 정리
        "--name=ModernCalculator"      # 6) 생성될 .exe 파일 이름 설정
    ]

    # 4. PyInstaller 빌드 명령어 실행
    import PyInstaller.__main__
    PyInstaller.__main__.run(pyinstaller_args)

    print("\n✅ 빌드가 완료되었습니다!")
    print(f"📁 생성된 실행 파일 위치: {os.path.abspath('dist/ModernCalculator.exe')}")

if __name__ == "__main__":
    install_and_build()