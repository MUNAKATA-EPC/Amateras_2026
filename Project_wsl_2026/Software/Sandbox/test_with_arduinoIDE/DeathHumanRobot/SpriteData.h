#include <binary.h>
#include <Sprite.h>

//-----------------------------------------------------------------------
// スプライトデータ
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

#include <Sprite.h>

Sprite big_heart = Sprite(5,4,
B00100,
B01110,
B11111,
B11011
);

Sprite small_heart = Sprite(5,4,
B00100,
B01110,
B01010,
B00000
);

Sprite broken_heart1 = Sprite(6,4,
B001000,
B010110,
B111011,
B110011
);

Sprite broken_heart2 = Sprite(6,4,
B000100,
B011010,
B111001,
B100011
);

Sprite broken_heart3 = Sprite(6,4,
B010000,
B001010,
B001001,
B100011
);

Sprite broken_heart4 = Sprite(6,4,
B011000,
B001000,
B001001,
B100011
);

Sprite noise = Sprite(1,1,
B1
);

#endif





