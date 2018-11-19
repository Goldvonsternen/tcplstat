/*
 * tcplstat - TCP packets monitor and statistical tool
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "tcplstat_in.h"

/* for testing tcplstat
sudo tcplstat -f "tcp port 445" -o "dESPD"
*/

/* for testing
echo "hello" | nc 192.168.6.21 445
*/

char    __TCPLSTAT_VERSION_0_10_1[] = "0.10.1" ;
char    *__TCPLSTAT_VERSION = __TCPLSTAT_VERSION_0_10_1 ;

struct TcplStatEnv	*g_p_env = NULL ;

#define _TCPLSTAT_DEBUG_FLUSH		1

/* ��ʾ�汾 */
static void version()
{
	printf( "tcplstat v%s build %s %s\n" , __TCPLSTAT_VERSION , __DATE__ , __TIME__ );
	printf( "copyright by calvin<calvinwilliams@163.com> 2018\n" );
	return;
}

/* ��ʾ�������﷨ */
static void usage()
{
	printf( "USAGE : tcplstat -v\n" );
	printf( "                 -l\n" );
	printf( "                 [ -i (network_interface) ] [ -f (filter_string) ] [ --max-packet-trace-count ] [ -o [ESPDd] ] [ --sql ] [ --http ] [ --log-file (pathfilename) ]\n" );
	printf( "-o E : Output EVENT\n" );
	printf( "   S : Output SESSION\n" );
	printf( "   P : Output PACKET\n" );
	printf( "   D : Output PACKET DATA\n" );
	printf( "   d : Output DEBUG\n" );
	printf( "--sql : Output SQL time elapse\n" );
	printf( "NOTICE : See pcap-filter(7) for the syntax of filter\n" );
	return;
}

#if ( defined __linux ) || ( defined _AIX )

/* �źŵƻص����� */
static void SignalProc( int sig_no )
{
	struct TcplStatEnv	*p_env = g_p_env ;
	
	if( sig_no == SIGTERM || sig_no == SIGINT )
	{
		struct TcplSession	*p_tcpl_session = NULL ;
		struct TcplSession	*p_next_tcpl_session = NULL ;
		struct TcplPacket	*p_tcpl_packet = NULL ;
		struct TcplPacket	*p_next_tcpl_packet = NULL ;
		
		/* ����TCP�Ự�� */
		DestroyTcplSessionTree( p_env );
		
		/* �����й�TCP�Ự���� */
		list_for_each_entry_safe( p_tcpl_session , p_next_tcpl_session , & (p_env->unused_tcpl_session.this_node) , struct TcplSession , this_node )
		{
			list_for_each_entry_safe( p_tcpl_packet , p_next_tcpl_packet , & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node )
			{
				DELETE_TCPL_PACKET( p_env , p_tcpl_packet )
			}
			
			DELETE_TCPL_SESSION( p_env , p_tcpl_session );
		}
		
		/* �����й�TCP�������� */
		list_for_each_entry_safe( p_tcpl_packet , p_next_tcpl_packet , & (p_env->unused_tcpl_packet.this_node) , struct TcplPacket , this_node )
		{
			DELETE_TCPL_PACKET( p_env , p_tcpl_packet )
		}
		
		exit(0);
	}
	else if( sig_no == SIGUSR1 )
	{
		if( p_env->cmd_line_para.log_pathfilename )
		{
			/* �ر���־�ļ� */
			fclose( p_env->fp );
			
			/* ����־�ļ� */
			p_env->fp = fopen( p_env->cmd_line_para.log_pathfilename , "a" ) ;
			if( p_env->fp == NULL )
			{
				exit(1);
			}

#if _TCPLSTAT_DEBUG_FLUSH
			setbuf( p_env->fp , NULL );
#endif
		}
	}
	
	return;
}

#endif

/* ����� */
int main( int argc , char *argv[] )
{
	struct TcplStatEnv	*p_env = NULL ;
	int			i ;
	
	bpf_u_int32		net ;
	bpf_u_int32		net_mask ;
	struct bpf_program	pcap_filter ;
	
#if ( defined __linux ) || ( defined _AIX )
	struct sigaction	act ;
#endif
	
	int			nret = 0 ;
	
	if( argc == 1 )
	{
		usage();
		exit(0);
	}
	
	/* �����ڴ��Դ�Ż����ṹ */
	p_env = (struct TcplStatEnv *)malloc( sizeof(struct TcplStatEnv) ) ;
	if( p_env == NULL )
	{
		printf( "*** ERROR : malloc TcplStatEnv failed , errno[%d]\n" , errno );
		return 1;
	}
	memset( p_env , 0x00 , sizeof(struct TcplStatEnv) );
	g_p_env = p_env ;
	
	INIT_LIST_HEAD( & (p_env->unused_tcpl_session.this_node) );
	INIT_LIST_HEAD( & (p_env->unused_tcpl_packet.this_node) );
	
	/* ���������в��� */
	for( i = 1 ; i < argc ; i++ )
	{
		if( STRCMP( argv[i] , == , "-v" ) )
		{
			version();
			free( p_env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "-i" ) && i + 1 < argc )
		{
			p_env->cmd_line_para.network_interface = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "-l" ) )
		{
			pcap_if_t	*network_interface_list = NULL ;
			pcap_if_t	*network_interface = NULL ;
			
			nret = pcap_findalldevs( & network_interface_list , p_env->pcap_errbuf ) ;
			if( nret == -1 )
			{
				printf( "*** ERROR : pcap_findalldevs failed , errbuf[%s]\n" , p_env->pcap_errbuf );
				free( p_env );
				return 1;
			}
			
			network_interface = network_interface_list ;
			while( network_interface )
			{
				printf( "%s %s\n" , network_interface->name , network_interface->description );
				
				network_interface = network_interface->next ;
			}
			
			pcap_freealldevs( network_interface_list );
			
			free( p_env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "-f" ) && i + 1 < argc )
		{
			p_env->cmd_line_para.filter_string = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--max-packet-trace-count" ) && i + 1 < argc )
		{
			p_env->cmd_line_para.max_packet_trace_count = (UINT32)atoi(argv[i+1]) ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "-o" ) )
		{
			if( strchr( argv[i+1] , 'd' ) )
				p_env->cmd_line_para.output_debug = 1 ;
			if( strchr( argv[i+1] , 'E' ) )
				p_env->cmd_line_para.output_event = 1 ;
			if( strchr( argv[i+1] , 'S' ) )
				p_env->cmd_line_para.output_session = 1 ;
			if( strchr( argv[i+1] , 'P' ) )
				p_env->cmd_line_para.output_session_packet = 1 ;
			if( strchr( argv[i+1] , 'D' ) )
				p_env->cmd_line_para.output_session_packet_data = 1 ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--sql" ) )
		{
			p_env->cmd_line_para.output_sql = 1 ;
		}
		else if( STRCMP( argv[i] , == , "--http" ) )
		{
			p_env->cmd_line_para.output_http = 1 ;
		}
		else if( STRCMP( argv[i] , == , "--log-file" ) && i + 1 < argc )
		{
			p_env->cmd_line_para.log_pathfilename = argv[i+1] ;
			i++;
		}
		else
		{
			printf( "***ERROR : invalid command parameter '%s'\n" , argv[i] );
			usage();
			free( p_env );
			exit(1);
		}
	}
	
	if( p_env->cmd_line_para.network_interface == NULL )
	{
		p_env->cmd_line_para.network_interface = "any" ;
	}
	else
	{
		pcap_if_t	*network_interface_list = NULL ;
		pcap_if_t	*network_interface = NULL ;
		
		nret = pcap_findalldevs( & network_interface_list , p_env->pcap_errbuf ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : pcap_findalldevs failed , errbuf[%s]\n" , p_env->pcap_errbuf );
			free( p_env );
			return 1;
		}
		
		network_interface = network_interface_list ;
		while( network_interface )
		{
			if( STRCMP( network_interface->name , == , p_env->cmd_line_para.network_interface ) )
				break;
			
			network_interface = network_interface->next ;
		}
		
		pcap_freealldevs( network_interface_list );
		
		if( network_interface == NULL )
		{
			printf( "*** ERROR : network interface [%s] not found\n" , p_env->cmd_line_para.network_interface );
			free( p_env );
			return 1;
		}
	}
	
	if( p_env->cmd_line_para.filter_string == NULL )
		p_env->cmd_line_para.filter_string = "" ;
	
	if( p_env->cmd_line_para.max_packet_trace_count <= 0 )
		p_env->cmd_line_para.max_packet_trace_count = TCPLSESSION_MAX_PACKET_TRACE_COUNT ;
	
	/* �õ������豸��Ϣ */
	nret = pcap_lookupnet( p_env->cmd_line_para.network_interface , & net , & net_mask , p_env->pcap_errbuf ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : pcap_lookupnet failed , errbuf[%s]\n" , p_env->pcap_errbuf );
		free( p_env );
		return 1;
	}
	
	/* �������豸 */
	p_env->pcap = pcap_open_live( p_env->cmd_line_para.network_interface , 65535 , 1 , 1000 , p_env->pcap_errbuf ) ;
	if( p_env->pcap == NULL )
	{
		printf( "*** ERROR : pcap_open_live failed , errbuf[%s]\n" , p_env->pcap_errbuf );
		free( p_env );
		return 1;
	}
	
	/* ������̽���� */
	memset( & pcap_filter , 0x00 , sizeof(struct bpf_program) );
	nret = pcap_compile( p_env->pcap , & pcap_filter , p_env->cmd_line_para.filter_string , 0 , net_mask ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : pcap_compile failed , errbuf[%s]\n" , p_env->pcap_errbuf );
		pcap_close( p_env->pcap );
		free( p_env );
		return 1;
	}
	
	/* ������̽���� */
	nret = pcap_setfilter( p_env->pcap , & pcap_filter ) ;
	if( nret == -1 )
	{
		printf( "*** ERROR : pcap_setfilter failed , errbuf[%s]\n" , p_env->pcap_errbuf );
		pcap_close( p_env->pcap );
		free( p_env );
		return 1;
	}
	
	/* ����־�ļ� */
	if( p_env->cmd_line_para.log_pathfilename )
	{
		p_env->fp = fopen( p_env->cmd_line_para.log_pathfilename , "a" ) ;
		if( p_env->fp == NULL )
		{
			printf( "*** ERROR : can't open [%s]\n" , p_env->cmd_line_para.log_pathfilename );
			free( p_env );
			return 1;
		}
	}
	else
	{
		p_env->fp = stdout ;
	}
#if _TCPLSTAT_DEBUG_FLUSH
	setbuf( p_env->fp , NULL );
#endif
	
#if ( defined __linux ) || ( defined _AIX )
	/* �����źŵ� */
	signal( SIGHUP , SIG_IGN );
	signal( SIGPIPE , SIG_IGN );
	
	act.sa_handler = & SignalProc ;
	sigemptyset( & (act.sa_mask) );
	act.sa_flags = SA_RESTART ;
	sigaction( SIGTERM , & act , NULL );
	sigaction( SIGINT , & act , NULL );
	sigaction( SIGUSR1 , & act , NULL );
#endif
	
	/* ������̽��ѭ��������TCP�������ûص����� */
	pcap_loop( p_env->pcap , -1 , PcapCallback , (u_char *)p_env );
	
	/* �ر���־�ļ� */
	if( p_env->cmd_line_para.log_pathfilename )
	{
		fclose( p_env->fp );
	}
	
	/* �ر������豸 */
	pcap_close( p_env->pcap );
	
	/* �ͷŻ����ṹ */
	free( p_env );
	
	return 0;
}

