#include "../include/datapack.h"

// #define DEBUG
#ifdef DEBUG
#include <assert.h>
#include <float.h>
#include <limits.h>
#endif

int main(void) {
#ifndef DEBUG
  unsigned char buf[1024];
  unsigned char magic;
  int monkeycount;
  long altitude;
  double absurdityfactor;
  char *s = "Great unmitigated Zot!  You've found the Runestaff!";
  char s2[96];
  unsigned int packetsize, ps2;

  packetsize = pack(buf, "CHhlsd", 'B', 0, 37, -5, s, -3490.5);
  packi16(buf + 1, packetsize); // store packet size in packet for kicks

  printf("packet is %u bytes\n", packetsize);

  unpack(buf, "CHhl96sd", &magic, &ps2, &monkeycount, &altitude, s2,
         &absurdityfactor);

  printf("'%c' %hhu %u %ld \"%s\" %f\n", magic, ps2, monkeycount, altitude, s2,
         absurdityfactor);

#else
  unsigned char buf[1024];

  int x;

  long long k, k2;
  long long test64[14] = {0,
                          -0,
                          1,
                          2,
                          -1,
                          -2,
                          0x7fffffffffffffffll >> 1,
                          0x7ffffffffffffffell,
                          0x7fffffffffffffffll,
                          -0x7fffffffffffffffll,
                          -0x8000000000000000ll,
                          9007199254740991ll,
                          9007199254740992ll,
                          9007199254740993ll};

  unsigned long long K, K2;
  unsigned long long testu64[14] = {0,
                                    0,
                                    1,
                                    2,
                                    0,
                                    0,
                                    0xffffffffffffffffll >> 1,
                                    0xfffffffffffffffell,
                                    0xffffffffffffffffll,
                                    0,
                                    0,
                                    9007199254740991ll,
                                    9007199254740992ll,
                                    9007199254740993ll};

  long i, i2;
  long test32[14] = {0,
                     -0,
                     1,
                     2,
                     -1,
                     -2,
                     0x7fffffffl >> 1,
                     0x7ffffffel,
                     0x7fffffffl,
                     -0x7fffffffl,
                     -0x80000000l,
                     0,
                     0,
                     0};

  unsigned long I, I2;
  unsigned long testu32[14] = {
      0,           0,           1, 2, 0, 0, 0xffffffffl >> 1,
      0xfffffffel, 0xffffffffl, 0, 0, 0, 0, 0};

  int j, j2;
  int test16[14] = {0,      -0,     1,       2,       -1, -2, 0x7fff >> 1,
                    0x7ffe, 0x7fff, -0x7fff, -0x8000, 0,  0,  0};

  printf("char bytes: %zu\n", sizeof(char));
  printf("int bytes: %zu\n", sizeof(int));
  printf("long bytes: %zu\n", sizeof(long));
  printf("long long bytes: %zu\n", sizeof(long long));
  printf("float bytes: %zu\n", sizeof(float));
  printf("double bytes: %zu\n", sizeof(double));
  printf("long double bytes: %zu\n", sizeof(long double));

  for (x = 0; x < 14; x++) {
    k = test64[x];
    pack(buf, "q", k);
    unpack(buf, "q", &k2);

    if (k2 != k) {
      printf("64: %lld != %lld\n", k, k2);
      printf("  before: %016llx\n", k);
      printf("  after:  %016llx\n", k2);
      printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
             " %02hhx %02hhx %02hhx %02hhx\n",
             buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    } else {
      // printf("64: OK: %lld == %lld\n", k, k2);
    }

    K = testu64[x];
    pack(buf, "Q", K);
    unpack(buf, "Q", &K2);

    if (K2 != K) {
      printf("64: %llu != %llu\n", K, K2);
    } else {
      // printf("64: OK: %llu == %llu\n", K, K2);
    }

    i = test32[x];
    pack(buf, "l", i);
    unpack(buf, "l", &i2);

    if (i2 != i) {
      printf("32(%d): %ld != %ld\n", x, i, i2);
      printf("  before: %08lx\n", i);
      printf("  after:  %08lx\n", i2);
      printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
             " %02hhx %02hhx %02hhx %02hhx\n",
             buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    } else {
      // printf("32: OK: %ld == %ld\n", i, i2);
    }

    I = testu32[x];
    pack(buf, "L", I);
    unpack(buf, "L", &I2);

    if (I2 != I) {
      printf("32(%d): %lu != %lu\n", x, I, I2);
    } else {
      // printf("32: OK: %lu == %lu\n", I, I2);
    }

    j = test16[x];
    pack(buf, "h", j);
    unpack(buf, "h", &j2);

    if (j2 != j) {
      printf("16: %d != %d\n", j, j2);
    } else {
      // printf("16: OK: %d == %d\n", j, j2);
    }
  }

  if (1) {
    long double testf64[8] = {-3490.6677,  0.0,         1.0,     -1.0,
                              DBL_MIN * 2, DBL_MAX / 2, DBL_MIN, DBL_MAX};
    long double f, f2;

    for (i = 0; i < 8; i++) {
      f = testf64[i];
      pack(buf, "g", f);
      unpack(buf, "g", &f2);

      if (f2 != f) {
        printf("f64: %Lf != %Lf\n", f, f2);
        printf("  before: %016llx\n", *((long long *)&f));
        printf("  after:  %016llx\n", *((long long *)&f2));
        printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
               " %02hhx %02hhx %02hhx %02hhx\n",
               buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
      } else {
        // printf("f64: OK: %f == %f\n", f, f2);
      }
    }
  }
  if (1) {
    double testf32[7] = {0.0, 1.0, -1.0, 10, -3.6677, 3.1875, -3.1875};
    double f, f2;

    for (i = 0; i < 7; i++) {
      f = testf32[i];
      pack(buf, "d", f);
      unpack(buf, "d", &f2);

      if (f2 != f) {
        printf("f32: %.10f != %.10f\n", f, f2);
        printf("  before: %016llx\n", *((long long *)&f));
        printf("  after:  %016llx\n", *((long long *)&f2));
        printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
               " %02hhx %02hhx %02hhx %02hhx\n",
               buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
      } else {
        // printf("f32: OK: %f == %f\n", f, f2);
      }
    }
  }
  if (1) {
    float testf16[7] = {0.0, 1.0, -1.0, 10, -10, 3.1875, -3.1875};
    float f, f2;

    for (i = 0; i < 7; i++) {
      f = testf16[i];
      pack(buf, "f", f);
      unpack(buf, "f", &f2);

      if (f2 != f) {
        printf("f16: %f != %f\n", f, f2);
        printf("  before: %08x\n", *((int *)&f));
        printf("  after:  %08x\n", *((int *)&f2));
        printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
               " %02hhx %02hhx %02hhx %02hhx\n",
               buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
      } else {
        // printf("f16: OK: %f == %f\n", f, f2);
      }
    }
  }
#endif

  return 0;
}
