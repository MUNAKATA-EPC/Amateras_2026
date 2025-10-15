#include "Motors.hpp"

Motors::Motors(HardwareSerial *serial, uint32_t baudrate)
{
    if (_dsr != nullptr)
    {
        delete _dsr;
    }
    _dsr = new DSR1202(serial, baudrate);
}

bool Motors::begin(DigitalSensor *toggle)
{
    _dsr->begin(toggle);

    return true;

}

void Motors::setDegPosition(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch)
{
    _deg_position[0] = deg_1ch;
    _deg_position[1] = deg_2ch;
    _deg_position[2] = deg_3ch;
    _deg_position[3] = deg_4ch;
}

void Motors::setMoveSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch)
{
    _move_sign[0] = sign_1ch / abs(sign_1ch); //-1or1
    _move_sign[1] = sign_2ch / abs(sign_2ch); //-1or1
    _move_sign[2] = sign_3ch / abs(sign_3ch); //-1or1
    _move_sign[3] = sign_4ch / abs(sign_4ch); //-1or1
}

void Motors::setPdSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch)
{
    _pd_sign[0] = sign_1ch / abs(sign_1ch); //-1or1
    _pd_sign[1] = sign_2ch / abs(sign_2ch); //-1or1
    _pd_sign[2] = sign_3ch / abs(sign_3ch); //-1or1
    _pd_sign[3] = sign_4ch / abs(sign_4ch); //-1or1
}

void Motors::stop()
{
    _dsr->stop();
}

void Motors::PDprocess(PD *pd, int deg, int target)
{
    _pd = pd;
    _pd->process(deg, target);
}

void Motors::move(int deg, int power)
{
    deg = (deg + 180) % 360; // 前を0度にする
    // いったん計算
    double max_move_output = 0.0;
    for (int i = 0; i < 4; i++)
    {
        Vector vec(deg + _deg_position[i], power); // "i"chから見た移動方向のベクトル
        _move_output[i] = vec.x() * _move_sign[i]; // "i"chの出力(X成分だけが移動に使える)

        max_move_output = (fabs(_move_output[i]) > max_move_output) ? fabs(_move_output[i]) : max_move_output; // 前よりも大きいなら更新
    }
    // 最大にする
    double scale = (max_move_output != 0) ? power / max_move_output : 0; // 出力がpowerになるようにするためのスケール
    for (int i = 0; i < 4; i++)
    {
        _move_output[i] *= scale;
    }
    // PD成分を付け加えて最終出力を決定
    double pd_output = -_pd->output();
    for (int i = 0; i < 4; i++)
    {
        _output[i] = (int)round(constrain(_move_output[i] + pd_output * _pd_sign[i], -power, power));
    }
    
    /*
    // いったん計算
    double pd_output = -map(_pd->output(), -100, 100, -power, power);
    double max_output = 0.0;
    for (int i = 0; i < 4; i++)
    {
        Vector vec(deg + _deg_position[i], power);        // "i"chから見た移動方向のベクトル
        _output[i] = vec.x() * _move_sign[i] + pd_output; // "i"chの出力(X成分だけが移動に使える)

        max_output = (fabs(_output[i]) > max_output) ? fabs(_output[i]) : max_output; // 前よりも大きいなら更新
    }
    // 最大にする
    double scale = (max_output != 0) ? power / max_output : 0; // 出力がpowerになるようにするためのスケール
    for (int i = 0; i < 4; i++)
    {
        _move_output[i] *= scale;
    }
    */

    // 制御
    _dsr->move(_output[0], _output[1], _output[2], _output[3]); // 制御
}

void Motors::PDmove()
{
    // 制御
    double pd_output = -_pd->output();
    _dsr->move((int)pd_output, (int)pd_output, (int)pd_output, (int)pd_output); // 制御
}