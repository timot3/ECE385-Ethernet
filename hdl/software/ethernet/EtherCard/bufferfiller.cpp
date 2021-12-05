#include "bufferfiller.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
// ltoa stolen from the internet (https://gist.github.com/boatilus/11305304)

#define BUFSIZE (sizeof(long) * 8 + 1)

char *ltoa(long N, char *str, int base) {
  int i = 2;
  long uarg;
  char *tail, *head = str, buf[BUFSIZE];

  if (36 < base || 2 > base)
    base = 10;              /* can only use 0-9, A-Z        */
  tail = &buf[BUFSIZE - 1]; /* last character position      */
  *tail-- = '\0';

  if (10 == base && N < 0L) {
    *head++ = '-';
    uarg = -N;
  } else
    uarg = N;

  if (uarg) {
    for (i = 1; uarg; ++i) {
      ldiv_t r;
      r = ldiv(uarg, base);
      *tail-- = (char)(r.rem + ((9L < r.rem) ? ('A' - 10L) : '0'));
      uarg = r.quot;
    }
  } else
    *tail-- = '0';

  memcpy(head, ++tail, i);
  return str;
}

void BufferFiller::emit_p(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  for (;;) {
    char c = *fmt++;
    if (c == 0)
      break;
    if (c != '$') {
      *ptr++ = c;
      continue;
    }
    c = *fmt++;
    switch (c) {
    case 'D':

      ether.wtoa(va_arg(ap, int), (char *)ptr);

      break;
#ifdef FLOATEMIT
    case 'T':
      dtostrf(va_arg(ap, double), 10, 3, (char *)ptr);
      break;
#endif
    case 'H': {

      char p1 = va_arg(ap, int);

      char p2;
      p2 = (p1 >> 4) & 0x0F;
      p1 = p1 & 0x0F;
      if (p1 > 9)
        p1 += 0x07; // adjust 0x0a-0x0f to come out 'a'-'f'
      p1 += 0x30;   // and complete
      if (p2 > 9)
        p2 += 0x07; // adjust 0x0a-0x0f to come out 'a'-'f'
      p2 += 0x30;   // and complete
      *ptr++ = p2;
      *ptr++ = p1;
      continue;
    }
    case 'L':
      ltoa(va_arg(ap, long), (char *)ptr, 10);
      break;
    case 'S':
      strcpy((char *)ptr, va_arg(ap, const char *));
      break;
    case 'F': {
      const char *s = va_arg(ap, const char *);
      char d;
      while ((d = *s++) != 0)
        *ptr++ = d;
      continue;
    }
    case 'E': {
      byte *s = va_arg(ap, byte *);
      char d;
      while ((d = *s++) != 0)
        *ptr++ = d;
      continue;
    }
    default:
      *ptr++ = c;
      continue;
    }
    ptr += strlen((char *)ptr);
  }
  va_end(ap);
}
