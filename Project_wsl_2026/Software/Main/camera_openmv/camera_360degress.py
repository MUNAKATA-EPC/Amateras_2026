import sensor
import time
from pyb import UART
import math

#############################################################
# ゴールの色取り用変数(黄色)
#goal_yellow = (6, 92, -22, 4, 43, 93) #福岡ﾉｳﾄﾞ
goal_yellow = (64, 93, -28, -2, 29, 64) #学校
#############################################################
# ゴールの色取り用変数(青色)
#goal_blue = (36, 21, -26, 0, -13, -5) #福岡ﾉｳﾄﾞ
goal_blue = (18, 38, -10, 13, -10, -26) #学校
#############################################################
# コートの色（カーペット用）
#court_green = (44, 75, -55, -29, 9, 40) #福岡ﾉｳﾄﾞ
court_green = (42, 87, -22, 1, -5, 19) #学校
#############################################################
# 画面の中央座標
screen_center = [156, 90] # QVGA(320x240)なら160, 120が中央。設定に合わせる
screen_short_r = 30
screen_long_r = 164

# -----------------------------------
# センサー設定
MANUAL_EXPOSURE = 20000
MANUAL_RGB_GAIN = (1.0, 1.0, 1.0)
MANUAL_GAIN_DB = 3

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_whitebal(False, rgb_gain_db=MANUAL_RGB_GAIN)
sensor.set_auto_exposure(False, exposure_us=MANUAL_EXPOSURE)
sensor.set_auto_gain(False, gain_db=MANUAL_GAIN_DB)
sensor.set_brightness(10)

clock = time.clock()
uart = UART(3, 115200, timeout_char=1000)

# -----------------------------------
# 角度・距離計算の共通関数
def calculate_pos(blob, center):
    # 中心からの相対座標
    dx = blob.cx() - center[0]
    dy = blob.cy() - center[1]
    # 距離計算
    dist = math.sqrt(dx**2 + dy**2)
    # 角度計算 (ロボットの前方を0度とするための調整を含む)
    # atan2(x, y) で、y方向を基準に計算し、反転・180度オフセットを適用
    deg = int(math.atan2(-dx, -dy) * 180 / math.pi)
    return deg, int(dist)

# 2バイトのデータ送信関数
def send_int16(uart, value):
    # 0xFFとの重複回避処理は、本来は受信側と合わせたプロトコルが必要
    low = value & 0xFF
    high = (value >> 8) & 0xFF
    uart.write(bytearray([low, high]))

# -----------------------------------
while True:
    clock.tick()
    img = sensor.snapshot()

    # 画面外周を黒く塗る（計算対象外のノイズを消す）
    img.draw_circle(screen_center[0], screen_center[1], screen_long_r, [0, 0, 0], 130)
    img.draw_circle(screen_center[0], screen_center[1], screen_short_r, [0, 0, 0], 1, fill=True)
    img.draw_cross(screen_center[0], screen_center[1])

    # 初期値（未検出時は 0xFF などの特定値を送る設定）
    court_deg = 0
    yellow_deg, yellow_dis = 255, 255
    blue_deg, blue_dis = 255, 255

    # -----------------------------------
    # 青ゴールの検出 (最大のBlobのみを採用)
    max_blue_area = 0
    for blob in img.find_blobs([goal_blue], pixels_threshold=10, area_threshold=10, merge=True, margin=25):
        if blob.area() > max_blue_area:
            max_blue_area = blob.area()
            blue_deg, blue_dis = calculate_pos(blob, screen_center)
            img.draw_rectangle(blob.rect())
            img.draw_line(screen_center[0], screen_center[1], blob.cx(), blob.cy(), color=(0, 0, 255))
            blue_deg = -blue_deg #反転

    # -----------------------------------
    # 黄ゴールの検出 (最大のBlobのみを採用)
    max_yellow_area = 0
    for blob in img.find_blobs([goal_yellow], pixels_threshold=10, area_threshold=10, merge=True, margin=25):
        if blob.area() > max_yellow_area:
            max_yellow_area = blob.area()
            yellow_deg, yellow_dis = calculate_pos(blob, screen_center)
            img.draw_rectangle(blob.rect())
            img.draw_line(screen_center[0], screen_center[1], blob.cx(), blob.cy(), color=(255, 255, 0))
            yellow_deg = -yellow_deg #反転

    # -----------------------------------
    # コート（緑）の検出
    max_court_area = 0
    for blob in img.find_blobs([court_green], pixels_threshold=50, area_threshold=50, merge=True, margin=25):
        if blob.area() > max_court_area:
            max_court_area = blob.area()
            court_deg, _ = calculate_pos(blob, screen_center)
            img.draw_line(screen_center[0], screen_center[1], blob.cx(), blob.cy(), color=(0, 255, 0))
            court_deg = -court_deg #反転

    # -----------------------------------
    # UART送信 (0x55: 開始, 0xAA: 終了)
    uart.write(bytearray([0x55]))
    send_int16(uart, court_deg)
    send_int16(uart, yellow_deg)
    send_int16(uart, yellow_dis)
    send_int16(uart, blue_deg)
    send_int16(uart, blue_dis)
    uart.write(bytearray([0xAA]))

    # デバッグ出力
    #print("yellow[deg:%d, dis:%d] blue[deg:%d, dis:%d] court[deg:%d]" % (yellow_deg, yellow_dis, blue_deg, blue_dis, court_deg))
