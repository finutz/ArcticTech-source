#pragma once

static constexpr unsigned int FNV_32_PRIME = 0x01000193;

constexpr unsigned int FNV1A(const char* buf) {
    unsigned int hval = 0x811c9dc5;

    while (*buf)
    {
        hval ^= (unsigned int)*buf++;
        hval *= FNV_32_PRIME;
    }

    return hval;
}