#include "stdafx.h"
#include "h\util.h"
namespace UI { namespace Util
{

#ifdef UTIL_OTHER

static char base64_table[] =
       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
enum Base64Option{BASE64_NeedCRLF=1};

int base64_encode(PBYTE bin_data, LONG bin_size, PCHAR base64_data, PLONG base64_size, INT Mode)
{
	long            i,j,k;
	long            blk_size,remain_size;
	unsigned char   *p;
	unsigned char   left[3];
	int             i64;

	blk_size    = bin_size / 3;
	remain_size = bin_size % 3;
	p   = bin_data;
	j   = 0;
	i64 = 0;

	for(i = 0; i < blk_size; i ++)
	{
		k = (p[0] & 0xFC) >> 2;
		base64_data[j++] = base64_table[k];

		k = ((p[0] & 0x03) << 4) | (p[1] >> 4);
		base64_data[j++] = base64_table[k];

		k = ((p[1] & 0x0F) << 2) | (p[2] >> 6);
		base64_data[j++] = base64_table[k];

		k = p[2] & 0x3F;
		base64_data[j++] = base64_table[k];

		i64 ++;
		i64 ++;
		i64 ++;
		i64 ++;

		if((Mode == BASE64_NeedCRLF) && (i64 % 64) == 0)
		{
			base64_data[j ++] = (char)0x0d;
			base64_data[j ++] = (char)0x0a;
			i64 = 0;
		}

		p += 3;
	}

	switch(remain_size)
	{
	case 0:
		break;

	case 1:
		left[0] = p[0];
		left[1] = 0;
		p = left;

		k = (p[0] & 0xFC) >> 2;
		base64_data[j ++] = base64_table[k];
		k = ((p[0] & 0x03) << 4) | (p[1] >> 4);
		base64_data[j ++] = base64_table[k];

		base64_data[j ++] = '=';
		base64_data[j ++] = '=';
		break;

	case 2:
			left[0] = p[0];
			left[1] = p[1];
			left[2] = 0;
			p = left;

            k = (p[0] & 0xFC) >> 2;
            base64_data[j ++] = base64_table[k];
            k = ((p[0] & 0x03) << 4) | (p[1] >> 4);
            base64_data[j ++] = base64_table[k];
            k = ((p[1] & 0x0F) << 2) | (p[2] >> 6);
            base64_data[j ++] = base64_table[k];
			base64_data[j ++] = '=';
			break;

		default:
			break;
	}

	base64_data[j] = 0;
	*base64_size = j;

	return 0;
}

int base64_decode(PCHAR base64_data, LONG base64_size, PBYTE bin_data, PLONG bin_size)
{
    long i, j, k, m, n, l;
    unsigned char four_bin[4];
    char four_char[4];
    char c;
    
    
    j = base64_size;
    i = 0;
    l = 0;
    
    for(;;)
    {
        if((i+4) > j)
        {
            break;
        }
        
        k = 0;
        while(k < 4)
        {
            if(i == j)
            {
                break;
            }
            
            c = base64_data[i ++];
			
			//{Add By leeihcy
			// 去除其中的 0D 0A 换行符
			if( c == 0x0D && base64_data[i] == 0x0A )
			{
				i ++;
				c = base64_data[ i++ ];
			}
			//}End Add
            
			if((c == '+') || (c == '/') || (c == '=') ||
                ((c >= '0') && (c <= '9')) ||
                ((c >= 'A') && (c <='Z')) ||
                ((c >= 'a') && (c <= 'z')))
            {
                four_char[k ++] = c;
            }
			else
			{
				return -1;
			}
        }
        
        if(k != 4)
        {
            return -1;
        }
        
        n = 0;
        for(k = 0; k < 4; k ++)
        {
            if(four_char[k] != '=')
            {
                for(m = 0; m < 64; m ++)
                {
                    if(base64_table[m] == four_char[k])
                    {
                        four_bin[k] = (unsigned char)m;
                        break;
                    }
                }
            }
            else
            {
                n ++;
            }
        }
        
        switch(n)
        {
        case 0:
            bin_data[l ++] = (four_bin[0] << 2) | (four_bin[1] >> 4);
            bin_data[l ++] = (four_bin[1] << 4) | (four_bin[2] >> 2);
            bin_data[l ++] = (four_bin[2] << 6) | four_bin[3]; 
            break;
            
        case 1:
            bin_data[l ++] = (four_bin[0] << 2) | (four_bin[1] >> 4);
            bin_data[l ++] = (four_bin[1] << 4) | (four_bin[2] >> 2);
            break;
            
        case 2:
            bin_data[l ++] = (four_bin[0] << 2) | (four_bin[1] >> 4);
            break;
            
        default:
            break;
        }
        
        if(n != 0)
        {
            break;
        }
    }
    
    *bin_size = l;
    
    return 0;
}

int bin2hex(PBYTE indata, INT inlen, PCHAR outdata, PINT outlen)
{
	int 	i;
	char	bcd[3];
	
	if (*outlen >= inlen * 2 + 1) {
		memset(outdata, 0, *outlen);
		for(i=0; i<inlen; i++) {
			sprintf(bcd, "%02X", indata[i]);
			strcat((char *)outdata, bcd);
		}
		*outlen = inlen * 2;	
		return 0;
	} else {
		*outlen = inlen * 2 + 1;	
		return -1;
	}
}

int hex2bin(PCHAR indata, INT inlen, PBYTE outdata, PINT outlen)
{
	int 	        i;
	unsigned char	c;
	short	        c1,c2;
	char           *tempdata = indata;
	int             malloctag = 0;
	
	// 补足整数倍
	if (inlen % 2 != 0) {
		inlen = inlen + 1;
		tempdata = (char *)malloc (inlen + 1);
		tempdata[0] = '0';
		strcpy (&tempdata[1], indata);
		malloctag = 1;
	}

	if (*outlen >= inlen / 2 + 1) {
		memset (outdata, 0, *outlen);
		for (i=0; i<inlen /2 ; i++) {				
			c1 = (short)((toupper (tempdata[i*2])<='9') ?
				(tempdata[i*2]-'0') : (tempdata[i*2]-'A'+10));
			c2 = (short)((toupper (tempdata[i*2+1])<='9') ? 
				(tempdata[i*2+1]-'0') : (tempdata[i*2+1]-'A'+10));
			c = c1 * 16 + c2;
			outdata[i] = c;		
		}
		*outlen = i;
		if (malloctag == 1)
			free (tempdata);
		return 0;
	} else {
		if (malloctag == 1)
			free (tempdata);
		*outlen = inlen / 2 + 1;	
		return -1;
	}
}


#endif

}}