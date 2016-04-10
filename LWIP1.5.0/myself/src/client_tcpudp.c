/**
  ******************************************************************************
  * @file    client.c
  * @author  MCD Application Team
  * @version wangjunwei
  * @date    20150717
  * @brief   A sample TCP client  Test
 */

#include "main.h"
#include <string.h>
#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/tcp_impl.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"

#define TCP_PORT      40096
//5s 一次TCP连接	 (250ms*4)*5
#define TCP_CREATTRM_INTERVAL  (250*4)*5
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct tcp_pcb *TcpPCB;

/* Private function prototypes -----------------------------------------------*/
void udp_client_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, u16_t port);
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
void tcp_client_err(void *arg, err_t err);
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);



  u8 Udpbuf[10];
void udp_client_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, u16_t port)
{
	  	    struct pbuf *ps;
		
	//接收固定长度的广播到数据并转发广播出去 test data len >10

	 if(p->tot_len<10){
	    memcpy((u8*)&Udpbuf[0],(u8*)p->payload,p->tot_len);
		ps = pbuf_alloc(PBUF_TRANSPORT, p->tot_len, PBUF_RAM);
		memcpy((u8*)ps->payload,(u8*)&Udpbuf[0],p->tot_len);
		udp_sendto(upcb, ps,IP_ADDR_BROADCAST, 21223);
		     pbuf_free(ps);
	  				  }

	   pbuf_free(p);
	  
}
struct udp_pcb *upcb;
void udp_client_callback_app(void)
{
  
  struct pbuf *p;
                           
   /* Create a new UDP control block  */
   upcb = udp_new();   
   upcb->so_options |= SOF_BROADCAST;
 
   /* Bind the upcb to any IP address and the UDP_PORT port*/
   udp_bind(upcb, IP_ADDR_ANY, 2);  
   /* Set a receive callback for the upcb */
   udp_recv(upcb, udp_client_callback, NULL);

}



/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the client application.
  * @param  None
  * @retval None
  */
 void  tcp_client_callback(void);
	char link=1;
   struct tcp_pcb *wpcb; 
 uint32_t retry_TCP_connect=0;
  uint32_t SystemRunTime=0 ;
  err_t  tcp_client_reciver(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{

					char *data;
					char UID_STM32[16];
				   	char IDon[7] ={0x67,0x74,0,0,1,0x0d,0x0a};
					char IDoff[7]={0x67,0x74,0,0,0,0x0d,0x0a};
					char IDs[7]  ={0x67,0x74,0,0,2,0x0d,0x0a};

			
 if (err == ERR_OK && p != NULL)
  {

    /* Inform TCP that we have taken the data. */
    tcp_recved(tpcb, p->tot_len);
		 data = p->payload;
		             if (strncmp(data, "getru", 5) == 0)
					 {
						  UID_STM32[0]=0xAA;
						  UID_STM32[4]=retry_TCP_connect;
						  UID_STM32[3]=retry_TCP_connect>>8;
						  UID_STM32[2]=retry_TCP_connect>>16;	  //重练次数
						  UID_STM32[1]=retry_TCP_connect>>24;

						  UID_STM32[8]=SystemRunTime;
						  UID_STM32[7]=SystemRunTime>>8;		  //运行时间
						  UID_STM32[6]=SystemRunTime>>16;
						  UID_STM32[5]=SystemRunTime>>24;
									
						  UID_STM32[9]=0xBB;
						  UID_STM32[10]=0x0d;
						  UID_STM32[11]=0x0a;
					 tcp_write(tpcb, &UID_STM32, 12, 1);
							 tcp_output(tpcb);
					 }
					 if (strncmp(data, "getid", 5) == 0)
						{
				 		  UID_STM32[0]=0xAB;
						  UID_STM32[1]=0x31;
						  UID_STM32[2]=0x32;
						  UID_STM32[3]=0x33;
						  UID_STM32[4]=0x34;

						  UID_STM32[5]=0x35;
						  UID_STM32[6]=0x36;
						  UID_STM32[7]=0x37;
						  UID_STM32[8]=0x38;
									
						  UID_STM32[9]=0x39;
						  UID_STM32[10]=0x40;
						  UID_STM32[11]=0x41;
						  UID_STM32[12]=0x42;

						  UID_STM32[13]=0xCD;
						  UID_STM32[14]=0x0d;
						  UID_STM32[15]=0x0a;
							 tcp_write(tpcb, &UID_STM32, 16, 1);
							 tcp_output(tpcb);

		                  }
						if(memcmp(&IDoff,data, 5)==0)
						{
						  	tcp_write(tpcb, &IDoff, 7, 1);
							 tcp_output(tpcb);
						
						}
						else if(memcmp(&IDon,data, 5)==0)
						{
							  tcp_write(tpcb, &IDon, 7, 1);
							 tcp_output(tpcb);
						
						}
						else if(memcmp(&IDs,data, 5)==0)
						{
								tcp_write(tpcb, &IDon, 7, 1);
							 tcp_output(tpcb);
						}

	   				 
	 pbuf_free(p);	
		


   }
if ((err == ERR_OK && p == NULL)||(err<0))
  {
	link=1;
    tcp_close(wpcb);

  }		 
  
	return ERR_OK;
} 
 void tcp_client_err(void *arg, err_t err)
 {
 	  if(err!=ERR_ABRT)
	  {
 	 link=1;
   tcp_close(wpcb);
   
 	   }
	   else
	   {
	   	 link=1;
	   }
	 
 }

void tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{


}
	// static char first=0;
 void  tcp_client_callback(void)
  {	
  
    	ip4_addr_t ip_addr;	

			wpcb= (struct tcp_pcb*)0;
			  /* Create a new TCP control block  */
			  wpcb = tcp_new();
			
			  /* Assign to the new pcb a local IP address and a port number */
			 if(tcp_bind(wpcb, IP_ADDR_ANY, TCP_PORT)!=ERR_OK)
			   {
			   		//failed
					 return ;
			   
			   }
		
			  IP4_ADDR(&ip_addr, 192,168,0,163);
			   // IP4_ADDR(&ip_addr, 192,168,1,3);
			/* Connect to the server: send the SYN *///TCP_PORT
		    tcp_connect(wpcb, &ip_addr, 2301, tcp_client_connected);
			//	tcp_poll(wpcb,tcp_client_poll,2);
			tcp_err( wpcb,tcp_client_err);  //register err
			tcp_recv(wpcb,tcp_client_reciver);  //register recv

}

/**
  * @brief  This function is called when the connection with the remote 
  *         server is established
  * @param arg user supplied argument
  * @param tpcb the tcp_pcb which received data
  * @param err error value returned by the tcp_connect 
  * @retval error value
  */

err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  
  	   link=0;
 	   retry_TCP_connect++;
	tpcb->so_options |= SOF_KEEPALIVE;
    tpcb->keep_idle = 500;// ms
    tpcb->keep_intvl = 500;// ms
    tpcb->keep_cnt = 2;// report error after 2 KA without response

   tcp_write(tpcb, "connecting....", 14, 1);
	tcp_output(tpcb);

  TcpPCB = tpcb;
  
  return ERR_OK;
}



uint32_t TCPCreatTrm =0;
 char i=0;
void TcpTestThread(uint32_t Time)
{
if (Time - TCPCreatTrm >= TCP_CREATTRM_INTERVAL)
  {
    TCPCreatTrm =  Time;

	 if(link)//if link =1,will be connectting to sever ..
	 {
	  
	  tcp_client_callback();
	 
	 }
	 if(i==0)//just actived once 
	 {
	   i=1;
	   udp_client_callback_app();
	 }
    
  }

}

	  const u8 Tcpbf[1024]={8};
	  char num=0,num1=0;
 void SendDataToSever(void)
 {			err_t erro;

 		 if(link==0)
		 {
		   //连接成功啦可以发送数据
				   erro= tcp_write(wpcb, Tcpbf, 1024, 1);
				 if(erro==ERR_OK )
			        {
				
				
				         if(tcp_output(wpcb)==ERR_OK)
						 {
						 
							//成功的发送了一包
						 }
						 else
						 {
						   	link=1;
   							 tcp_close(wpcb);
						 }
					}
					else
					{
						  num++;
						  if(num>=50)
						  {	   num=0;
									  switch(erro)
									  {
									 /* case -1:{UART_Write(UART0,"Out of memory error\r\n", sizeof("Out of memory error\r\n"));break;}
									  case -2:{UART_Write(UART0,"Buffer error", sizeof("Buffer error"));break;}
									  case -3:{UART_Write(UART0,"Timeout", sizeof("Timeout"));break;}
									  case -4:{UART_Write(UART0,"Routing problem", sizeof("Routing problem"));break;}
									  case -5:{UART_Write(UART0,"Operation in progress", sizeof("Operation in progress"));break;}
									  case -6:{UART_Write(UART0,"Illegal value", sizeof("Illegal value"));break;}
									  case -7:{UART_Write(UART0,"Operation would block", sizeof("Operation would block"));break;}
									  case -8:{UART_Write(UART0,"Address in use", sizeof("Address in use"));break;}
									  case -9:{UART_Write(UART0,"Already connected", sizeof("Already connected"));break;}
									  case -10:{UART_Write(UART0,"Connection aborted", sizeof("Connection aborted"));break;}
									  case -11:{UART_Write(UART0,"Connection reset", sizeof("Connection reset"));break;}
									  case -12:{UART_Write(UART0,"Connection closed", sizeof("Connection closed"));break;}
									  case -13:{UART_Write(UART0,"Not connected", sizeof("Not connected"));break;}
									  case -14:{UART_Write(UART0,"Illegal argument", sizeof("Illegal argument"));break;}
									  case -15:{UART_Write(UART0,"Low-level netif error", sizeof("Low-level netif error"));break;}
									   */
									  }
									  
									   if(erro!=(-1))
									   {
									   	link=1;
				  					   tcp_close(wpcb);
									   }

						}
					}

		 }
		 else
		 {
		   //没有建立一个成功的连接发什么？

		 }
 
 
 }
  uint8_t Udpbf[1024]={0x55};
 void UDPSendData(void)
 {
 
   	     struct pbuf *Udps;
		 uint16_t j;
		 ip_addr_t dst_ip_addr;
		 static u8 count=0;

		 IP4_ADDR(&dst_ip_addr, 192,168,0,163);
		if(i)
		{ 

		Udps = pbuf_alloc(PBUF_TRANSPORT, 1024, PBUF_RAM);
				if(Udps!=NULL)
				{
				   
				   /*for(j=0;j<1024;j++)
				   {
				   	Udpbf[j]=count;//

				   }
					Udpbf[j-1]=0x55;//
					Udpbf[0]=0xaa;//
					count++;
				   if( count==150)
				   {
				   	  count=0;
				   }
					   */
				memcpy((u8*)Udps->payload,(u8*)&Udpbf[0],1024);
		
				udp_sendto(upcb,Udps,&dst_ip_addr/* IP_ADDR*/,8080);
				
				 pbuf_free(Udps);
				}
				else
				{
					   num1++;
				   if(num1==50)
				   {	
				   num1=0;
			//	   UART_Write(UART0,"Oups is NULL\r\n", sizeof("Oups is NULL\r\n"));
				   }
				
				}

	 	}
 
 }


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
