import sensor, image, time, math, lcd
from machine import UART
from fpioa_manager import fm

lcd.init()

# ==================== UART設定 ====================
fm.register(35, fm.fpioa.UART1_TX, force=True)
fm.register(34, fm.fpioa.UART1_RX, force=True)
uart = UART(UART.UART1, 115200, 8, None, 1, timeout=1000)
#(0, 100, -128, 127, -128, 127) <- スライダーリセットの時の値
goal_yellow = (44, 61, -7, 17, 28, 109)
goal_blue   = (44, 61, -7, 17, 28, 109)
screen_center = [160, 120]  # 中央座標

sensor.reset(dual_buff=True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=400)
sensor.set_contrast(-3)
sensor.set_brightness(-3)
sensor.set_saturation(0)
sensor.skip_frames(time=250)
sensor.set_auto_gain(False, gain_db=10, gain_db_ceiling=16)
sensor.set_auto_exposure(False, exposure_us=3000)
clock = time.clock()

# ==================== 角度計算 ====================
def calc_angle(cx):
    dx = cx - screen_center[0]
    raw_angle = int((abs(dx) / screen_center[0]) * 90)

    if dx < 0:
        return raw_angle        # 左側 → 0～179°
    else:
        return (360 - raw_angle) % 360  # 右側 → 359～181°

# ==================== メインループ ====================
while True:

    clock.tick()
    img = sensor.snapshot()
    img.rotation_corr(z_rotation=180)

    #img.draw_line(0, screen_center[1], screen_center[0]*2, screen_center[1], thickness=1)
    #img.draw_line(screen_center[0], 0, screen_center[0], screen_center[1]*2, thickness=1)
    #img.gamma(gamma=1.2, contrast=1.3, brightness=-0.2)
    yellow_dir = -1
    yellow_dis = -1
    blue_dir = -1
    blue_dis = -1
    court_dir = -1

    # ==================== 黄ゴール（最大面積を選択） ====================
    blobs = img.find_blobs([goal_yellow], pixels_threshold=20, area_threshold=20, merge=True)
    if blobs:
        biggest_blob = max(blobs, key=lambda b: b.pixels())
        img.draw_rectangle(biggest_blob.rect(), color=(255,255,0))
        cx, cy = biggest_blob.cx(), biggest_blob.cy()

        yellow_dis = int(math.sqrt((cx - screen_center[0])**2 + (cy - screen_center[1])**2))
        yellow_dir = calc_angle(cx)

    # ==================== 青ゴール（最大面積を選択） ====================
    blobs = img.find_blobs([goal_blue], pixels_threshold=20, area_threshold=20, merge=True)
    if blobs:
        biggest_blob = max(blobs, key=lambda b: b.pixels())
        img.draw_rectangle(biggest_blob.rect(), color=(0,0,255))
        cx, cy = biggest_blob.cx(), biggest_blob.cy()

        blue_dis = int(math.sqrt((cx - screen_center[0])**2 + (cy - screen_center[1])**2))
        blue_dir = calc_angle(cx)

    # ==================== Teensyへ送信 ====================
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

    #2バイトのデータのなので1バイトずつ送る
    # 送信したい値をintにしてバイナリ送信
    def send_int16(uart, value):
        if value == -1:
            value = 0xFFFF
        low = value & 0xFF
        high = (value >> 8) & 0xFF
        uart.write(bytearray([low, high]))  # low -> high

    # 同期ヘッダー送信
    uart.write(bytearray([0xAA]))

    # バイナリで送信
    send_int16(uart, court_dir)
    send_int16(uart, yellow_dir)
    send_int16(uart, yellow_dis)
    send_int16(uart, blue_dir)
    send_int16(uart, blue_dis)

    print("court:%d yellow:%d dist:%d blue:%d dist:%d" %
          (court_dir, yellow_dir, yellow_dis, blue_dir, blue_dis))
