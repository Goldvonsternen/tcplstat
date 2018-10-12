tcplstat - TCP�����ع���
==========================

<!-- TOC -->

- [1. ����](#1-����)
- [2. ��װ](#2-��װ)
    - [2.1. Դ����밲װ��Linux����ϵͳ��](#21-Դ����밲װlinux����ϵͳ)
    - [2.2. Դ����밲װ��WINDOWS����ϵͳ��VS2008����������](#22-Դ����밲װwindows����ϵͳvs2008��������)
- [3. ʹ��](#3-ʹ��)
    - [3.1. �����в���˵��](#31-�����в���˵��)
    - [3.2. һ��ʾ������ʱ���TCP�����¼���](#32-һ��ʾ����ʱ���tcp�����¼�)
    - [3.3. ��һ��ʾ���������ӶϿ������ͳ����Ϣ��](#33-��һ��ʾ�������ӶϿ������ͳ����Ϣ)
    - [3.4. ��һ��ʾ�����ɼ�ͳ��SQL��ʱ��](#34-��һ��ʾ���ɼ�ͳ��sql��ʱ)
- [4. ���](#4-���)

<!-- /TOC -->

# 1. ����

ֻ��Ϊ�����������Ҳ�С�Ŀ���һ��libpcap���ϣ�����������������һ����ĩ��û���ã������ĸ����Ϸ������飬�������ܶ�������ڵ��������TCP�����ع��ߡ�

tcplstat�ǻ���libpcap������̽������������ع��ߣ�����**��·**�������о��������豸���˹����TCP���ݣ����ٵ�ǰ����TCP���ӻỰ����¼���о�����TCP���飬�����ӶϿ��򵽴�����¼��ʱ����ͳ����Ϣ���������������������������ַ������ʱ������������ָ������ӳ١��Ĳ����ָ������ӳ٣����ݷ�����ϸ������������ӳٺ��෴��������ӳٵ���С��ƽ�������ͳ��ֵ��

tcplstat��**��·**���������Բ����Ӧ������κ�Ӱ�죬Ҳ��������Ӧ�ã����ɻ��������������������ϸ��ͳ����Ϣ��

tcplstat��ʵ�ֻ��������ع���ʱ��ʵ���˲ɼ�����SQL��ʱ��HTTP��ʱ��Ϣ��ͬ��Ҳ��**��·**���񣬲�Ӱ��Ӧ��Ҳ�������Ӧ�ã�����Ӧ���Ż����ܡ�

tcplstat�ǿ�Դ�ģ�����������Linux�ں˵ĺ����������Դ���⣬����Դ��ֻ��1500�����ң�Դ��ṹ���׶���

tcplstat������ƽ̨Linux��WINDOWS��AIX��

# 2. ��װ

������tcplstat���԰�װ���κ���libpcap�Ļ���������Linux��WINDOWS��AIX�ȡ�

Linux/AIX�ϱ��밲װtcplstatǰ���Ȱ�װlibpcap-devel��ֻ����tcplstatǰ���Ȱ�װlibpcap��

WINDOWS�ϱ��밲װtcplstatǰ���Ȱ�װwinpcap��������ֻ����tcplstatǰ���Ȱ�װwinpcap���а���

**ע�⣺winpcap������̽�ػ��ӿ�������������ط��ʱ��أ����������̽�ػ��ӿ������밲װnpcap�滻winpcap��**

## 2.1. Դ����밲װ��Linux����ϵͳ��

��tcplstatԴ���й�վ�㣨��ַ�������������Դ������⿪������Դ��Ŀ¼

```
$ tar xvzf tcplstat.tar.gz
...
�� cd tcplstat/src
```

�����޸İ�װĿ¼

```
$ vi makeinstall
_BINBASE        =       $(HOME)/bin
```

**ע�⣺���뻷����Ҫ������libpcap-devel����Ԥ�Ȱ�װ�á�**

���롢��װtcplstat

```
$ make -f makefile.Linux
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c list.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c rbtree.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c rbtree_ins.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c Util.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c main.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c PcapCallback.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c ProcessTcpPacket.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c AddTcpPacket.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -I/home/calvin/include -I. -I/home/calvin/include  -c OutputTcplSession.c
gcc -g -fPIC -O2 -Wall -Werror -fno-strict-aliasing -o tcplstat list.o rbtree.o rbtree_ins.o Util.o main.o PcapCallback.o ProcessTcpPacket.o AddTcpPacket.o OutputTcplSession.o -L/home/calvin/lib -L. -L/home/calvin/lib -lpcap 
$ make -f makefile.Linux install
cp -rf tcplstat /home/calvin/bin/
```

��������ֻ������һ����ִ�г���`tcplstat`��Ҳ�����и��Ƶ�Ŀ��Ŀ¼��

��ʾ�汾��Ϣ

```
$ tcplstat -v
tcplstat v0.10.0 build Feb 28 2018 23:14:00
copyright by calvin<calvinwilliams@163.com> 2018
```

## 2.2. Դ����밲װ��WINDOWS����ϵͳ��VS2008����������

��VS2008��src\vs2008\vs2008.sln������winpcapͷ�ļ��Ϳ��ļ�·����������ִ�г��򼴿ɡ�

**ע�⣺Դ���bin\tcplstat.exe������WIN10���������ӳ����Ŀ�ִ�г��򣬿�ֱ��ִ��ʹ�á�**

# 3. ʹ��

## 3.1. �����в���˵��

���������в���ִ����ʾ���������в���

```
$ tcplstat
USAGE : tcplstat -v
                 -l
                 [ -i (network_interface) ] [ -f (filter_string) ] [ -o [ESPDd] ] [ --sql ] [ --http ] [ --max-packet-trace-count ] [ --log-file (pathfilename) ]
-o E : Output EVENT
   S : Output SESSION
   P : Output PACKET
   D : Output PACKET DATA
   d : Output DEBUG
--sql : Output SQL time elapse
NOTICE : See pcap-filter(7) for the syntax of filter
```

* `-v` ��ʾ�汾��Ϣ
* `-l` �г����������豸�����ݸ�`-i`����������WINDOWS�������豸�����ò�
* `-i` ���������豸�ӿڣ���������Ĭ��ʹ��`any`
* `-f` ����������˹��򣬱���`tcp port 445`��̽�������ӵ��˿�445������TCP���飬����μ�`pcap-filter(7)`
* `-o` һ������TCP���飬����������ͣ�E��ʾ��������¼���S��ʾ���ӶϿ�����Ựͳ����Ϣ��P��ʾ���ӶϿ����TCP����ͳ����Ϣ��D��ʾ���ӶϿ����TCP����������Ϣ��d��ʾ���������Ϣ
* `--sql` ����SQLͳ�ƺ�ʱ��Ϣ
* `--http` ����HTTPͳ�ƺ�ʱ��Ϣ
* `--max-packet-trace-count` ��Գ����Ӳ��ͷţ����ǲ�����⵽���ӶϿ�Ҳ�Ͳ�������Ự�ܽ���Ϣ����ѡ������TCP�����ۻ�������ʱǿ����������TCP������ϸ��Ϣ��Ĭ��Ϊ1000���´�����Ựͳ����Ϣʱǰ׺��'E |'���'E -'
* `--log-file` �������־�ļ����������ļ����������Ļ

**ע�⣺ִ��tcplstat��ҪrootȨ�ޡ�**

## 3.2. һ��ʾ������ʱ���TCP�����¼���

��һ������tcplstat

```
# tcplstat -f "tcp port 445" -o E
```

�ڶ�����445�˿ڷ���һ���ַ�����Ȼ��samba����������ǿ�жϿ�

```
$ echo "hello" | nc 114.215.179.129 445
```

��һ�����

```
E | 2018-02-07T20:09:21.249992 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[44200] DPORT[445] SEQ[3750544418] ACKSEQ[0] SYN[1] ACK[0] FIN[0] PSH[0] RST[0] URG[0] | [0]BYTES
E | 2018-02-07T20:09:21.250004 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[445] DPORT[44200] SEQ[974809372] ACKSEQ[3767321634] SYN[1] ACK[1] FIN[0] PSH[0] RST[0] URG[0] | [0]BYTES
E | 2018-02-07T20:09:21.250018 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[44200] DPORT[445] SEQ[3767321634] ACKSEQ[991586588] SYN[0] ACK[1] FIN[0] PSH[0] RST[0] URG[0] | [0]BYTES
E | 2018-02-07T20:09:21.251501 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[44200] DPORT[445] SEQ[3767321634] ACKSEQ[991586588] SYN[0] ACK[1] FIN[0] PSH[1] RST[0] URG[0] | [6]BYTES
E |                  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F    0123456789ABCDEF
E |     0x00000000   68 65 6C 6C 6F 0A                                 hello.          
E | 2018-02-07T20:09:21.251507 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[445] DPORT[44200] SEQ[991586588] ACKSEQ[3867984930] SYN[0] ACK[1] FIN[0] PSH[0] RST[0] URG[0] | [0]BYTES
E | 2018-02-07T20:09:21.251661 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[44200] DPORT[445] SEQ[3867984930] ACKSEQ[991586588] SYN[0] ACK[1] FIN[1] PSH[0] RST[0] URG[0] | [0]BYTES
E | 2018-02-07T20:09:21.256246 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[445] DPORT[44200] SEQ[991586588] ACKSEQ[3884762146] SYN[0] ACK[1] FIN[1] PSH[0] RST[0] URG[0] | [0]BYTES
E | 2018-02-07T20:09:21.256267 | LHT[113] | SMAC[] DMAC[] | SIP[114.215.179.129] DIP[114.215.179.129] | SPORT[44200] DPORT[445] SEQ[3884762146] ACKSEQ[1008363804] SYN[0] ACK[1] FIN[0] PSH[0] RST[0] URG[0] | [0]BYTES
...
```

E��ͷ����Ϊһ��TCP���飬���зֱ��Ƿ���ʱ�������·��ͷ�ṹ�����ͷ�MAC�����շ�MAC�����ͷ�IP�����շ�IP�����ͷ�PORT�����շ�PORT�����кš��������кš��������ͱ�־���ϡ���Ч�������ݳ��ȡ�

## 3.3. ��һ��ʾ���������ӶϿ������ͳ����Ϣ��

��һ������tcplstat

```
# tcplstat -f "tcp port 445" -o SPD
```

�ڶ�����445�˿ڷ���һ���ַ�����Ȼ��samba����������ǿ�жϿ�

```
$ echo "hello" | nc 114.215.179.129 445
```

��һ�����

```
S | [114.215.179.129:44205]->[114.215.179.129:445] | 2018-02-07T20:20:41.903338 | 0.016535 | 0.000016 0.000015 , 0.000006 0.000741 0.001477 0.000006 0.000749 0.001492 , 0.000036 0.014964 0.000021 | 2 6
P |     2018-02-07T20:20:41.903338 | 0.000000 0.000000 | [114.215.179.129:44205]->[114.215.179.129:445] | S..... 0
P |     2018-02-07T20:20:41.903354 | 0.000016 0.000016 | [114.215.179.129:44205]<-[114.215.179.129:445] | S..A.. 0
P |     2018-02-07T20:20:41.903369 | 0.000015 0.000015 | [114.215.179.129:44205]->[114.215.179.129:445] | ...A.. 0
P |     2018-02-07T20:20:41.904846 | 0.001477 0.001492 | [114.215.179.129:44205]->[114.215.179.129:445] | ..PA.. 6
D |                  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F    0123456789ABCDEF
D |     0x00000000   68 65 6C 6C 6F 0A                                 hello.          
P |     2018-02-07T20:20:41.904852 | 0.000006 0.000006 | [114.215.179.129:44205]<-[114.215.179.129:445] | ...A.. 0
P |     2018-02-07T20:20:41.904888 | 0.000036 0.000036 | [114.215.179.129:44205]->[114.215.179.129:445] | .F.A.. 0
P |     2018-02-07T20:20:41.919852 | 0.014964 0.014964 | [114.215.179.129:44205]<-[114.215.179.129:445] | .F.A.. 0
P |     2018-02-07T20:20:41.919873 | 0.000021 0.000021 | [114.215.179.129:44205]->[114.215.179.129:445] | ...A.. 0
...
```

S��ͷ����Ϊһ������ͳ����Ϣ�����зֱ������ӷ���ַ�������ӷ���ַ����������ʱ����������ܴ���ʱ�䡢�������ָ������ӳ١��Ĳ����ָ������ӳ١�����������ӳٺ��෴��������ӳٵ���С��ƽ�������ͳ��ֵ���ܷ���������Ч�غ������ܴ�С��

P��ͷ����Ϊһ�������е�һ��TCP����ͳ����Ϣ�����зֱ��Ƿ��鷢��ʱ���������������ӳٺ��෴��������ӳ١����ӷ���ַ�����鷢�ͷ��򡢱����ӷ���ַ���������ͱ�־���ϡ���Ч�������ݳ��ȡ�

���Կ������Լ�����ʲô�������ݣ�����������в���`-o`�������ĸ���ϼ��ɣ�����ϸ����Ϣ���������`ESPDd`

## 3.4. ��һ��ʾ�����ɼ�ͳ��SQL��ʱ��

����SQL��ԭ��ܼ򵥣����ÿһ��TCP�������Ƿ����SQL��䣬�������������ǣ��ȴ���һ����Ч�غɵķ���TCP���鵽���󣬼���ʱ����SQLִ��ʱ�䡣

������PostgreSQLΪ����MySQL��Oracle��ͬ����Ч��

��һ������tcplstat

```
# tcplstat -f "tcp port 8432" --sql
```

�ڶ�����psql�����ݿ����ӣ���ѯ���б��ܼ�¼��

```
calvin=# \d
                               �����б�
 �ܹ�ģʽ |                   ����                   |  �ͱ�  | ӵ���� 
----------+------------------------------------------+--------+--------
 public   | alphastock_company_info                  | ���ϱ� | calvin
 public   | alphastock_company_ipo                   | ���ϱ� | calvin
 public   | alphastock_stock_code                    | ���ϱ� | calvin
 public   | alphastock_stock_kline                   | ���ϱ� | calvin
 public   | alphastock_stock_kline_max_closing_price | ���ϱ� | calvin
 public   | financing_chinawealth                    | ���ϱ� | calvin
 public   | whoispider_domain                        | ���ϱ� | calvin
(7 �м�¼)

calvin=# select count(*) from alphastock_company_info;
 count 
-------
  3596
(1 �м�¼)

calvin=# select count(*) from alphastock_company_ipo ;
 count 
-------
  3596
(1 �м�¼)

calvin=# select count(*) from alphastock_stock_code ;
 count 
-------
  3596
(1 �м�¼)

calvin=# select count(*) from alphastock_stock_kline ;
  count  
---------
 8826375
(1 �м�¼)

calvin=# select count(*) from financing_chinawealth ;
calvin-# ;
 count  
--------
 168148
(1 �м�¼)
```

��һ�����

```
Q | 2018-02-07T20:28:07.978745 0.000869 | select count(*) from alphastock_company_info;
Q | 2018-02-07T20:28:10.427744 0.000605 | select count(*) from alphastock_company_ipo ;
Q | 2018-02-07T20:28:12.923744 0.000737 | select count(*) from alphastock_stock_code ;
Q | 2018-02-07T20:28:15.291747 42.884759 | select count(*) from alphastock_stock_kline ;
Q | 2018-02-07T20:29:35.218747 3.505407 | select count(*) from financing_chinawealth ;
...
```

Q��ͷ����Ϊһ��SQL��ʱͳ�ƣ������ǿ�ʼִ��ʱ�����ִ�к�ʱ��SQL��䡣

���Կ�����`alphastock_stock_kline`�ܴ�SQL`select count(*) from alphastock_stock_kline`����42�룬��`alphastock_company_ipo`��С��SQL`select count(*) from alphastock_company_ipo`����0.6���롣

�����ɼ�ͳ�ƹ�����ȫ��**��·**��ʽ���У���Ӱ��Ӧ��Ҳ��������Ӧ�á�

# 4. ���

��ӭʹ��tcplstat�������ʹ��������������������ң�лл ^_^

Դ���йܵ�ַ : [��Դ�й�](https://gitee.com/calvinwilliams/tcplstat)��[github](https://github.com/calvinwilliams/tcplstat)

�������� : [����](mailto:calvinwilliams@163.com)��[Gmail](mailto:calvinwilliams.c@gmail.com)
