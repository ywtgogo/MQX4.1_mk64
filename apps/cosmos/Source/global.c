/*
*	提供全局调用函数的原型,以及全局变量原型
*/

#include "global.h"

MUTEX_STRUCT                     mutex_serial_rf;


/*   Function Code       */
void print_sys_time
(
    void
)
{ /* Body*/
    TIME_STRUCT  	ts;
    DATE_STRUCT  	tm;
	
	uint32_t		sys_time;
	TIME_STRUCT		mqx_time;
	
	_time_get(&mqx_time);
	sys_time = mqx_time.SECONDS;
	
    ts.SECONDS = sys_time;
    ts.MILLISECONDS = 0;
    /* Convert rtc_time to date format */
    if (_time_to_date(&ts, &tm) == FALSE ) {
        printf("\n Cannot convert mqx_time to date format");
    }
    printf("\n (SYS) Current time is: %4d-%.2d-%.2d %.2d:%.2d:%.2d ",
            //_days_abbrev[tm.WDAY],
            //_months_abbrev[tm.MONTH - 1],
		   	tm.YEAR,
			tm.MONTH,
            tm.DAY,
            tm.HOUR,
            tm.MINUTE,
            tm.SECOND
            );
} /* Endboy*/

/*   Function Code       */
void print_rtc_time
(
    void
)
{ /* Body*/
    TIME_STRUCT  	ts;
    DATE_STRUCT  	tm;
	
	uint32_t		rtc_time;
	
	_rtc_get_time(&rtc_time);
	
    ts.SECONDS = rtc_time;
    ts.MILLISECONDS = 0;
    /* Convert rtc_time to date format */
    if (_time_to_date(&ts, &tm) == FALSE ) {
        printf("\n Cannot convert rtc_time to date format");
    }
    printf("\n (RTC) Current time is: %4d-%.2d-%.2d %.2d:%.2d:%.2d ",
            //_days_abbrev[tm.WDAY],
            //_months_abbrev[tm.MONTH - 1],
		   	tm.YEAR,
			tm.MONTH,
            tm.DAY,
            tm.HOUR,
            tm.MINUTE,
            tm.SECOND
            );
} /* Endboy*/

void StrToHex(char *src, uchar *dst)    //字符串转换为HEX码
{
	int i,j=0;
	
//	printf("\n%x, %x, %x, %x", src[0], src[1], src[2], src[3]);
//	printf("\n%x, %x, %x, %x", dst[0], dst[1], dst[2], dst[3]);
	for(i=0;i<strlen(src);i+=2)//strlen(src)/2
	{
		if (src[i]>='0'&&src[i]<='9'){
			dst[j] = (src[i]-'0')<<4;
		}else if (src[i]>='A'&&src[i]<='Z'){
			dst[j] = (src[i]-'A'+10)<<4;
		}else if (src[i]>='a'&&src[i]<='z'){
			dst[j] = (src[i]-'a'+10)<<4;
		}
		if (src[i+1]>='0'&&src[i+1]<='9'){
			dst[j] = dst[j]|(src[i+1]-'0');
		}else if (src[i+1]>='A'&&src[i+1]<='Z'){
			dst[j] = dst[j]|(src[i+1]-'A'+10);
		}else if (src[i+1]>='a'&&src[i+1]<='z'){
			dst[j] = dst[j]|(src[i+1]-'a'+10);
		}
		j++;
	}
}

static char a2x(char ch)
{
    switch(ch)
    {
	case 0:
		return '0';
    case 1:
        return '1';
    case 2:
        return '2';
    case 3:
        return '3';
    case 4:
        return '4';
    case 5:
        return '5';
    case 6:
        return '6';
    case 7:
        return '7';
    case 8:
        return '8';
    case 9:
        return '9';
    case 10:
        return 'A';
    case 11:
        return 'B';
    case 12:
        return 'C';
    case 13:
        return 'D';
    case 14:
        return 'E';
    case 15:
        return 'F';
    default:
        break;
    }
    return ch;//若有字符输入，字符原样返回
}
void ToString(uchar *src, char *dst, uint32_t slen)
{
	char 		ch;
	uint32_t 	i = 0;
//	i = slen*2;
	for (i=0;i<slen;i++)
	{
		dst[2*i] = a2x(src[i]>>4);//) | a2x(src[i + 1]);
		dst[2*i+1]=a2x(src[i]&0x0F);
	}
	dst[2*slen]='\0';
	
//	printf("\n%s", dst);
}

/* End of File */
