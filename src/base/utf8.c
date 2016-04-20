#include "utf8.h"

#include <ctype.h>

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
