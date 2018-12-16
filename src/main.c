#include <psp2/sysmodule.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <psp2/ctrl.h>
#include <psp2/net/net.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdbool.h>

#include "debugScreen.h"

#define printf psvDebugScreenPrintf

#define NET_PARAM_MEM_SIZE (1*1024*1024)

/* dist IP */
//#define IP "192.168.13.3"
#define IP "192.168.4.1"

/* dist PORT */
#define PORT 5000

/* buffer length */
#define BUF_LEN 7

static int32_t sfd; /* Socket file descriptor */
int send(char *buffer);
int fix(int n);
int main (int argc, char *argv[]){
	SceNetSockaddrIn serv_addr; /* server address to send data to */
	SceNetInitParam net_init_param; /* Net init param structure */

	psvDebugScreenInit(); /* start psvDebugScreen */
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET); /* load NET module */

	/* Initialize Net */
	net_init_param.memory = malloc(NET_PARAM_MEM_SIZE);
	memset(net_init_param.memory, 0, NET_PARAM_MEM_SIZE);
	net_init_param.size = NET_PARAM_MEM_SIZE;
	net_init_param.flags = 0;

	serv_addr.sin_len = sizeof(serv_addr);
	serv_addr.sin_family = SCE_NET_AF_INET;
	serv_addr.sin_port = sceNetHtons(PORT);
	memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero)); /* fill sin_zero with zeroes */

	sceNetInit(&net_init_param);

	sceNetInetPton(SCE_NET_AF_INET, IP, &serv_addr.sin_addr);

	sfd = sceNetSocket("controller", SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, 0);
	if (sfd < 0)
		goto exit;
	sceNetConnect(sfd, (SceNetSockaddr *)&serv_addr, sizeof(serv_addr));

	char send_buf[BUF_LEN];

	
	/* Initialize Controller */
	printf("press Select+Start+L+R to stop\n");
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
	SceCtrlData ctrl;
	const char* btn_label[]={"SELECT ","","","START ",
		"UP ","RIGHT ","DOWN ","LEFT ","L ","R ","","",
		"TRIANGLE ","CIRCLE ","CROSS ","SQUARE "};

	while(true){
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		printf("Buttons:%08X == ", ctrl.buttons);
		int i;
		for(i=0; i < sizeof(btn_label)/sizeof(*btn_label); i++){
			printf("\e[9%im%s",(ctrl.buttons & (1<<i)) ? 7 : 0, btn_label[i]);
		}
		send_buf[1] = 0x00;
		send_buf[2] = 0x00;
		send_buf[1] |= (bool)(ctrl.buttons & SCE_CTRL_UP) << 6;
		send_buf[1] |= (bool)(ctrl.buttons & SCE_CTRL_RIGHT) << 5;
		send_buf[1] |= (bool)(ctrl.buttons & SCE_CTRL_DOWN) << 4;
		send_buf[1] |= (bool)(ctrl.buttons & SCE_CTRL_LEFT) << 3;
		send_buf[1] |= (bool)(ctrl.buttons & SCE_CTRL_LTRIGGER) << 2;
		send_buf[1] |= (bool)(ctrl.buttons & SCE_CTRL_RTRIGGER) << 1;
		send_buf[2] |= (bool)(ctrl.buttons & SCE_CTRL_TRIANGLE) << 6;
		send_buf[2] |= (bool)(ctrl.buttons & SCE_CTRL_CIRCLE) << 5;
		send_buf[2] |= (bool)(ctrl.buttons & SCE_CTRL_CROSS) << 4;
		send_buf[2] |= (bool)(ctrl.buttons & SCE_CTRL_SQUARE) << 3;
		send_buf[2] |= (bool)(ctrl.buttons & SCE_CTRL_START) << 2;
		send_buf[2] |= (bool)(ctrl.buttons & SCE_CTRL_SELECT) << 1;
		send_buf[3] = fix(ctrl.lx/2);
		send_buf[4] = fix(ctrl.ly/2);
		send_buf[5] = fix(ctrl.rx/2);
		send_buf[6] = fix(ctrl.ry/2);

		int sum=0;
		for(i=1; i<BUF_LEN; i++){
			sum += send_buf[i];
		}
		send_buf[0] = 0x80 | (sum & 0x7F);

		send(send_buf);
		printf("\e[m Stick:[%3i:%3i][%3i:%3i]\r", ctrl.lx,ctrl.ly, ctrl.rx,ctrl.ry);
		sceKernelDelayThread(2000); //2msec
	}
	
exit:
	/* Select to exit */
	printf("\n\n\n\n\n\n                Press select to exit.\n");
	while(1){
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		if (ctrl.buttons & SCE_CTRL_SELECT)
			break;

		sceKernelDelayThread(100*1000);
	}
	sceNetSocketClose(sfd); /* Close socket */
	sceNetTerm(); /* Close net */
	sceSysmoduleUnloadModule(SCE_SYSMODULE_NET); /* Unload net module */
	sceKernelExitProcess(0); /* Exit */
	return 0;
}

int send(char *buffer)
{
	return sceNetSend(sfd, buffer, BUF_LEN, 0);
}

int fix(int n){
	if(n > 127)
		return 127;
	else
		return n; 
}