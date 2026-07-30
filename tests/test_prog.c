typedef unsigned char u8;
typedef unsigned short u16;

#define MEM(a) (*(volatile u8 *)(a))

void main(void) {
  u8 i;
  u8 arr[8];
  u8 *ptr;
  u16 a16, b16;

  // 8-bit ADC/SBC  $0400-$0406
  {
    u8 x, y;
    x = 0x30;
    y = 0x00;
    MEM(0x0400) = x + y;
    x = 0x10;
    y = 0x30;
    MEM(0x0401) = x + y;
    x = 0x20;
    y = 0xD0;
    MEM(0x0402) = x + y;
    x = 0xFF;
    y = 0xF2;
    MEM(0x0403) = x + y;
    x = 0x80;
    y = 0x80;
    MEM(0x0404) = x + y;
    x = 0x20;
    y = 0x00;
    MEM(0x0405) = x - y;
    x = 0x10;
    y = 0x30;
    MEM(0x0406) = x - y;
  }

  // 16-bit add  $0407-$040A
  {
    u16 r;
    a16 = 0x1234;
    b16 = 0x0001;
    r = a16 + b16;
    MEM(0x0407) = (u8)r;
    MEM(0x0408) = (u8)(r >> 8);
    a16 = 0x00FF;
    b16 = 0x0001;
    r = a16 + b16;
    MEM(0x0409) = (u8)r;
    MEM(0x040A) = (u8)(r >> 8);
  }

  // 16-bit sub  $040B-$040E
  {
    u16 r;
    a16 = 0x1234;
    b16 = 0x0001;
    r = a16 - b16;
    MEM(0x040B) = (u8)r;
    MEM(0x040C) = (u8)(r >> 8);
    a16 = 0x0000;
    b16 = 0x0001;
    r = a16 - b16;
    MEM(0x040D) = (u8)r;
    MEM(0x040E) = (u8)(r >> 8);
  }

  // multiply  $040F-$0412
  {
    u16 p;
    p = (u16)7 * (u16)6;
    MEM(0x040F) = (u8)p;
    p = (u16)0xFF * (u16)2;
    MEM(0x0410) = (u8)p;
    MEM(0x0411) = (u8)(p >> 8);
    p = (u16)0 * (u16)42;
    MEM(0x0412) = (u8)p;
  }

  // divide  $0413-$0416
  {
    u16 q;
    q = (u16)100 / (u16)7;
    MEM(0x0413) = (u8)q;
    q = (u16)0xFF / (u16)0x10;
    MEM(0x0414) = (u8)q;
    q = (u16)0 / (u16)5;
    MEM(0x0415) = (u8)q;
    q = (u16)50 / (u16)50;
    MEM(0x0416) = (u8)q;
  }

  // modulo  $0417-$041A
  {
    u16 rm;
    rm = (u16)100 % (u16)7;
    MEM(0x0417) = (u8)rm;
    rm = (u16)0xFF % (u16)0x10;
    MEM(0x0418) = (u8)rm;
    rm = (u16)0 % (u16)5;
    MEM(0x0419) = (u8)rm;
    rm = (u16)50 % (u16)50;
    MEM(0x041A) = (u8)rm;
  }

  // signed negate  $041B-$041E
  {
    char n;
    n = 42;
    MEM(0x041B) = (u8)(-n);
    n = -1;
    MEM(0x041C) = (u8)(-n);
    n = 0;
    MEM(0x041D) = (u8)(-n);
    n = -128;
    MEM(0x041E) = (u8)(-n);
  }

  // array indexing  $041F-$0426
  {
    arr[0] = 0xAA;
    arr[1] = 0xBB;
    arr[2] = 0xCC;
    arr[3] = 0xDD;
    arr[4] = 0x11;
    arr[5] = 0x22;
    arr[6] = 0x33;
    arr[7] = 0x44;
    for (i = 0; i < 8; i++)
      MEM(0x041F + i) = arr[i];
  }

  // pointer deref  $0427-$042E
  {
    u8 buf[4];
    buf[0] = 0xDE;
    buf[1] = 0xAD;
    buf[2] = 0xBE;
    buf[3] = 0xEF;
    ptr = buf;
    MEM(0x0427) = ptr[0];
    MEM(0x0428) = ptr[1];
    MEM(0x0429) = ptr[2];
    MEM(0x042A) = ptr[3];
    ptr = buf + 2;
    MEM(0x042B) = ptr[0];
    MEM(0x042C) = ptr[-1];
    MEM(0x042D) = *ptr;
    ptr++;
    MEM(0x042E) = *ptr;
  }

  // 16-bit compare  $042F-$0436
  {
    u16 ca, cb;
    ca = 0x1234;
    cb = 0x1234;
    MEM(0x042F) = (ca == cb) ? 1 : 0;
    MEM(0x0430) = (ca != cb) ? 1 : 0;
    ca = 0x1234;
    cb = 0x1235;
    MEM(0x0431) = (ca < cb) ? 1 : 0;
    MEM(0x0432) = (ca > cb) ? 1 : 0;
    ca = 0x1235;
    cb = 0x1234;
    MEM(0x0433) = (ca < cb) ? 1 : 0;
    MEM(0x0434) = (ca > cb) ? 1 : 0;
    ca = 0x00FF;
    cb = 0x0100;
    MEM(0x0435) = (ca < cb) ? 1 : 0;
    MEM(0x0436) = (ca > cb) ? 1 : 0;
  }

  // 8-bit compare  $0437-$043E
  {
    u8 ca, cb;
    ca = 0x80;
    cb = 0x80;
    MEM(0x0437) = (ca == cb) ? 1 : 0;
    ca = 0x7F;
    cb = 0x80;
    MEM(0x0438) = (ca < cb) ? 1 : 0;
    ca = 0x80;
    cb = 0x7F;
    MEM(0x0439) = (ca > cb) ? 1 : 0;
    ca = 0xFF;
    cb = 0x01;
    MEM(0x043A) = (ca < cb) ? 1 : 0;
    MEM(0x043B) = (ca >= cb) ? 1 : 0;
    ca = 0x01;
    cb = 0xFF;
    MEM(0x043C) = (ca <= cb) ? 1 : 0;
    MEM(0x043D) = (ca != cb) ? 1 : 0;
    MEM(0x043E) = (ca == cb) ? 1 : 0;
  }

  // switch  $043F-$0443
  {
    u8 sv;
    for (sv = 0; sv < 5; sv++) {
      switch (sv) {
      case 0:
        MEM(0x043F + sv) = 0xA0;
        break;
      case 1:
        MEM(0x043F + sv) = 0xA1;
        break;
      case 2:
        MEM(0x043F + sv) = 0xA2;
        break;
      case 3:
        MEM(0x043F + sv) = 0xA3;
        break;
      default:
        MEM(0x043F + sv) = 0xAF;
        break;
      }
    }
  }

  // shifts  $0444-$044B
  {
    u8 sh;
    u16 s16;
    sh = 0x01;
    MEM(0x0444) = sh << 1;
    sh = 0x80;
    MEM(0x0445) = sh >> 1;
    sh = 0xFF;
    MEM(0x0446) = sh << 4;
    sh = 0xFF;
    MEM(0x0447) = sh >> 4;
    s16 = 0x0001;
    MEM(0x0448) = (u8)(s16 << 8);
    MEM(0x0449) = (u8)((s16 << 8) >> 8);
    s16 = 0x8000;
    MEM(0x044A) = (u8)(s16 >> 8);
    MEM(0x044B) = (u8)s16;
  }

  // logical  $044C-$0453
  {
    u8 la, lb;
    la = 0;
    lb = 0;
    MEM(0x044C) = (la || lb) ? 1 : 0;
    la = 1;
    lb = 0;
    MEM(0x044D) = (la || lb) ? 1 : 0;
    la = 0;
    lb = 1;
    MEM(0x044E) = (la || lb) ? 1 : 0;
    la = 0xFF;
    lb = 0x01;
    MEM(0x044F) = (la && lb) ? 1 : 0;
    la = 0;
    lb = 0xFF;
    MEM(0x0450) = (la && lb) ? 1 : 0;
    la = 42;
    MEM(0x0451) = !la ? 1 : 0;
    la = 0;
    MEM(0x0452) = !la ? 1 : 0;
    la = 0xFF;
    MEM(0x0453) = !la ? 1 : 0;
  }

  // bitwise NOT  $0454-$0455
  {
    u8 bn;
    bn = 0x00;
    MEM(0x0454) = (u8)(~bn);
    bn = 0xA5;
    MEM(0x0455) = (u8)(~bn);
  }

  // pointer write  $0456-$0459
  {
    u8 dst[4];
    u8 *p;
    p = dst;
    *p++ = 0x11;
    *p++ = 0x22;
    *p++ = 0x33;
    *p = 0x44;
    MEM(0x0456) = dst[0];
    MEM(0x0457) = dst[1];
    MEM(0x0458) = dst[2];
    MEM(0x0459) = dst[3];
  }

  // 16-bit multiply  $045A-$045D
  {
    u16 m;
    m = (u16)12 * (u16)34;
    MEM(0x045A) = (u8)m;
    MEM(0x045B) = (u8)(m >> 8);
    m = (u16)255 * (u16)2;
    MEM(0x045C) = (u8)m;
    MEM(0x045D) = (u8)(m >> 8);
  }

  // 16-bit divide  $045E-$0461
  {
    u16 d;
    d = (u16)1000 / (u16)7;
    MEM(0x045E) = (u8)d;
    MEM(0x045F) = (u8)(d >> 8);
    d = (u16)0xFFFF / (u16)256;
    MEM(0x0460) = (u8)d;
    MEM(0x0461) = (u8)(d >> 8);
  }

  // 16-bit modulo  $0462-$0463
  {
    u16 rm;
    rm = (u16)1000 % (u16)7;
    MEM(0x0462) = (u8)rm;
    rm = (u16)0xFFFF % (u16)256;
    MEM(0x0463) = (u8)rm;
  }

  // loops  $0464-$0467
  {
    u8 sum;
    signed char wi;
    sum = 0;
    for (i = 0; i < 20; i++) {
      if (i == 5)
        continue;
      if (i == 10)
        break;
      sum += i;
    }
    MEM(0x0464) = sum;
    sum = 0;
    {
      u8 j;
      for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
          if (j == 2)
            continue;
          sum++;
        }
      }
    }
    MEM(0x0465) = sum;
    sum = 0;
    wi = 10;
    while (wi > 0) {
      sum += (u8)wi;
      wi -= 3;
    }
    MEM(0x0466) = sum;
    sum = 0;
    i = 0;
    do {
      sum += i;
      i++;
    } while (i < 5);
    MEM(0x0467) = sum;
  }

  // end marker
  MEM(0x0468) = 0xED;

  asm("brk");
}
