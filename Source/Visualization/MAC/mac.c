#include "stdio.h"
#include "string.h"



#define EN0 0   // MODE == encrypt
#define DE1 1   // MODE == decrypt

typedef union {
    unsigned long blok[2];
    unsigned short word[4];
    unsigned char byte[8];
    } M68K;

extern void des8(unsigned char *InData,unsigned char *key,unsigned char *OutData,short Mode,int readlen);
extern void des16(unsigned char *InData,unsigned char *key,unsigned char *OutData,short Mode,int readlen);


static void scrunch(unsigned char *, unsigned long *);
static void unscrun(unsigned long *, unsigned char *);
static void desfunc(unsigned long *, unsigned long *);
static void cookey(unsigned long *);

static unsigned long KnL[32] = { 0L };
static unsigned long KnR[32] = { 0L };
static unsigned long Kn3[32] = { 0L };
static unsigned char Df_Key[24] = {
    0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
    0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,
    0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67 };

static unsigned short bytebit[8]    = {
    0200, 0100, 040, 020, 010, 04, 02, 01 };

static unsigned long bigbyte[24] = {
    0x800000L,  0x400000L,  0x200000L,  0x100000L,
    0x80000L,   0x40000L,   0x20000L,   0x10000L,
    0x8000L,    0x4000L,    0x2000L,    0x1000L,
    0x800L,     0x400L,     0x200L,     0x100L,
    0x80L,      0x40L,      0x20L,      0x10L,
    0x8L,       0x4L,       0x2L,       0x1L    };

static unsigned char pc1[56] = {
    56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,
     9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35,
    62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
    13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11,  3 };

static unsigned char totrot[16] = {
    1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28 };

static unsigned char pc2[48] = {
    13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9,
    22, 18, 11,  3, 25,  7, 15,  6, 26, 19, 12,  1,
    40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31 };

void deskey(unsigned char * key,short edf)
{
    register int i, j, l, m, n;
    unsigned char pc1m[56], pcr[56];
    unsigned long kn[32];

    for ( j = 0; j < 56; j++ ) {
        l = pc1[j];
        m = l & 07;
        pc1m[j] = (key[l >> 3] & bytebit[m]) ? 1 : 0;
        }
    for( i = 0; i < 16; i++ ) {
        if( edf == DE1 ) m = (15 - i) << 1;
        else m = i << 1;
        n = m + 1;
        kn[m] = kn[n] = 0L;
        for( j = 0; j < 28; j++ ) {
            l = j + totrot[i];
            if( l < 28 ) pcr[j] = pc1m[l];
            else pcr[j] = pc1m[l - 28];
            }
        for( j = 28; j < 56; j++ ) {
            l = j + totrot[i];
            if( l < 56 ) pcr[j] = pc1m[l];
            else pcr[j] = pc1m[l - 28];
            }
        for( j = 0; j < 24; j++ ) {
            if( pcr[pc2[j]] ) kn[m] |= bigbyte[j];
            if( pcr[pc2[j+24]] ) kn[n] |= bigbyte[j];
            }
        }
    cookey(kn);
    return;
    }

void usekey(register unsigned long *from);
static void cookey(register unsigned long *raw1)
{
    register unsigned long *cook, *raw0;
    unsigned long dough[32];
    register int i;

    cook = dough;
    for( i = 0; i < 16; i++, raw1++ ) {
        raw0 = raw1++;
        *cook    = (*raw0 & 0x00fc0000L) << 6;
        *cook   |= (*raw0 & 0x00000fc0L) << 10;
        *cook   |= (*raw1 & 0x00fc0000L) >> 10;
        *cook++ |= (*raw1 & 0x00000fc0L) >> 6;
        *cook    = (*raw0 & 0x0003f000L) << 12;
        *cook   |= (*raw0 & 0x0000003fL) << 16;
        *cook   |= (*raw1 & 0x0003f000L) >> 4;
        *cook++ |= (*raw1 & 0x0000003fL);
        }
    usekey(dough);  //将结果存入Knl
    return;
    }

void usekey(register unsigned long *from)
{
    register unsigned long *to, *endp;

    to = KnL, endp = &KnL[32];
    while( to < endp ) *to++ = *from++;
    return;
    }

void cpkey(register unsigned long *into)
{
    register unsigned long *from, *endp;

    from = KnL, endp = &KnL[32];
    while( from < endp ) *into++ = *from++;
    return;
    }

static void scrunch(register unsigned char *outof,register unsigned long *into)
{
    *into    = (*outof++ & 0xffL) << 24;
    *into   |= (*outof++ & 0xffL) << 16;
    *into   |= (*outof++ & 0xffL) << 8;
    *into++ |= (*outof++ & 0xffL);
    *into    = (*outof++ & 0xffL) << 24;
    *into   |= (*outof++ & 0xffL) << 16;
    *into   |= (*outof++ & 0xffL) << 8;
    *into   |= (*outof   & 0xffL);
    return;
    }

static void unscrun(register unsigned long *outof,register unsigned char *into)
{
    *into++ = (unsigned char)((*outof >> 24) & 0xffL);
    *into++ = (unsigned char)((*outof >> 16) & 0xffL);
    *into++ = (unsigned char)((*outof >>  8) & 0xffL);
    *into++ = (unsigned char)( *outof++  & 0xffL);
    *into++ = (unsigned char)((*outof >> 24) & 0xffL);
    *into++ = (unsigned char)((*outof >> 16) & 0xffL);
    *into++ = (unsigned char)((*outof >>  8) & 0xffL);
    *into   = (unsigned char)( *outof    & 0xffL);
    return;
    }

static unsigned long SP1[64] = {
    0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
    0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
    0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
    0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
    0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
    0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
    0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
    0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
    0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
    0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
    0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
    0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
    0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
    0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
    0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
    0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L };

static unsigned long SP2[64] = {
    0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
    0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
    0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
    0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
    0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
    0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
    0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
    0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
    0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
    0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
    0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
    0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
    0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
    0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
    0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
    0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L };

static unsigned long SP3[64] = {
    0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
    0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
    0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
    0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
    0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
    0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
    0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
    0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
    0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
    0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
    0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
    0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
    0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
    0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
    0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
    0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L };

static unsigned long SP4[64] = {
    0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
    0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
    0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
    0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
    0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
    0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
    0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
    0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
    0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
    0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
    0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
    0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
    0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
    0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
    0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
    0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L };

static unsigned long SP5[64] = {
    0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
    0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
    0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
    0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
    0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
    0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
    0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
    0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
    0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
    0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
    0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
    0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
    0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
    0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
    0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
    0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L };

static unsigned long SP6[64] = {
    0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
    0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
    0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
    0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
    0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
    0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
    0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
    0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
    0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
    0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
    0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
    0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
    0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
    0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
    0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
    0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L };

static unsigned long SP7[64] = {
    0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
    0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
    0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
    0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
    0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
    0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
    0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
    0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
    0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
    0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
    0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
    0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
    0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
    0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
    0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
    0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L };

static unsigned long SP8[64] = {
    0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
    0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
    0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
    0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
    0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
    0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
    0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
    0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
    0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
    0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
    0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
    0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
    0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
    0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
    0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
    0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L };

static void desfunc(register unsigned long *block,register unsigned long *keys)
{
    register unsigned long fval, work, right, leftt;
    register int round;

    leftt = block[0];
    right = block[1];
    work = ((leftt >> 4) ^ right) & 0x0f0f0f0fL;
    right ^= work;
    leftt ^= (work << 4);
    work = ((leftt >> 16) ^ right) & 0x0000ffffL;
    right ^= work;
    leftt ^= (work << 16);
    work = ((right >> 2) ^ leftt) & 0x33333333L;
    leftt ^= work;
    right ^= (work << 2);
    work = ((right >> 8) ^ leftt) & 0x00ff00ffL;
    leftt ^= work;
    right ^= (work << 8);
    right = ((right << 1) | ((right >> 31) & 1L)) & 0xffffffffL;
    work = (leftt ^ right) & 0xaaaaaaaaL;
    leftt ^= work;
    right ^= work;
    leftt = ((leftt << 1) | ((leftt >> 31) & 1L)) & 0xffffffffL;

    for( round = 0; round < 8; round++ ) {
        work  = (right << 28) | (right >> 4);
        work ^= *keys++;
        fval  = SP7[ work        & 0x3fL];
        fval |= SP5[(work >>  8) & 0x3fL];
        fval |= SP3[(work >> 16) & 0x3fL];
        fval |= SP1[(work >> 24) & 0x3fL];
        work  = right ^ *keys++;
        fval |= SP8[ work        & 0x3fL];
        fval |= SP6[(work >>  8) & 0x3fL];
        fval |= SP4[(work >> 16) & 0x3fL];
        fval |= SP2[(work >> 24) & 0x3fL];
        leftt ^= fval;
        work  = (leftt << 28) | (leftt >> 4);
        work ^= *keys++;
        fval  = SP7[ work        & 0x3fL];
        fval |= SP5[(work >>  8) & 0x3fL];
        fval |= SP3[(work >> 16) & 0x3fL];
        fval |= SP1[(work >> 24) & 0x3fL];
        work  = leftt ^ *keys++;
        fval |= SP8[ work        & 0x3fL];
        fval |= SP6[(work >>  8) & 0x3fL];
        fval |= SP4[(work >> 16) & 0x3fL];
        fval |= SP2[(work >> 24) & 0x3fL];
        right ^= fval;
        }

    right = (right << 31) | (right >> 1);
    work = (leftt ^ right) & 0xaaaaaaaaL;
    leftt ^= work;
    right ^= work;
    leftt = (leftt << 31) | (leftt >> 1);
    work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
    right ^= work;
    leftt ^= (work << 8);
    work = ((leftt >> 2) ^ right) & 0x33333333L;
    right ^= work;
    leftt ^= (work << 2);
    work = ((right >> 16) ^ leftt) & 0x0000ffffL;
    leftt ^= work;
    right ^= (work << 16);
    work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
    leftt ^= work;
    right ^= (work << 4);
    *block++ = right;
    *block = leftt;
    return;
    }
//int DES_Encrypt(unsigned char *sXorData,unsigned char *sMacKey,unsigned char *sDesData)
void DES_Encrypt(unsigned char *InData,unsigned char *key,unsigned char *OutData)
//参数:
//1.InData存放待加/解密的内存指针(长度为readlen,可能经过填充);
//2.Key存放用户输入的密钥内存的指针
//3.OutData存放输出结果
//4.Mode加/解密选择（加/解密=EN0/DE1=0/1）
//5.readlen待加/解密的长度(8字节的倍数，如：8字节数据readlen=1、16字节数据readlen=2等等)
//功能:
//根据Mode生成16个48位的加/解密子密钥,
//把待加/解密的数据分割成64位的块,逐块完成16次迭代加/解密,
//结果存放在OutData所指向的内存中.
//说明：
//八字节输入、八字节密钥、八字节输出
{
    int i=0;
    unsigned long work[2];
    unsigned char *cpi,*cpo;
    cpi=InData;
    cpo=OutData;
    deskey(key,0);//由八字节key，根据Mode(加/解密=EN0/DE1=0/1)所指方式，生成16个48位的加/解密子密钥，子密钥存入KnL中。

    for(i=0;i<1;i++)
    {
        //cpi[0]->work[0]第三字节、cpi[1]->work[0]第二字节、cpi[2]->work[0]第一字节、cpi[3]->work[0]第零字节。
        //cpi[4]->work[1]第三字节、cpi[5]->work[1]第二字节、cpi[6]->work[1]第一字节、cpi[7]->work[1]第零字节。
        scrunch(cpi,work);

        //用KnL中的16个48位的加/解密子密钥，完成16次迭代加/解密
        desfunc(work,KnL);

        //work[0]第三字节->cpo[0]、work[0]第二字节->cpo[1]、work[0]第一字节->cpo[2]、work[0]第零字节->cpo[3]。
        //work[1]第三字节->cpo[4]、work[1]第二字节->cpo[5]、work[1]第一字节->cpo[6]、work[1]第零字节->cpo[7]。
        unscrun(work,cpo);

        //指向下一组数据
        cpi+=8; cpo+=8;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

void print(unsigned char * tempt)
{
    int i=0;
    for (i=0;i<30;i++)
    {
    printf("%d",i);
    printf("%s"," ~");
    printf("%x",tempt[i]);
    printf("%s","  ");
    }
    printf("%s\n"," ");
}
void xor(unsigned char *input1,unsigned char *input2,unsigned char *output,int len)
{
    while (len) {
        *output++=*input1++^*input2++;
        len--;
    }
}
/*
*@brief: 根据输入数据计算MAC，初始IV向量默认为"x00x00x00x00x00x00x00x00"
*@param: sMacKey 密钥
*@param: pInData 输入数据
*@param: iInLen 输入数据长度
*@param: pRetData 计算出来的MAC
*@调用自定义xor和des函数
*/

void MacArithmetic(unsigned char *sMacKey,unsigned char *pInData,int iInLen,unsigned char *pRetData)
{
//MAC算法:
//将字符串pInata分为8字节为单位的数据块，不足补x00,分别标号为D1,D2,D3,...,Dn
//设置初始向量E0="x00x00x00x00x00x00x00x00"
//将E0^D1 —---->E1(E0,D1异或的后结果经des加密得到E1)
//将E1^D2 ----->E2
//如此类推，知道得出En结束，En即是计算出来的MAC
 char s1=NULL;
    unsigned char sUpData[512];
    unsigned char sData[20];
    unsigned char sXorData[20];
    unsigned char sDesData[20];
    int i,n,iNum;

    memset(sUpData,0,sizeof(sUpData));
    memset(sData,0,sizeof(sData));
    memset(sXorData,0,sizeof(sXorData));
    memset(sDesData,0,sizeof(sDesData));

//补全要加密数据成8倍数到sUpData,不足补x00
    memcpy(sUpData,pInData,iInLen);
    iNum = iInLen%8;
    if (iNum == 0)
        n=iInLen/8;
    else {
        n=iInLen/8+1;
    int ii=0;
    for (ii=0;ii<8-iNum;ii++)
        sUpData[iInLen+ii]=0x00;
//  memcpy(sUpData+iInLen,"x00x00x00x00x00x00x00x00",8-iNum);
    }

//  printf("n=%dnsUpData=[%s]n",n,sUpData);

//轮循异或，加密
    int ii=0;
    for (ii=0;ii<8-iNum;ii++)
        sDesData[ii]=0x00;
//  memcpy(sDesData,"x00x00x00x00x00x00x00x00",8); //初始向量赋给sDesData
    for (i=0;i<n;i++) {
//拷贝sUpData中下一个8位到sData
        for (ii=0;ii<8;ii++)
    sData[ii]=sUpData[i*8+ii];
            //  memcpy(sData,sUpData+i*8,8);
        xor(sDesData,sData,sXorData,8); //异或

        DES_Encrypt(sXorData,sMacKey,sDesData); //xordata是明文,desdata是密文
    }
    for (ii=0;ii<4;ii++)
    pRetData[ii]=sDesData[ii];
//  memcpy(pRetData,sDesData,4);

    return ;
}

int  main()
{
unsigned char  sMacKey[8]={0x4f,0xf6,0x3e,0x5d,0x02,0xff,0xca,0x1b};            //密钥的定义
unsigned char pInData[19]={0x00,0x00,0x12,0x34,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x20,0x12,0x12,0x07,0x16,0x41,0x00,0x80};//加密数据的定义,加密数据定长.
unsigned char pRetData[4];

MacArithmetic(sMacKey,pInData,19,pRetData);

int i=0;
for (i=0;i<4;i++)
        printf("%x",pRetData[i]);
return 0;
}

//测试输入用例：
//密钥：4f f6 3e 5d 02 ff ca 1b
//数据：00 00 12 34 02 00 00 00 00 00 01 20 12 12 07 16 41 00 80
//初始化数据：00 00 00 00 00 00 00 00
//补值：80
//mac加密结果：44 9f d3 63