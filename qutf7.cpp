/*
 *  QConsole by Davide Libenzi ( Symbian console server )
 *  Copyright (C) 2004  Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#include "qinclude.h"



static int const index_64[128] =
{
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
		-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
		52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1,
		-1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
		15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
		-1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
		41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1
};

static char const base_64[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
		'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
		't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', '+', '/'
};



TUint16 *Utf7ToUnicode(const char *u7, int u7len, TUint16 **u16, int *u16len)
{
	char *buf;
	TUint16 *p;
	int b, ch, k;

	if ((buf = (char *) User::Alloc(2 * u7len + 1)) == NULL)
		return NULL;
	for (p = (TUint16 *) buf; u7len; u7++, u7len--)
	{
		if (*u7 == '&')
		{
			u7++, u7len--;

			if (u7len && *u7 == '-')
			{
				*p++ = (TUint16) '&';
				continue;
			}

			ch = 0;
			k = 10;
			for (; u7len; u7++, u7len--)
			{
				if ((*u7 & 0x80) || (b = index_64[(int) *u7]) == -1)
					break;
				if (k > 0)
				{
					ch |= b << k;
					k -= 6;
				}
				else
				{
					ch |= b >> (-k);
					*p++ = (TUint16) ch;
					ch = (b << (16 + k)) & 0xffff;
					k += 10;
				}
			}
			if (ch || k < 6)
				goto error_exit;
			if (!u7len || *u7 != '-')
				goto error_exit;
			if (u7len > 2 && u7[1] == '&' && u7[2] != '-')
				goto error_exit;
		}
		else if (*u7 < 0x20 || *u7 >= 0x7f)
			goto error_exit;
		else
			*p++ = (TUint16) *u7;
	}
	if (u16len)
		*u16len = p - (TUint16 *) buf;

	buf = (char *) User::ReAlloc(buf, (char *) p - buf);
	if (u16)
		*u16 = (TUint16 *) buf;
	return (TUint16 *) buf;

	error_exit:
	User::Free(buf);
	return NULL;
}


char *UnicodeToUtf7(const TUint16 *u16, int u16len, char **u7, int *u7len)
{
	char *buf, *p;
	int ch;
	int b = 0, k = 0;
	int base64 = 0;

	if ((p = buf = (char *) User::Alloc((u16len / 2) * 7 + 6)) == NULL)
		return NULL;
	while (u16len)
	{
		ch = *u16;
		u16++, u16len--;

		if (ch < 0x20 || ch >= 0x7f)
		{
			if (!base64)
			{
				*p++ = '&';
				base64 = 1;
				b = 0;
				k = 10;
			}
			if (ch & ~0xffff)
				ch = 0xfffe;
			*p++ = base_64[b | ch >> k];
			k -= 6;
			for (; k >= 0; k -= 6)
				*p++ = base_64[(ch >> k) & 0x3f];
			b = (ch << (-k)) & 0x3f;
			k += 16;
		}
		else
		{
			if (base64)
			{
				if (k > 10)
					*p++ = base_64[b];
				*p++ = '-';
				base64 = 0;
			}
			*p++ = ch;
			if (ch == '&')
				*p++ = '-';
		}
	}
	if (base64)
	{
		if (k > 10)
			*p++ = base_64[b];
		*p++ = '-';
	}
	*p++ = '\0';
	if (u7len)
		*u7len = p - buf;
	buf = (char *) User::ReAlloc(buf, p - buf);
	if (u7)
		*u7 = buf;
	return buf;
}

