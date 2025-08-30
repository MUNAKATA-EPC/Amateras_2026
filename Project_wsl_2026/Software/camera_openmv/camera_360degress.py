import sensor
import time
import image
from pyb import UART
import math

#############################################################
# ゴールの色取り用変数(黄色)
goal_yellow = (43, 97, -39, -12, 28, 127)
#############################################################
# ゴールの色取り用変数(青色)
goal_blue = (43, 97, -39, -12, 28, 127)
#############################################################
# コートの色（カーペット用）
court_green = (23, 48, -13, -2, -15, 7)
#############################################################
# 画面の中央座標
screen_center = [170, 102]

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

# センサー設定
sensor.set_saturation(0)           # 彩度
sensor.set_brightness(0)           # 明度
sensor.set_contrast(0)             # 明暗
sensor.set_auto_gain(False)        # 自動ゲインオフ
sensor.set_auto_exposure(False)    # 自動露出オフ

sensor.reset()                     # センサーをリセットして初期化
sensor.set_pixformat(sensor.RGB565)# ピクセルフォーマットをRGB565に設定
sensor.set_framesize(sensor.QVGA)  # フレームサイズをQVGA (320x240)に設定
sensor.skip_frames(time=200)       # 設定が有効になるまで待機
clock = time.clock()               # FPSを追跡するためのクロックオブジェクトを作成
uart = UART(3, 115200, timeout_char=1000) # UART3を115200bpsで初期化

# -----------------------------------
# 2バイトのデータ送信関数
def send_int16(uart, value):
    if value == -1:       # -1を特別値として0xFFFFに変換
        value = 0xFFFF
    low = value & 0xFF
    high = (value >> 8) & 0xFF
    uart.write(bytearray([low, high]))  # low -> high

# -----------------------------------
while True:
    clock.tick()           # FPSクロックを更新
    frame_count += 1       # フレームカウント
    img = sensor.snapshot()# 画像をキャプチャ
    img.draw_cross(screen_center[0], screen_center[1]) # 画面中央に十字

    # 画面外周を黒く塗りつぶす（ノイズ軽減や視野制限のため）
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
        img.draw_rectangle(blob.rect()) # 検出されたブロブに四角を描画

        if count_goal_blue >= 2:
            count_goal_blue = 0

        blue[0] = blob.cx() # ブロブの中心X座標
        blue[1] = blob.cy() # ブロブの中心Y座標
        blue_dis = math.sqrt((blue[0] - screen_center[0])**2 + (blue[1] - screen_center[1])**2)

        img.draw_line(screen_center[0], screen_center[1], blue[0], blue[1], color=(0, 32, 255), thickness=2)

        blue_dir = ((math.atan2(blue[0] - screen_center[0], blue[1] - screen_center[1]) * 180 / math.pi + 180) - 90 + 360) % 360 - 90

        dir_goal_blue[count_goal_blue] = blue_dir
        dis_goal_blue[count_goal_blue] = blue_dis
        area_goal_blue[count_goal_blue] = blob.area()

        blue_num = area_goal_blue.index(max(area_goal_blue[:]))
        blue_dir = dir_goal_blue[blue_num]
        blue_dis = dis_goal_blue[blue_num]

        if blue_dir > 180:
            blue_dir -= 360

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

        yellow_dir = ((math.atan2(yellow[0] - screen_center[0], yellow[1] - screen_center[1]) * 180 / math.pi + 180) - 90 + 360) % 360 - 90

        dir_goal_yellow[count_goal_yellow] = yellow_dir
        dis_goal_yellow[count_goal_yellow] = yellow_dis
        area_goal_yellow[count_goal_yellow] = blob.area()

        yellow_num = area_goal_yellow.index(max(area_goal_yellow[:]))
        yellow_dir = dir_goal_yellow[yellow_num]
        yellow_dis = dis_goal_yellow[yellow_num]

        if yellow_dir > 180:
            yellow_dir -= 360

        count_goal_yellow += 1

    # -----------------------------------
    # コート検出（軽量化：5フレームに1回のみ更新）
    if frame_count % 5 == 0:
        for blob in img.find_blobs([court_green], pixels_threshold=50, area_threshold=50, merge=True, margin=25):
            img.draw_rectangle(blob.rect())  # 描画最小限

            if count_court_green >= 2:
                count_court_green = 0

            court[0] = blob.cx()
            court[1] = blob.cy()
            court_dir_temp = ((math.atan2(court[0] - screen_center[0], court[1] - screen_center[1]) * 180 / math.pi + 180) - 90 + 360) % 360
            dir_court_green[count_court_green] = court_dir_temp
            area_court_green[count_court_green] = blob.area()

            court_green_num = area_court_green.index(max(area_court_green[:]))
            court_dir_temp = dir_court_green[court_green_num]
            court_dir_temp = (court_dir_temp - 90 + 360) % 360  # -90〜270範囲

            count_court_green += 1

        # field_dir更新
        if court_dir_temp != -1:
            court_dir = court_dir_temp

    # -----------------------------------
    # UART送信前に整数化
    court_dir = int(court_dir)
    yellow_dir = int(yellow_dir)
    yellow_dis = int(yellow_dis)
    blue_dir = int(blue_dir)
    blue_dis = int(blue_dis)

    # 同期ヘッダー送信
    uart.write(bytearray([0xAA]))

    # バイナリ送信（2バイトずつ）
    send_int16(uart, court_dir)
    send_int16(uart, yellow_dir)
    send_int16(uart, yellow_dis)
    send_int16(uart, blue_dir)
    send_int16(uart, blue_dis)

    # -----------------------------------
    # OpenMV IDE用のデバッグ出力
    print("--- Current Frame Data ---")
    print(f"Yellow Dir: {yellow_dir}")
    print(f"Court Dir: {court_dir}")
    print(f"Blue Dir: {blue_dir}")
    print(f"Yellow Dis: {yellow_dis}")
    print(f"Blue Dis: {blue_dis}")
    print("\n")
