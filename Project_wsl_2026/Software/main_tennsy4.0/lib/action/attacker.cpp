#include "attacker.hpp"
#include "test.hpp"

/*攻撃用のプログラムを実行する*/

void play_attacker()
{
    play_test(TEST_ALL_CHECK_WITH_SERIAL); // シリアルプリントさせる

    PD_use_gyro(); // PD制御にジャイロを使う

    if (is_LINE_exist) // ラインがあるならば
    {
        motors_move(get_LINE_deg() + 180, 95); // ラインから逃れる動きをする
    }
    else
    {
        if (is_IR_exist) // IRボールがあるならば
        {
            if (get_IR_deg() < 10 || get_IR_deg() > 350) // 前にIRボールがあるとき
            {
                motors_move(0, 95); // 前進する
            }
            else if (get_IR_deg() < 25 || get_IR_deg() > 335) // 前付近にIRボールがあるとき
            {
                motors_move(get_IR_follow_deg(100), 95); // 前方100方向にボールが行くように制御する
            }
            else
            {
                if (get_IR_distance() < 170) // 近くにIRボールがあるとき
                {
                    motors_move(get_IR_mawarikomi_deg(), 95); // 回り込むように制御
                }
                else
                {
                    motors_move(get_IR_follow_deg(0), 95); // ボールに近寄るように制御
                }
            }
        }
        else
        {
            motors_only_PD(95); // PD制御のみさせる
        }
    }
}