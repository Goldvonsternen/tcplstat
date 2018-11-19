/*
 * tcplstat - TCP packets monitor and statistical tool
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "tcplstat_in.h"

char	*_g_tcplstat_tcplsession_state[] = { "DISCONNECTED" , "CONNECTING" , "CONNECTED" , "DISCONNECTING" } ;

/* ����TCP���� */
int ProcessTcpPacket( struct TcplStatEnv *p_env , const struct pcap_pkthdr *pcaphdr , struct NetinetEthernetHeader *etherhdr , struct NetinetIpHeader *iphdr , struct NetinetTcpHeader *tcphdr , struct TcplAddrHumanReadable *p_tcpl_addr_hr , char *packet_data_intercepted , UINT32 packet_data_len_intercepted , UINT32 packet_data_len_actually )
{
	struct TcplSession	tcpl_session ;
	struct TcplSession	*p_tcpl_session = NULL ;
	struct TcplPacket	*p_last_tcpl_packet = NULL ;
	
	int			nret = 0 ;
	
	/* ���TCP�������SYN��־ */
	if( TH_FLAG(tcphdr,TH_SYN) )
	{
		/* ��ѯ����ỰID */
		SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_src , tcphdr->_th_sport , iphdr->_ip_dst , tcphdr->_th_dport )
		p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
		if( p_tcpl_session )
		{
			/* �ظ��յ�SYN�� */
			if( p_tcpl_session->status[0] == TCPLSESSION_STATUS_SYN )
			{
				fprintf( p_env->fp , "*** WARN : [%s:%d]->[%s:%d] SYN DUPLICATED\n" , p_tcpl_addr_hr->src_ip , p_tcpl_addr_hr->src_port , p_tcpl_addr_hr->dst_ip , p_tcpl_addr_hr->dst_port );
				return 0;
			}
			/* δ���յ�SYN����Ī��������ĻỰ�������յ�SYN�� */
			else
			{
				fprintf( p_env->fp , "*** ERROR : [%s:%d]->[%s:%d] status invalid\n" , p_tcpl_addr_hr->src_ip , p_tcpl_addr_hr->src_port , p_tcpl_addr_hr->dst_ip , p_tcpl_addr_hr->dst_port );
				UnlinkTcplSessionTreeNode( p_env , p_tcpl_session );
				if( p_env->unused_tcpl_session_count < PENV_MAX_UNUSED_TCPLSESSION_COUNT )
				{
					RECYCLING_TCPL_SESSION( p_env , p_tcpl_session );
				}
				else
				{
					DELETE_TCPL_SESSION( p_env , p_tcpl_session );
				}
				return 0;
			}
		}
		else
		{
			/* ��ѯ����ỰID */
			SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_dst , tcphdr->_th_dport , iphdr->_ip_src , tcphdr->_th_sport )
			p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
			if( p_tcpl_session )
			{
				/* �ظ��յ�SYN�� */
				if( p_tcpl_session->status[1] == TCPLSESSION_STATUS_SYN )
				{
					fprintf( p_env->fp , "*** WARN : [%s:%d]->[%s:%d] REVERSE SYN DUPLICATED\n" , p_tcpl_addr_hr->src_ip , p_tcpl_addr_hr->src_port , p_tcpl_addr_hr->dst_ip , p_tcpl_addr_hr->dst_port );
					return 0;
				}
				
				/* ͳ�Ʒ���SYN���յ�SYN+ACK���ӳ� */
				p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
				COPY_TIMEVAL( p_tcpl_session->wait_for_second_syn_and_first_ack_elapse , p_env->fixed_timestamp );
				DIFF_TIMEVAL( p_tcpl_session->wait_for_second_syn_and_first_ack_elapse , p_last_tcpl_packet->timestamp )
				
				p_tcpl_session->status[1] = TCPLSESSION_STATUS_SYN ;
				
				/* ��¼TCP������ϸ */
				nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_OPPO_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
				if( nret )
					return nret;
				
				if( p_env->cmd_line_para.output_debug )
					OUTPUT_SESSION_EVENT( "MODIFY" , TCPLPACKET_OPPO_DIRECTION , p_tcpl_session )
				
				return 0;
			}
			else
			{
				/* δ��ѯ���Ự���յ���һ��SYN�������������Ự */
				if( p_env->unused_tcpl_session_count == 0 )
				{
					p_tcpl_session = (struct TcplSession *)malloc( sizeof(struct TcplSession) ) ;
					if( p_tcpl_session == NULL )
					{
						fprintf( p_env->fp , "*** ERROR : alloc failed , errno[%d]\n" , errno );
						exit(1);
					}
					memset( p_tcpl_session , 0x00 , sizeof(struct TcplSession) );
				}
				else
				{
					REUSE_TCPL_SESSION( p_env , p_tcpl_session )
				}
				
				SET_TCPL_SESSION_ID( p_tcpl_session->tcpl_session_id , iphdr->_ip_src , tcphdr->_th_sport , iphdr->_ip_dst , tcphdr->_th_dport )
				memcpy( & (p_tcpl_session->tcpl_addr_hr) , p_tcpl_addr_hr , sizeof(struct TcplAddrHumanReadable) );
				COPY_TIMEVAL( p_tcpl_session->begin_timestamp , p_env->fixed_timestamp )
				p_tcpl_session->state = TCPLSESSION_STATE_CONNECTING ;
				p_tcpl_session->status[0] = TCPLSESSION_STATUS_SYN ;
				INIT_LIST_HEAD( & (p_tcpl_session->tcpl_packets_trace_list.this_node) );
				
				if( p_env->cmd_line_para.output_debug )
					OUTPUT_SESSION_EVENT( "ADD" , TCPLPACKET_DIRECTION , p_tcpl_session )
				
				/* �½��Ự���ҵ��Ự���� */
				nret = LinkTcplSessionTreeNode( p_env , p_tcpl_session ) ;
				if( nret )
					return nret;
				
				/* ��¼TCP������ϸ */
				nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
				if( nret )
					return nret;
				
				return 0;
			}
		}
	}
	
	/* ���TCP�������FIN��־ */
	if( TH_FLAG(tcphdr,TH_FIN) )
	{
		/* ��ѯ����ỰID */
		SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_src , tcphdr->_th_sport , iphdr->_ip_dst , tcphdr->_th_dport )
		p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
		if( p_tcpl_session )
		{
			/* �ظ��յ�FIN�� */
			if( p_tcpl_session->status[0] == TCPLSESSION_STATUS_FIN )
			{
				return 0;
			}
			
			p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
			if( p_tcpl_session->status[1] == TCPLSESSION_STATUS_SYN )
			{
				/* �����ȷ���FIN�� */
				COPY_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_env->fixed_timestamp );
				DIFF_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_last_tcpl_packet->timestamp )
			}
			else
			{
				/* �����ȷ���FIN�� */
				COPY_TIMEVAL( p_tcpl_session->wait_for_second_fin_and_first_ack_elapse , p_env->fixed_timestamp );
				DIFF_TIMEVAL( p_tcpl_session->wait_for_second_fin_and_first_ack_elapse , p_last_tcpl_packet->timestamp )
			}
			
			p_tcpl_session->status[0] = TCPLSESSION_STATUS_FIN ;
			if( p_tcpl_session->state == TCPLSESSION_STATE_CONNECTED )
			{
				p_tcpl_session->state = TCPLSESSION_STATE_DISCONNECTING ;
				if( p_tcpl_session->disconnect_direction == TCPLSESSION_DISCONNECT_WAITFOR )
					p_tcpl_session->disconnect_direction = TCPLSESSION_DISCONNECT_DIRECTION ;
			}
			
			/* ��¼TCP������ϸ */
			nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
			if( nret )
				return nret;
			
			if( p_env->cmd_line_para.output_debug )
				OUTPUT_SESSION_EVENT( "MODIFY" , TCPLPACKET_DIRECTION , p_tcpl_session )
			
			return 0;
		}
		else
		{
			/* ��ѯ����ỰID */
			SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_dst , tcphdr->_th_dport , iphdr->_ip_src , tcphdr->_th_sport )
			p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
			if( p_tcpl_session )
			{
				if( p_tcpl_session->status[1] == TCPLSESSION_STATUS_FIN )
				{
					return 0;
				}
				
				p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
				if( p_tcpl_session->status[0] == TCPLSESSION_STATUS_SYN )
				{
					/* �����ȷ���FIN�� */
					COPY_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_env->fixed_timestamp );
					DIFF_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_last_tcpl_packet->timestamp )
				}
				else
				{
					/* �����ȷ���FIN�� */
					COPY_TIMEVAL( p_tcpl_session->wait_for_second_fin_and_first_ack_elapse , p_env->fixed_timestamp );
					DIFF_TIMEVAL( p_tcpl_session->wait_for_second_fin_and_first_ack_elapse , p_last_tcpl_packet->timestamp )
				}
				
				p_tcpl_session->status[1] = TCPLSESSION_STATUS_FIN ;
				if( p_tcpl_session->state == TCPLSESSION_STATE_CONNECTED )
				{
					p_tcpl_session->state = TCPLSESSION_STATE_DISCONNECTING ;
					if( p_tcpl_session->disconnect_direction == TCPLSESSION_DISCONNECT_WAITFOR )
						p_tcpl_session->disconnect_direction = TCPLSESSION_DISCONNECT_OPPO_DIRECTION ;
				}
				
				/* ��¼TCP������ϸ */
				nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_OPPO_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
				if( nret )
					return nret;
				
				if( p_env->cmd_line_para.output_debug )
					OUTPUT_SESSION_EVENT( "MODIFY" , TCPLPACKET_OPPO_DIRECTION , p_tcpl_session )
				
				return 0;
			}
		}
	}
	
	/* ���TCP�������RST��־ */
	if( TH_FLAG(tcphdr,TH_RST) )
	{
		/* ��ѯ����ỰID */
		SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_src , tcphdr->_th_sport , iphdr->_ip_dst , tcphdr->_th_dport )
		p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
		if( p_tcpl_session )
		{
			/* ͳ�Ƶ�һ��FIN���ӳ� */
			p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
			COPY_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_env->fixed_timestamp );
			DIFF_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_last_tcpl_packet->timestamp )
			
			p_tcpl_session->state = TCPLSESSION_STATE_DISCONNECTED ;
			
			/* ��¼TCP������ϸ */
			nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
			if( nret )
				return nret;
			
			if( p_env->cmd_line_para.output_debug )
				OUTPUT_SESSION_EVENT( "REMOVE" , TCPLPACKET_DIRECTION , p_tcpl_session )
			
			/* ���TCP�Ự��Ϣ */
			OutputTcplSession( p_env , pcaphdr , p_tcpl_session , 1 );
			
			/* ��TCP�Ự����ɾ�� */
			UnlinkTcplSessionTreeNode( p_env , p_tcpl_session );
			if( p_env->unused_tcpl_session_count < PENV_MAX_UNUSED_TCPLSESSION_COUNT )
			{
				RECYCLING_TCPL_SESSION( p_env , p_tcpl_session );
			}
			else
			{
				DELETE_TCPL_SESSION( p_env , p_tcpl_session );
			}
			
			return 0;
		}
		else
		{
			/* ��ѯ����ỰID */
			SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_dst , tcphdr->_th_dport , iphdr->_ip_src , tcphdr->_th_sport )
			p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
			if( p_tcpl_session )
			{
				/* ͳ�Ƶ�һ��FIN���ӳ� */
				p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
				COPY_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_env->fixed_timestamp );
				DIFF_TIMEVAL( p_tcpl_session->wait_for_first_fin_elapse , p_last_tcpl_packet->timestamp )
				
				p_tcpl_session->state = TCPLSESSION_STATE_DISCONNECTED ;
				
				/* ��¼TCP������ϸ */
				nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_OPPO_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
				if( nret )
					return nret;
				
				if( p_env->cmd_line_para.output_debug )
					OUTPUT_SESSION_EVENT( "REMOVE" , TCPLPACKET_OPPO_DIRECTION , p_tcpl_session )
				
				/* ���TCP�Ự��Ϣ */
				OutputTcplSession( p_env , pcaphdr , p_tcpl_session , 1 );
				
				/* ��TCP�Ự����ɾ�� */
				UnlinkTcplSessionTreeNode( p_env , p_tcpl_session );
				if( p_env->unused_tcpl_session_count < PENV_MAX_UNUSED_TCPLSESSION_COUNT )
				{
					RECYCLING_TCPL_SESSION( p_env , p_tcpl_session );
				}
				else
				{
					DELETE_TCPL_SESSION( p_env , p_tcpl_session );
				}
				
				return 0;
			}
		}
	}
	
	/* ���TCP�������ACK��־��������־ */
	/* ��ѯ����ỰID */
	SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_src , tcphdr->_th_sport , iphdr->_ip_dst , tcphdr->_th_dport )
	p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
	if( p_tcpl_session )
	{
		if( p_tcpl_session->status[0] == TCPLSESSION_STATUS_SYN && p_tcpl_session->status[1] == TCPLSESSION_STATUS_SYN )
		{
			/* �յ��������ֵ����һ��ACK ���� ����TCP�������� */
			
			if( TH_FLAG(tcphdr,TH_ACK) && p_tcpl_session->state == TCPLSESSION_STATE_CONNECTING )
			{
				p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
				COPY_TIMEVAL( p_tcpl_session->wait_for_after_syn_and_second_ack_elapse , p_env->fixed_timestamp );
				DIFF_TIMEVAL( p_tcpl_session->wait_for_after_syn_and_second_ack_elapse , p_last_tcpl_packet->timestamp )
			}
			
			/* ��¼TCP������ϸ */
			nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
			if( nret )
				return nret;
			
			if( TH_FLAG(tcphdr,TH_ACK) && p_tcpl_session->state == TCPLSESSION_STATE_CONNECTING )
			{
				p_tcpl_session->state = TCPLSESSION_STATE_CONNECTED ;
			}
			
			if( p_env->cmd_line_para.output_debug )
				OUTPUT_SESSION_EVENT( "MODIFY" , TCPLPACKET_DIRECTION , p_tcpl_session )
			
			if( p_tcpl_session->total_packet_trace_count >= p_env->cmd_line_para.max_packet_trace_count )
			{
				/* ���TCP�Ự��Ϣ */
				OutputTcplSession( p_env , pcaphdr , p_tcpl_session , 0 );
			}
			
			return 0;
		}
		else if( p_tcpl_session->status[0] == TCPLSESSION_STATUS_FIN && p_tcpl_session->status[1] == TCPLSESSION_STATUS_FIN )
		{
			/* �յ��Ĳ����ֵ����һ��ACK */
			if( TH_FLAG(tcphdr,TH_ACK) && p_tcpl_session->state == TCPLSESSION_STATE_DISCONNECTING && p_tcpl_session->disconnect_direction == TCPLSESSION_DISCONNECT_DIRECTION )
			{
				p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
				COPY_TIMEVAL( p_tcpl_session->wait_for_second_ack_elapse , p_env->fixed_timestamp );
				DIFF_TIMEVAL( p_tcpl_session->wait_for_second_ack_elapse , p_last_tcpl_packet->timestamp )
				
				/* ��¼TCP������ϸ */
				nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
				if( nret )
					return nret;
				
				p_tcpl_session->state = TCPLSESSION_STATE_DISCONNECTED ;
				
				if( p_env->cmd_line_para.output_debug )
					OUTPUT_SESSION_EVENT( "REMOVE" , TCPLPACKET_DIRECTION , p_tcpl_session )
				
				/* ���TCP�Ự��Ϣ */
				OutputTcplSession( p_env , pcaphdr , p_tcpl_session , 1 );
				
				/* ��TCP�Ự����ɾ�� */
				UnlinkTcplSessionTreeNode( p_env , p_tcpl_session );
				if( p_env->unused_tcpl_session_count < PENV_MAX_UNUSED_TCPLSESSION_COUNT )
				{
					RECYCLING_TCPL_SESSION( p_env , p_tcpl_session );
				}
				else
				{
					DELETE_TCPL_SESSION( p_env , p_tcpl_session );
				}
				
				return 0;
			}
		}
	}
	else
	{
		/* ��ѯ����ỰID */
		SET_TCPL_SESSION_ID( tcpl_session.tcpl_session_id , iphdr->_ip_dst , tcphdr->_th_dport , iphdr->_ip_src , tcphdr->_th_sport )
		p_tcpl_session = QueryTcplSessionTreeNode( p_env , & tcpl_session ) ;
		if( p_tcpl_session )
		{
			/* ����TCP�������� */
			if( p_tcpl_session->status[1] == TCPLSESSION_STATUS_SYN && p_tcpl_session->status[0] == TCPLSESSION_STATUS_SYN )
			{
				nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_OPPO_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
				if( nret )
					return nret;
				
				if( p_env->cmd_line_para.output_debug )
					OUTPUT_SESSION_EVENT( "MODIFY" , TCPLPACKET_OPPO_DIRECTION , p_tcpl_session )
				
				if( p_tcpl_session->total_packet_trace_count >= p_env->cmd_line_para.max_packet_trace_count )
				{
					/* ���TCP�Ự��Ϣ */
					OutputTcplSession( p_env , pcaphdr , p_tcpl_session , 0 );
				}
				
				return 0;
			}
			else if( p_tcpl_session->status[1] == TCPLSESSION_STATUS_FIN && p_tcpl_session->status[0] == TCPLSESSION_STATUS_FIN )
			{
				/* �յ��Ĳ����ֵ����һ��ACK */
				if( TH_FLAG(tcphdr,TH_ACK) && p_tcpl_session->state == TCPLSESSION_STATE_DISCONNECTING && p_tcpl_session->disconnect_direction == TCPLSESSION_DISCONNECT_OPPO_DIRECTION )
				{
					p_last_tcpl_packet = list_last_entry( & (p_tcpl_session->tcpl_packets_trace_list.this_node) , struct TcplPacket , this_node ) ;
					COPY_TIMEVAL( p_tcpl_session->wait_for_second_ack_elapse , p_env->fixed_timestamp );
					DIFF_TIMEVAL( p_tcpl_session->wait_for_second_ack_elapse , p_last_tcpl_packet->timestamp )
					
					/* ��¼TCP������ϸ */
					nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_OPPO_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
					if( nret )
						return nret;
					
					p_tcpl_session->state = TCPLSESSION_STATE_DISCONNECTED ;
					
					if( p_env->cmd_line_para.output_debug )
						OUTPUT_SESSION_EVENT( "REMOVE" , TCPLPACKET_OPPO_DIRECTION , p_tcpl_session )
					
					/* ���TCP�Ự��Ϣ */
					OutputTcplSession( p_env , pcaphdr , p_tcpl_session , 1 );
					
					/* ��TCP�Ự����ɾ�� */
					UnlinkTcplSessionTreeNode( p_env , p_tcpl_session );
					if( p_env->unused_tcpl_session_count < PENV_MAX_UNUSED_TCPLSESSION_COUNT )
					{
						RECYCLING_TCPL_SESSION( p_env , p_tcpl_session );
					}
					else
					{
						DELETE_TCPL_SESSION( p_env , p_tcpl_session );
					}
					
					return 0;
				}
			}
		}
		else
		{
			/* �����п�ʼ��̽���͵��������Ự */
			
			p_tcpl_session = (struct TcplSession *)malloc( sizeof(struct TcplSession) ) ;
			if( p_tcpl_session == NULL )
			{
				fprintf( p_env->fp , "*** ERROR : alloc failed , errno[%d]\n" , errno );
				exit(1);
			}
			memset( p_tcpl_session , 0x00 , sizeof(struct TcplSession) );
			
			SET_TCPL_SESSION_ID( p_tcpl_session->tcpl_session_id , iphdr->_ip_src , tcphdr->_th_sport , iphdr->_ip_dst , tcphdr->_th_dport )
			memcpy( & (p_tcpl_session->tcpl_addr_hr) , p_tcpl_addr_hr , sizeof(struct TcplAddrHumanReadable) );
			COPY_TIMEVAL( p_tcpl_session->begin_timestamp , p_env->fixed_timestamp )
			p_tcpl_session->state = TCPLSESSION_STATE_CONNECTED ;
			p_tcpl_session->status[0] = TCPLSESSION_STATUS_SYN ;
			p_tcpl_session->status[1] = TCPLSESSION_STATUS_SYN ;
			INIT_LIST_HEAD( & (p_tcpl_session->tcpl_packets_trace_list.this_node) );
			
			if( p_env->cmd_line_para.output_debug )
				OUTPUT_SESSION_EVENT( "ADD" , TCPLPACKET_DIRECTION , p_tcpl_session )
			
			/* �����Ự���ҵ��Ự���� */
			nret = LinkTcplSessionTreeNode( p_env , p_tcpl_session ) ;
			if( nret )
				return nret;
			
			/* ��¼TCP������ϸ */
			nret = AddTcpPacket( p_env , p_tcpl_session , TCPLPACKET_DIRECTION , tcphdr , packet_data_intercepted , packet_data_len_intercepted , packet_data_len_actually ) ;
			if( nret )
				return nret;
			
			return 0;
		}
	}
	
	return 0;
}

