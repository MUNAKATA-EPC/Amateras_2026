import sensor
import time
import image
from pyb import UART
import math

#############################################################
# ゴールの色取り用変数(黄色)
goal_yellow = (28, 100, 127, 16, -87, 123)
#############################################################
# ゴールの色取り用変数(青色)
goal_blue = (28, 100, 127, 16, -87, 123)
#############################################################
# コートの色（カーペット用）
court_green = (42, 57, -26, -2, -40, -9)
#############################################################
#コートの明るさ
court_light = (30, 88, -71, -27, -40, 18)
#############################################################
# 黒色の閾値
black = (23, 0, -1, -4, 2, 20)
wall_balck = [0,0]
#############################################################
# 画面の中央座標
screen_center = [140, 117]


court = [0, 0]
yellow = [0, 0]
blue = [0,0]

court_dir = 0
#########################################################
# 青ゴール検出用変数
count_goal_blue = 0
blue_num = 0
dir_goal_blue = [0, 0]    # 青ゴールの方向保存用配列
dis_goal_blue = [0, 0]    # 青ゴールの距離保存用配列
area_goal_blue = [0, 0]   # 青ゴールの色取りエリア保存用配列

#########################################################
# 黄ゴール検出用変数
count_goal_yellow = 0
yellow_num = 0
dir_goal_yellow = [0, 0]    # 黄ゴールの方向保存用配列
dis_goal_yellow = [0, 0]    # 黄ゴールの距離保存用配列
area_goal_yellow = [0, 0]   # 黄ゴールの色取りエリア保存用配列

###############################################################
# コート（緑）検出用変数
count_court_green = 0
court_green_num = 0
dir_court_green = [0, 0]    # コートの方向保存用配列
area_court_green = [0, 0]   # コートの色取りエリア保存用配列

###############################################################

# センサー設定
sensor.set_saturation(0) # 彩度
sensor.set_brightness(0) # 明度
sensor.set_contrast(0)   # 明暗
sensor.set_auto_gain(False) # 自動ゲインオフ
sensor.set_auto_exposure(False) # 自動露出オフ

sensor.reset()  # センサーをリセットして初期化
sensor.set_pixformat(sensor.RGB565)  # ピクセルフォーマットをRGB565に設定
sensor.set_framesize(sensor.QVGA)  # フレームサイズをQVGA (320x240)に設定
sensor.skip_frames(time=200)  # 設定が有効になるまで待機
clock = time.clock()  # FPSを追跡するためのクロックオブジェクトを作成
uart = UART(3, 115200, timeout_char=1000) # UART3を115200bpsで初期化

while True:
    clock.tick()  # FPSクロックを更新
    img = sensor.snapshot() # 画像をキャプチャ
    img.draw_cross(screen_center[0], screen_center[1]) # 画面中央に十字を描画

    # 画面外周を黒く塗りつぶす（ノイズ軽減や視野制限のため）
    img.draw_circle(screen_center[0], screen_center[1],190,[0,0,0],130)
    img.draw_circle(screen_center[0], screen_center[1],30,[0, 0, 0], 1, 1)

    # 各方向と距離の初期化
    yellow_dir = -1
    court_dir = -1
    yellow_dis = -1
    blue_dir = -1
    blue_dis = -1

    # 青ゴールの検出
    for blob in img.find_blobs([goal_blue], pixels_threshold=10, area_threshold=10, merge=True,margin=25):
        img.draw_rectangle(blob.rect()) # 検出されたブロブに四角を描画
        if count_goal_blue >= 2:
           count_goal_blue = 0

        blue[0] = blob.cx() # ブロブの中心X座標
        blue[1] = blob.cy() # ブロブの中心Y座標
        # 画面中心からの距離を計算
        blue_dis = math.sqrt(math.pow(blue[0] - screen_center[0] ,2) + math.pow(blue[1] -screen_center[1],2))

        # 画面中心から青ゴールへの線を描画
        img.draw_line(screen_center[0], screen_center[1], blue[0], blue[1],color=(0,32,255), thickness=2)

        # 青ゴールの角度を計算 (-90から270の範囲で、0度が真上)
        blue_dir = ((math.atan2(blue[0] - screen_center[0] , blue[1] - screen_center[1] ) * 180 / math.pi + 180) - 90 + 360) % 360 - 90

        dir_goal_blue[count_goal_blue] = blue_dir
        dis_goal_blue[count_goal_blue] = blue_dis
        area_goal_blue[count_goal_blue] = blob.area()

        # 複数のブロブがある場合、最大の面積を持つブロブを選択
        blue_num = area_goal_blue.index(max(area_goal_blue[:]))

        blue_dir = dir_goal_blue[blue_num]
        blue_dis = dis_goal_blue[blue_num]

        # 角度を-180から180の範囲に調整
        if blue_dir > 180:
            blue_dir = blue_dir - 360

        count_goal_blue += 1

    # 黄ゴールの検出
    for blob in img.find_blobs([goal_yellow], pixels_threshold=10, area_threshold=10, merge=True,margin=25):
        img.draw_rectangle(blob.rect()) # 検出されたブロブに四角を描画
        if count_goal_yellow >= 2:
            count_goal_yellow = 0

        yellow[0] = blob.cx() # ブロブの中心X座標
        yellow[1] = blob.cy() # ブロブの中心Y座標
        # 画面中心からの距離を計算
        yellow_dis = math.sqrt(math.pow(yellow[0] - screen_center[0] ,2) + math.pow(yellow[1] -screen_center[1],2))

        # 画面中心から黄ゴールへの線を描画
        img.draw_line(screen_center[0], screen_center[1], yellow[0], yellow[1],color=(255,255,0), thickness=2)

        # 黄ゴールの角度を計算 (-90から270の範囲で、0度が真上)
        yellow_dir = ((math.atan2(yellow[0] - screen_center[0] , yellow[1] - screen_center[1] ) * 180 / math.pi + 180) - 90 + 360) % 360 - 90

        dir_goal_yellow[count_goal_yellow] = yellow_dir
        dis_goal_yellow[count_goal_yellow] = yellow_dis
        area_goal_yellow[count_goal_yellow] = blob.area()

        # 複数のブロブがある場合、最大の面積を持つブロブを選択
        yellow_num = area_goal_yellow.index(max(area_goal_yellow[:]))

        yellow_dir = dir_goal_yellow[yellow_num]
        yellow_dis = dis_goal_yellow[yellow_num]

        # 角度を-180から180の範囲に調整
        if yellow_dir > 180:
            yellow_dir = yellow_dir - 360

        count_goal_yellow += 1

    # コート（緑）の検出
    for blob in img.find_blobs([court_green], pixels_threshold=50, area_threshold=50, merge=True,margin=25):
        img.draw_rectangle(blob.rect()) # 検出されたブロブに四角を描画

        if count_court_green >= 2:
           count_court_green = 0

        court[0] = blob.cx() # ブロブの中心X座標
        court[1] = blob.cy() # ブロブの中心Y座標

        # 画面中心からコート中心へのライン描画
        img.draw_line(screen_center[0], screen_center[1], court[0], court[1],color=(0,190,0), thickness=2)

        # コートの角度を計算 (0から360の範囲で、0度が真上)
        court_dir = ((math.atan2(court[0] - screen_center[0] , court[1] - screen_center[1] ) * 180 / math.pi + 180) - 90 + 360) % 360

        dir_court_green[count_court_green] = court_dir
        area_court_green[count_court_green] = blob.area()

        # 複数のブロブがある場合、最大の面積を持つブロブを選択
        court_green_num = area_court_green.index(max(area_court_green[:]))

        court_dir = dir_court_green[court_green_num]

        # 角度を-90から270の範囲に調整 (元のコードのロジックに合わせる)
        court_dir = court_dir - 90

        if court_dir < 0:
            court_dir = court_dir + 360

        count_court_green += 1

    #########################################
    # 角度を0~360度の範囲に正規化（UART送信前）
    if court_dir != -1:
        court_dir = (court_dir + 360) % 360
        court_dir = 360.0 - court_dir
    else:
        court_dir = -1

    if yellow_dir != -1:
        yellow_dir = (yellow_dir + 360) % 360
        yellow_dir = 360 - yellow_dir
    else:
        yellow_dir = -1

    if blue_dir != -1:
        blue_dir = (blue_dir + 360) % 360
        blue_dir = 360 - blue_dir
    else:
        blue_dir = -1

    # すべて整数に変換
    court_dir = int(court_dir)
    yellow_dir = int(yellow_dir)
    yellow_dis = int(yellow_dis)
    blue_dir = int(blue_dir)
    blue_dis = int(blue_dis)



    # 数値を文字列にして、区切り文字を間に挟む

    uart.write(str(court_dir))
    uart.write("a")
    uart.write(str(yellow_dir))
    uart.write("b")
    uart.write(str(yellow_dis))
    uart.write("c")
    uart.write(str(blue_dir))
    uart.write("d")
    uart.write(str(blue_dis))
    uart.write("e")


    # UARTに文字列として送信
    #uart.write(send_str + "\n")

    uart.flush()

    # PCにも送る（OpenMV IDEのシリアルターミナルなどで見る用）
    # ここprintにprint文を配置することで、毎フレーム値が出力されます。
    #print("--- Current Frame Data ---")
    #print(f"Yellow Dir: {yellow_dir}")
    #print(f"Court Dir: {court_dir}")
    #print(f"Blue Dir: {blue_dir}")
    #print(f"Yellow Dis: {yellow_dis}")
    #print(f"Blue Dis: {blue_dis}")
    #print("\n") # 区切り線
