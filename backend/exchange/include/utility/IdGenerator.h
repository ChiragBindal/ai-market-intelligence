#pragma once

inline int getIdByGenerator() {
    static int nextId = 0;
    return ++nextId;
}
