#include "posi.hpp"

bool posiAccord(posi_t a, posi_t b)
{
    return (a.x == b.x) && (a.y == b.y);
}