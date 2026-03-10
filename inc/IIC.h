#ifndef __24CXX_H
#define __24CXX_H
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//鏈▼搴忓彧渚涘涔犱娇鐢紝鏈粡浣滆€呰鍙紝涓嶅緱鐢ㄤ簬鍏跺畠浠讳綍鐢ㄩ€?
//ALIENTEK鎴樿埌STM32寮€鍙戞澘
//24CXX椹卞姩 浠ｇ爜(閫傚悎24C01~24C16,24C32~256鏈粡杩囨祴璇?鏈夊緟楠岃瘉!)		   
//姝ｇ偣鍘熷瓙@ALIENTEK
//鎶€鏈鍧?www.openedv.com
//淇敼鏃ユ湡:2012/9/9
//鐗堟湰锛歏1.0
//鐗堟潈鎵€鏈夛紝鐩楃増蹇呯┒銆?
//Copyright(C) 骞垮窞甯傛槦缈肩數瀛愮鎶€鏈夐檺鍏徃 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//EID-V2浣跨敤鐨勬槸24c02锛屾墍浠ュ畾涔塃E_TYPE涓篈T24C02
#define EE_TYPE AT24C02
					  
u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//鎸囧畾鍦板潃璇诲彇涓€涓瓧鑺?
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//鎸囧畾鍦板潃鍐欏叆涓€涓瓧鑺?
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//鎸囧畾鍦板潃寮€濮嬪啓鍏ユ寚瀹氶暱搴︾殑鏁版嵁
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					//鎸囧畾鍦板潃寮€濮嬭鍙栨寚瀹氶暱搴︽暟鎹?
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//浠庢寚瀹氬湴鍧€寮€濮嬪啓鍏ユ寚瀹氶暱搴︾殑鏁版嵁
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//浠庢寚瀹氬湴鍧€寮€濮嬭鍑烘寚瀹氶暱搴︾殑鏁版嵁

u8 AT24CXX_Check(void);  //妫€鏌ュ櫒浠?
void AT24CXX_Init(void); //鍒濆鍖朓IC
#endif
