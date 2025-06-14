#pragma once

struct S {
    double d[4];
};

struct S returnsLargeStruct() {
    struct S s;
    s.d[0] = 0;
    s.d[1] = 1;
    s.d[2] = 2;
    s.d[3] = 3;
    return s;
}
