#pragma once
#ifndef _XOR_H_
#define _XOR_H_

#pragma once


template<INT XORSTART, INT BUFLEN, INT XREFKILLER>

class XorStr
{
private:
	XorStr();
public:
	CHAR s[BUFLEN];

	XorStr(CONST CHAR *xs);
	~XorStr()
	{
		for (INT i = 0; i < BUFLEN; i++)
			s[i] = 0;
	}
};

template<INT XORSTART, INT BUFLEN, INT XREFKILLER>

XorStr<XORSTART, BUFLEN, XREFKILLER>::XorStr(CONST CHAR *xs)
{
	INT xvalue = XORSTART;
	INT i = 0;

	for (; i < (BUFLEN - 1); i++)
	{
		s[i] = xs[i - XREFKILLER] ^ xvalue;
		xvalue += 1;
		xvalue %= 256;
	}

	s[BUFLEN - 1] = 0;
}


#endif

#pragma once
