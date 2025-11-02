import sensor
import time
import image
from pyb import UART
import math

#############################################################
# ゴールの色取り用変数(黄色)
goal_yellow = (45, 100, -36, 15, 26, 127)
#############################################################
# ゴールの色取り用変数(青色)
goal_blue = (26, 65, -11, 25, -56, -31)
#############################################################
# コートの色（カーペット用）
court_green = (58, 79, -13, 0, -8, 16)
#############################################################
# 画面の中央座標
screen_center = [136, 120]
screen_short_r = 41
screen_long_r = 170

court = [0, 0]
yellow = [0, 0]
blue = [0, 0]

frame_count = 0  # フレームカウント

# 青ゴール検出用変数
count_goal_blue = 0
blue_num = 0
deg_goal_blue = [0, 0]
dis_goal_blue = [0, 0]
area_goal_blue = [0, 0]

# 黄ゴール検出用変数
count_goal_yellow = 0
yellow_num = 0
deg_goal_yellow = [0, 0]
dis_goal_yellow = [0, 0]
area_goal_yellow = [0, 0]

# コート（緑）検出用変数
count_court_green = 0
court_green_num = 0
deg_court_green = [0, 0]
area_court_green = [0, 0]
court_pos = None  # 最新のコート座標を保持

# -----------------------------------
# センサー設定
sensor.reset()                     # センサーをリセットして初期化
sensor.set_pixformat(sensor.RGB565)# ピクセルフォーマットをRGB565に設定
sensor.set_framesize(sensor.QVGA)  # フレームサイズをQVGA (320x240)に設定
sensor.skip_frames(time=2000)      # 設定が有効になるまで待機

# 明るさ関連の設定（ここで反映される）
# sensor.set_auto_gain(False, gain_db=8)             # 自動ゲインオフ、ゲインを低めに
# sensor.set_auto_whitebal(False)                    # ホワイトバランス固定
# sensor.set_auto_exposure(False, exposure_us=1800)  # 露出を短くして暗めに
# sensor.set_brightness(0)                           # さらに暗く（-3 〜 +3）

clock = time.clock()
uart = UART(3, 115200, timeout_char=1000)

# -----------------------------------
# 2バイトのデータ送信関数
def send_int16(uart, value):
    if value == 0xFF:       # 0xFFはそのまま
        value_to_send = 0xFF
    else:
        value_to_send = value
    low = value_to_send & 0xFF
    high = (value_to_send >> 8) & 0xFF
    uart.write(bytearray([low, high]))
# -----------------------------------
while True:
    clock.tick()
    frame_count += 1
    img = sensor.snapshot()
    img.draw_cross(screen_center[0], screen_center[1])

    # 画面外周を黒く塗る
    img.draw_circle(screen_center[0], screen_center[1], screen_long_r, [0, 0, 0], 130)
    img.draw_circle(screen_center[0], screen_center[1], screen_short_r, [0, 0, 0], 1, 1)

    # 各方向と距離の初期化
    court_deg_temp = 0

    yellow_detected = False
    yellow_deg = 0xFF
    yellow_dis = 0xFF

    blue_detected = False
    blue_deg = 0xFF
    blue_dis = 0xFF

    # -----------------------------------
    # 青ゴールの検出
    for blob in img.find_blobs([goal_blue], pixels_threshold=10, area_threshold=10, merge=True, margin=25):
        if count_goal_blue >= 2:
            count_goal_blue = 0
        blue[0] = blob.cx()
        blue[1] = blob.cy()
        blue_dis = math.sqrt((blue[0] - screen_center[0])**2 + (blue[1] - screen_center[1])**2)
        blue_deg = math.atan2(blue[0] - screen_center[0], blue[1] - screen_center[1]) * 180 / math.pi
        deg_goal_blue[count_goal_blue] = blue_deg
        dis_goal_blue[count_goal_blue] = blue_dis
        area_goal_blue[count_goal_blue] = blob.area()
        blue_num = area_goal_blue.index(max(area_goal_blue[:]))
        blue_deg = deg_goal_blue[blue_num]
        blue_dis = dis_goal_blue[blue_num]
        count_goal_blue += 1

        img.draw_rectangle(blob.rect())
        img.draw_line(screen_center[0], screen_center[1], blue[0], blue[1], color=(0, 0, 255), thickness=2)

        blue_detected = True
    # -----------------------------------
    # 黄ゴールの検出
    for blob in img.find_blobs([goal_yellow], pixels_threshold=10, area_threshold=10, merge=True, margin=25):
        if count_goal_yellow >= 2:
            count_goal_yellow = 0
        yellow[0] = blob.cx()
        yellow[1] = blob.cy()
        yellow_dis = math.sqrt((yellow[0] - screen_center[0])**2 + (yellow[1] - screen_center[1])**2)
        yellow_deg = math.atan2(yellow[0] - screen_center[0], yellow[1] - screen_center[1]) * 180 / math.pi
        deg_goal_yellow[count_goal_yellow] = yellow_deg
        dis_goal_yellow[count_goal_yellow] = yellow_dis
        area_goal_yellow[count_goal_yellow] = blob.area()
        yellow_num = area_goal_yellow.index(max(area_goal_yellow[:]))
        yellow_deg = deg_goal_yellow[yellow_num]
        yellow_dis = dis_goal_yellow[yellow_num]
        count_goal_yellow += 1

        img.draw_rectangle(blob.rect())
        img.draw_line(screen_center[0], screen_center[1], yellow[0], yellow[1], color=(255, 255, 0), thickness=2)

        yellow_detected = True
    # -----------------------------------
    # コート検出
    found_court = False
    for blob in img.find_blobs([court_green], pixels_threshold=50, area_threshold=50, merge=True, margin=25):
        found_court = True
        if count_court_green >= 2:
            count_court_green = 0
        court[0] = blob.cx()
        court[1] = blob.cy()
        court_pos = (court[0], court[1])
        court_deg_temp = math.atan2(court[0] - screen_center[0], court[1] - screen_center[1]) * 180 / math.pi
        deg_court_green[count_court_green] = court_deg_temp
        area_court_green[count_court_green] = blob.area()
        court_green_num = area_court_green.index(max(area_court_green[:]))
        court_deg_temp = deg_court_green[court_green_num]
        court_deg = court_deg_temp
        count_court_green += 1

        img.draw_line(screen_center[0], screen_center[1], court[0], court[1], color=(0, 255, 0), thickness=2)
    # -----------------------------------

    # 整数化
    court_deg = int(-court_deg) # 反転させてint型に変換

    if yellow_detected == True:
        yellow_deg = int(-yellow_deg) # 反転させてint型に変換
        yellow_dis = int(yellow_dis)

    if blue_detected == True:
        blue_deg = int(-blue_deg) # 反転させてint型に変換
        blue_dis = int(blue_dis)

    # UART送信
    uart.write(bytearray([0xAA]))  # 同期ヘッダー
    send_int16(uart, court_deg)
    send_int16(uart, yellow_deg)
    send_int16(uart, yellow_dis)
    send_int16(uart, blue_deg)
    send_int16(uart, blue_dis)
