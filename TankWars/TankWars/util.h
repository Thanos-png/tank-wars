#pragma once

#define SETCOLOR(c, r, g, b) {c[0]=r; c[1]=g; c[2]=b;}

template <typename T>
T clamp(const T& value, const T& min, const T& max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
