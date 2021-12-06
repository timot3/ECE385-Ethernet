//Modified version of Generic Types as included by Microchip C18

#ifndef __GENERIC_TYPE_DEFS_H_
#define __GENERIC_TYPE_DEFS_H_

typedef enum _BOOL {
	FALSE = 0, TRUE
} BOOL;	// Undefined size

#define	ON TRUE
#define	OFF	FALSE

typedef unsigned char BYTE;				// 8-bit unsigned
typedef unsigned short int WORD;				// 16-bit unsigned
typedef unsigned long DWORD;				// 32-bit unsigned
typedef unsigned long long QWORD;				// 64-bit unsigned
typedef signed char CHAR;				// 8-bit signed
typedef signed short int SHORT;				// 16-bit signed
typedef signed long LONG;				// 32-bit signed
typedef signed long long LONGLONG;			// 64-bit signed

typedef union _BYTE_VAL {
	BYTE Val;
	struct {
		unsigned char b0 :1;
		unsigned char b1 :1;
		unsigned char b2 :1;
		unsigned char b3 :1;
		unsigned char b4 :1;
		unsigned char b5 :1;
		unsigned char b6 :1;
		unsigned char b7 :1;
	} bits;
} BYTE_VAL;

typedef union _WORD_VAL {
	WORD Val;
	BYTE v[2];
	struct {
		BYTE LB;
		BYTE HB;
	} byte;
	struct {
		unsigned char b0 :1;
		unsigned char b1 :1;
		unsigned char b2 :1;
		unsigned char b3 :1;
		unsigned char b4 :1;
		unsigned char b5 :1;
		unsigned char b6 :1;
		unsigned char b7 :1;
		unsigned char b8 :1;
		unsigned char b9 :1;
		unsigned char b10 :1;
		unsigned char b11 :1;
		unsigned char b12 :1;
		unsigned char b13 :1;
		unsigned char b14 :1;
		unsigned char b15 :1;
	} bits;
} WORD_VAL;

typedef union _DWORD_VAL {
	DWORD Val;
	WORD w[2];
	BYTE v[4];
	struct {
		WORD LW;
		WORD HW;
	} word;
	struct {
		BYTE LB;
		BYTE HB;
		BYTE UB;
		BYTE MB;
	} byte;
	struct {
		unsigned char b0 :1;
		unsigned char b1 :1;
		unsigned char b2 :1;
		unsigned char b3 :1;
		unsigned char b4 :1;
		unsigned char b5 :1;
		unsigned char b6 :1;
		unsigned char b7 :1;
		unsigned char b8 :1;
		unsigned char b9 :1;
		unsigned char b10 :1;
		unsigned char b11 :1;
		unsigned char b12 :1;
		unsigned char b13 :1;
		unsigned char b14 :1;
		unsigned char b15 :1;
		unsigned char b16 :1;
		unsigned char b17 :1;
		unsigned char b18 :1;
		unsigned char b19 :1;
		unsigned char b20 :1;
		unsigned char b21 :1;
		unsigned char b22 :1;
		unsigned char b23 :1;
		unsigned char b24 :1;
		unsigned char b25 :1;
		unsigned char b26 :1;
		unsigned char b27 :1;
		unsigned char b28 :1;
		unsigned char b29 :1;
		unsigned char b30 :1;
		unsigned char b31 :1;
	} bits;
} DWORD_VAL;

typedef union _QWORD_VAL {
	QWORD Val;
	DWORD d[2];
	WORD w[4];
	BYTE v[8];
	struct {
		WORD LD;
		WORD HD;
	} dword;
	struct {
		WORD LW;
		WORD HW;
		WORD UW;
		WORD MW;
	} word;
	struct {
		unsigned char b0 :1;
		unsigned char b1 :1;
		unsigned char b2 :1;
		unsigned char b3 :1;
		unsigned char b4 :1;
		unsigned char b5 :1;
		unsigned char b6 :1;
		unsigned char b7 :1;
		unsigned char b8 :1;
		unsigned char b9 :1;
		unsigned char b10 :1;
		unsigned char b11 :1;
		unsigned char b12 :1;
		unsigned char b13 :1;
		unsigned char b14 :1;
		unsigned char b15 :1;
		unsigned char b16 :1;
		unsigned char b17 :1;
		unsigned char b18 :1;
		unsigned char b19 :1;
		unsigned char b20 :1;
		unsigned char b21 :1;
		unsigned char b22 :1;
		unsigned char b23 :1;
		unsigned char b24 :1;
		unsigned char b25 :1;
		unsigned char b26 :1;
		unsigned char b27 :1;
		unsigned char b28 :1;
		unsigned char b29 :1;
		unsigned char b30 :1;
		unsigned char b31 :1;
		unsigned char b32 :1;
		unsigned char b33 :1;
		unsigned char b34 :1;
		unsigned char b35 :1;
		unsigned char b36 :1;
		unsigned char b37 :1;
		unsigned char b38 :1;
		unsigned char b39 :1;
		unsigned char b40 :1;
		unsigned char b41 :1;
		unsigned char b42 :1;
		unsigned char b43 :1;
		unsigned char b44 :1;
		unsigned char b45 :1;
		unsigned char b46 :1;
		unsigned char b47 :1;
		unsigned char b48 :1;
		unsigned char b49 :1;
		unsigned char b50 :1;
		unsigned char b51 :1;
		unsigned char b52 :1;
		unsigned char b53 :1;
		unsigned char b54 :1;
		unsigned char b55 :1;
		unsigned char b56 :1;
		unsigned char b57 :1;
		unsigned char b58 :1;
		unsigned char b59 :1;
		unsigned char b60 :1;
		unsigned char b61 :1;
		unsigned char b62 :1;
		unsigned char b63 :1;
	} bits;
} QWORD_VAL;

#ifndef NULL
#define NULL 0
#endif

#endif //__GENERIC_TYPE_DEFS_H_
