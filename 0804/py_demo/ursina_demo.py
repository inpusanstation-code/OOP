from ursina import *
from random import uniform  # 무작위 색상 및 좌표용

# 1. 게임 엔진 초기화 및 앱 생성
app = Ursina()

# --- 2. 기본 배경 및 조명 설정 ---
window.color = color.gray

# 기본 조명 추가
direction_light = DirectionalLight()
direction_light.look_at(Vec3(1, -1, -1))

# --- 3. 3D 물체 (Entity) 생성 및 설정 ---
# 회전하는 큐브 Entity
cube = Entity(
    model='cube',            # 물체의 모양 ('cube', 'sphere', 'plane' 등)
    color=color.orange,      # 기본 색상
    texture='white_cube',    # 기본 텍스처 (격자무늬)
    scale=2,                 # 크기 배율
    position=(0, 0, 0),       # 초기 위치 (x, y, z)
    collider='box'           # 마우스 클릭 충돌 감지용 콜라이더 추가
)

# 클릭했을 때 나타날 텍스트 Entity
click_text = Text(
    text='Click!',
    origin=(0, 0),           # 텍스트 기준점 (중앙)
    scale=3,
    color=color.white,
    position=(0, 0.25),      # 화면 중앙에서 살짝 위
    enabled=False            # 초기에는 숨김 처리
)

# --- 4. 게임 루프 (Update) 정의 ---
def update():
    # 큐브를 매 프레임 조금씩 회전시킴
    cube.rotation_x += 10 * time.dt
    cube.rotation_y += 30 * time.dt

# --- 5. 마우스 입력 (Input) 이벤트 정의 ---
def input(key):
    if key == 'left mouse down':  # 마우스 왼쪽 버튼 클릭 시
        # 마우스가 큐브 위에 있다면
        if mouse.hovered_entity == cube:
            # 💡 수정된 부분: color.color -> color.hsv
            cube.color = color.hsv(
                uniform(0, 360),
                1,
                uniform(0.5, 1)
            )
            
            # 클릭 텍스트 노출 및 페이드 아웃
            click_text.enabled = True
            click_text.fade_out(duration=0.15)
            invoke(click_text.disable, delay=0.15)

# --- 6. 실행 ---
app.run()