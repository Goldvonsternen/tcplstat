/*
 * tcplstat - TCP packets monitor and statistical tool
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "tcplstat_in.h"

/* ����һ���ڴ�� */
char *memndup( const char *s, size_t n )
{
	char	*p = NULL ;
	
	p = (char*)malloc( n ) ;
	if( p == NULL )
		return NULL;
	memcpy( p , s , n );
	
	return p;
}

/* ���ڴ���в�ѯ�ַ��� */
/* binary_mode��־�Ƿ��ܽ���������������� */
char *memistr2_region( char *p_curr , char *find , char *end , unsigned char binary_mode )
{
	char	*p_match = NULL ;
	char	*bak = NULL ;
	
	while( p_curr <= end )
	{
		if( binary_mode == 0 && ! (isprint(*p_curr)||(*p_curr)=='\t') )
			break;
		
		if( p_match == NULL )
		{
			if( toupper(*p_curr) == toupper(*find) )
			{
				p_match = find + 1 ;
				bak = p_curr ;
			}
		}
		else
		{
			if( toupper(*p_curr) == toupper(*p_match) )
			{
				p_match++;
				if( (*p_match) == '\0' )
					return bak;
			}
			else
			{
				p_match = NULL ;
				p_curr = bak ;
			}
		}
		
		p_curr++;
	}
	
	return NULL;
}

/* ��ÿɼ��ַ�����Ч���� */
int LengthUtilEndOfText( char *p_curr , char *end )
{
	char	*p = NULL ;
	
	for( p = p_curr ; p <= end ; p++ )
	{
		if( (*p) == '\r' || (*p) == '\n' )
		{
			(*p) = ' ' ;
		}
		else if( ! (isprint(*p)||(*p)=='\t') )
		{
			p--;
			break;
		}
	}
	
	return p-p_curr+1;
}

char *ConvDateTimeHumanReadable( time_t tt )
{
	struct tm	tm ;
	static char	date_time_buf[ 19 + 1 ] ;
	
#if ( defined __linux ) || ( defined _AIX )
	localtime_r( & tt , & tm ) ;
#elif ( defined _WIN32 )
	localtime_s( & tm , & tt ) ;
#endif
	sprintf( date_time_buf , "%04d-%02d-%02dT%02d:%02d:%02d" , tm.tm_year+1900 , tm.tm_mon+1 , tm.tm_mday , tm.tm_hour , tm.tm_min , tm.tm_sec );
	
	return date_time_buf;
}

/* ���ʮ�����Ƹ�ʽ������ */
int DumpBuffer( FILE *fp , char *indentation , int buf_len , void *buf )
{
	int		lines_offset , bytes_offset ;
	
	if( indentation == NULL )
		indentation = "" ;
	
	/* д��־ */
	fprintf( fp , "%s             0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F    0123456789ABCDEF\n" , indentation );
	
	lines_offset = 0 ;
	bytes_offset = 0 ;
	
	while(1)
	{
		fprintf( fp , "%s0x%08X   " , indentation , (unsigned int)(lines_offset*16) );
		
		for( bytes_offset = 0 ; bytes_offset < 16 ; bytes_offset++ )
		{
			if( lines_offset*16 + bytes_offset < buf_len )
				fprintf( fp , "%02X " , *( (unsigned char *)buf + lines_offset * 16 + bytes_offset ) );
			else
				fprintf( fp , "   " );
		}
		
		fprintf( fp , "  " );
		
		for( bytes_offset = 0 ; bytes_offset < 16 ; bytes_offset++ )
		{
			if( lines_offset*16 + bytes_offset < buf_len )
			{
				unsigned char		ch = *( (unsigned char *)buf + lines_offset * 16 + bytes_offset ) ;
				
				if( 32 <= ch && ch <= 125 )
					fprintf( fp , "%c" , ch );
				else
					fputc( '.' , fp );
			}
			else
			{
				fprintf( fp , " " );
			}
		}
		
		fputc( '\n' , fp );
		
		if( ! ( lines_offset*16 + bytes_offset < buf_len ) )
			break;
		
		lines_offset++;
		
		if( ! ( lines_offset*16 < buf_len ) )
			break;
	}
	
	return 0;
}

