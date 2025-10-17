import sensor
import time
import image
from pyb import UART
import math

#############################################################
# ゴールの色取り用変数(黄色)
goal_yellow = (35, 54, -26, -8, 25, 127)
#############################################################
# ゴールの色取り用変数(青色)
goal_blue = (35, 54, -26, -8, 25, 127)
#############################################################
# コートの色（カーペット用）
court_green = (24, 33, -9, -5, 2, 12)
#############################################################
# 画面の中央座標
screen_center = [165, 88]

court = [0, 0]
yellow = [0, 0]
blue = [0, 0]

court_dir = 0
frame_count = 0  # フレームカウント

# 青ゴール検出用変数
count_goal_blue = 0
blue_num = 0
dir_goal_blue = [0, 0]
dis_goal_blue = [0, 0]
area_goal_blue = [0, 0]

# 黄ゴール検出用変数
count_goal_yellow = 0
yellow_num = 0
dir_goal_yellow = [0, 0]
dis_goal_yellow = [0, 0]
area_goal_yellow = [0, 0]

# コート（緑）検出用変数
count_court_green = 0
court_green_num = 0
dir_court_green = [0, 0]
area_court_green = [0, 0]
court_pos = None  # 最新のコート座標を保持

# -----------------------------------
# センサー設定
sensor.reset()                     # センサーをリセットして初期化
sensor.set_pixformat(sensor.RGB565)# ピクセルフォーマットをRGB565に設定
sensor.set_framesize(sensor.QVGA)  # フレームサイズをQVGA (320x240)に設定
sensor.skip_frames(time=2000)      # 設定が有効になるまで待機

# 明るさ関連の設定（ここで反映される）
sensor.set_auto_gain(False, gain_db=6)        # 自動ゲインオフ、ゲインを低めに
sensor.set_auto_whitebal(False)                # ホワイトバランス固定
sensor.set_auto_exposure(False, exposure_us=6000)  # 露出を短くして暗めに
sensor.set_brightness(0)                      # さらに暗く（-3 〜 +3）

clock = time.clock()
uart = UART(3, 115200, timeout_char=1000)

# -----------------------------------
# 2バイトのデータ送信関数
def send_int16(uart, value):
    if value == -1:       # -1はそのまま
        value_to_send = 0xFFFF
    else:
        value_to_send = value
    low = value_to_send & 0xFF
    high = (value_to_send >> 8) & 0xFF
    uart.write(bytearray([low, high]))

# -----------------------------------
# Teensy用角度変換
def convert_angle_for_teensy(angle):
    if angle == -1:
        return -1

    angle = (angle - 270 + 360)%360
    angle = 360 - angle

    return angle

# -----------------------------------
while True:
    clock.tick()
    frame_count += 1
    img = sensor.snapshot()
    img.draw_cross(screen_center[0], screen_center[1])

    # 画面外周を黒く塗る
    img.draw_circle(screen_center[0], screen_center[1], 190, [0, 0, 0], 130)
    img.draw_circle(screen_center[0], screen_center[1], 30, [0, 0, 0], 1, 1)

    # 各方向と距離の初期化
    yellow_dir = -1
    court_dir_temp = -1
    yellow_dis = -1
    blue_dir = -1
    blue_dis = -1

    # -----------------------------------
    # 青ゴールの検出
    for blob in img.find_blobs([goal_blue], pixels_threshold=10, area_threshold=10, merge=True, margin=25):
        img.draw_rectangle(blob.rect())
        if count_goal_blue >= 2:
            count_goal_blue = 0
        blue[0] = blob.cx()
        blue[1] = blob.cy()
        blue_dis = math.sqrt((blue[0] - screen_center[0])**2 + (blue[1] - screen_center[1])**2)
        img.draw_line(screen_center[0], screen_center[1], blue[0], blue[1], color=(0, 32, 255), thickness=2)
        blue_dir = ((math.atan2(blue[0] - screen_center[0], blue[1] - screen_center[1]) * 180 / math.pi + 180) - 90 + 360) % 360 - 180
        dir_goal_blue[count_goal_blue] = blue_dir
        dis_goal_blue[count_goal_blue] = blue_dis
        area_goal_blue[count_goal_blue] = blob.area()
        blue_num = area_goal_blue.index(max(area_goal_blue[:]))
        blue_dir = dir_goal_blue[blue_num]
        blue_dis = dis_goal_blue[blue_num]
        count_goal_blue += 1

    # -----------------------------------
    # 黄ゴールの検出
    for blob in img.find_blobs([goal_yellow], pixels_threshold=10, area_threshold=10, merge=True, margin=25):
        img.draw_rectangle(blob.rect())
        if count_goal_yellow >= 2:
            count_goal_yellow = 0
        yellow[0] = blob.cx()
        yellow[1] = blob.cy()
        yellow_dis = math.sqrt((yellow[0] - screen_center[0])**2 + (yellow[1] - screen_center[1])**2)
        img.draw_line(screen_center[0], screen_center[1], yellow[0], yellow[1], color=(255, 255, 0), thickness=2)
        yellow_dir = ((math.atan2(yellow[0] - screen_center[0], yellow[1] - screen_center[1]) * 180 / math.pi + 180) - 90 + 360) % 360 - 180
        dir_goal_yellow[count_goal_yellow] = yellow_dir
        dis_goal_yellow[count_goal_yellow] = yellow_dis
        area_goal_yellow[count_goal_yellow] = blob.area()
        yellow_num = area_goal_yellow.index(max(area_goal_yellow[:]))
        yellow_dir = dir_goal_yellow[yellow_num]
        yellow_dis = dis_goal_yellow[yellow_num]
        count_goal_yellow += 1

    # -----------------------------------
    # コート検出（毎フレーム更新）
    found_court = False
    for blob in img.find_blobs([court_green], pixels_threshold=50, area_threshold=50, merge=True, margin=25):
        found_court = True
        if count_court_green >= 2:
            count_court_green = 0
        court[0] = blob.cx()
        court[1] = blob.cy()
        court_pos = (court[0], court[1])
        court_dir_temp = ((math.atan2(court[0] - screen_center[0], court[1] - screen_center[1]) * 180 / math.pi + 180) - 90 + 360) % 360 - 180
        dir_court_green[count_court_green] = court_dir_temp
        area_court_green[count_court_green] = blob.area()
        court_green_num = area_court_green.index(max(area_court_green[:]))
        court_dir_temp = dir_court_green[court_green_num]
        court_dir = court_dir_temp
        count_court_green += 1

    if court_pos is not None:
        img.draw_line(screen_center[0], screen_center[1], court_pos[0], court_pos[1], color=(0, 255, 0), thickness=2)

    # -----------------------------------
    # Teensy用角度変換
    court_dir = convert_angle_for_teensy(court_dir)
    yellow_dir = convert_angle_for_teensy(yellow_dir)
    blue_dir   = convert_angle_for_teensy(blue_dir)

    # 整数化
    court_dir = int(court_dir)
    yellow_dir = int(yellow_dir)
    yellow_dis = int(yellow_dis)
    blue_dir = int(blue_dir)
    blue_dis = int(blue_dis)

    # UART送信
    uart.write(bytearray([0xAA]))  # 同期ヘッダー
    send_int16(uart, court_dir)
    send_int16(uart, yellow_dir)
    send_int16(uart, yellow_dis)
    send_int16(uart, blue_dir)
    send_int16(uart, blue_dis)

    # デバッグ表示
    print("--- Current Frame Data ---")
    print(f"Yellow Dir: {yellow_dir}")
    print(f"Court Dir: {court_dir}")
    print(f"Blue Dir: {blue_dir}")
    print(f"Yellow Dis: {yellow_dis}")
    print(f"Blue Dis: {blue_dis}")
    print("\n")
