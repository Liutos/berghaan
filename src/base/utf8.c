#include "utf8.h"

#include <ctype.h>

static unsigned int MASKS[] = {
    0x1F,
    0x0F,
    0x07,
    0x03,
    0x01,
};

static unsigned int
bitwise_count1(uint8_t byte)
{
    unsigned int count = 0;
    while ((byte & 0x80) == 0x80) {
        count++;
        byte = byte << 1;
    }
    return count;
}

uint32_t
utf8_fread(FILE *in, unsigned int *nbytes)
{
    uint8_t byte1 = fgetc(in);
    if (isascii(byte1))
        return byte1;
    unsigned int length = bitwise_count1(byte1);
    unsigned int mask = MASKS[length - 2];
    uint32_t code = byte1 & mask;
    for (unsigned int i = 0; i < length - 1; i++) {
        uint8_t byte = fgetc(in);
        code = (code << 6) | (byte & 0x3F);
    }
    if (nbytes != NULL) {
        *nbytes = length;
    }
    return code;
}

uint32_t
utf8_sread(const char *in, unsigned int *nbytes)
{
    uint8_t byte1 = *in++;
    if (isascii(byte1))
        return byte1;
    unsigned int length = bitwise_count1(byte1);
    unsigned int mask = MASKS[length - 2];
    uint32_t code = byte1 & mask;
    for (unsigned int i = 0; i < length - 1; i++) {
        uint8_t byte = *in++;
        code = (code << 6) | (byte & 0x3F);
    }
    if (nbytes != NULL) {
        *nbytes = length;
    }
    return code;
}

void
utf8_code_to_bytes(uint32_t c, uint8_t *bytes, size_t *length)
{
    if (isascii(c)) {
        bytes[0] = c;
        *length = 1;
        return;
    }
    int n = 0;
    uint8_t bs[8] = { 0 };
    // 能够从c中提取出低6位
    while (c > (2 << 6)) {
        uint8_t l = c & 0x3F;
        bs[n] = l | 0x80;
        n++;
        c = c >> 6;
    }
    // 根据n对c剩下的位补上1
    uint8_t h = 0;
    for (int i = 0; i <= n; i++)
        h = (h >> 1) | 0x80;
    bs[n] = c | h;
    // 倒序输出bs中的字节
    for (int i = n; i >= 0; i--)
        bytes[n - i] = bs[i];
    *length = n + 1;
}

void
utf8_fprintf(FILE *out, uint32_t c)
{
    if (isascii(c)) {
        fprintf(out, "%c", c);
        return;
    }
    int n = 0;
    uint8_t bs[8] = {0};
    // 能够从c中提取出低6位
    while (c > (2 << 6)) {
        uint8_t l = c & 0x3F;
        bs[n] = l | 0x80;
        n++;
        c = c >> 6;
    }
    // 根据n对c剩下的位补上1
    uint8_t h = 0;
    for (int i = 0; i <= n; i++)
        h = (h >> 1) | 0x80;
    bs[n] = c | h;
    // 倒序输出bs中的字节
    for (int i = n; i >= 0; i--)
        fwrite(&bs[i], sizeof(uint8_t), 1, out);
}
