/**
  ******************************************************************************
  * @file InputCaptureMode/main.c 
  * @author   MCD Application Team
  * @version  V3.0.0
  * @date     04/27/2009
  * @brief    Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/** @addtogroup InputCaptureMode
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h"
#include "lcd.h"
#include "stdio.h"

extern u32 xScreen, yScreen;

/**
* @title 通信协议注释
* @PCtoMCUWrite write #wordAddress sentence.
* @PCtoMCURead read #wordAddress #charNum
* @MCUtoPCfomate sentence.
*/

typedef enum Error{
	ReceiveErrorCMD,ReceiveErrorData
} Error;

int count = 0;
#define USART_REC_LEN 200
extern char USART_RX_BUF[USART_REC_LEN];
extern uint8_t Read_CMD;
extern uint8_t Write_CMD;
extern uint8_t Flag;
extern uint8_t Transmit_state_OK;
extern u16 RX_count;
int isWaiting;
int isReady;
void TransmitError(Error er);

extern void number_set(int num,int index);
extern void Base_Timer6_Config(uint16_t arr, uint16_t psc);
extern void Base_Timer2_Config(uint16_t arr, uint16_t psc);
extern void Base_Timer3_Config(uint16_t arr, uint16_t psc);
extern void UART1_Init(void);
extern void RS232SendStr(char* data);

#define length 200
#define high 200
#define blank 20
int GameState = 0;
int isSelect;
int wayClear;
int isEnd;
int color_round;
#define CHESS_SIZE 14
#define CHOICE_SIZE 5
int chess_move;
int isBoard;
int isComputer;

void BoardInit()
{
  /* System Clocks Configuration */
	SystemInit();
	
}

void LCD_Background(void){
	int i;
	LCD_Change_Brash_Color(GRAY);
	LCD_DrawLine(blank,blank,length+blank,blank);
	LCD_DrawLine(blank,blank,blank,high+blank);
	LCD_DrawLine(length+blank,blank,length+blank,high+blank);
	LCD_DrawLine(length+blank,high+blank,blank,high+blank);
	LCD_Change_Brash_Color(BLACK);
	LCD_ShowString(240,50,4*16,16,16,(uint8_t*)"Select");
	LCD_ShowString(240,100,10*16,16,16,(uint8_t*)"board");
	LCD_ShowString(240,150,10*16,16,16,(uint8_t*)"computer");
	LCD_ShowString(285,100,3*16,16,16,(uint8_t *)"OFF");
	LCD_ShowString(285,170,3*16,16,16,(uint8_t *)"OFF");
	LCD_Change_Brash_Color(GRAY);
	for(i = 1; i < 4; i++){
		LCD_DrawLine(blank+(i*length/4),blank,blank+(i*length/4),high+blank);
	}
	for(i = 1; i < 4 ; i++){
		LCD_DrawLine(blank,blank+(i*high/4),length+blank,blank+(i*high/4));
	}
	isSelect = 0;
	isBoard = 0;
	isComputer = 0;
	isEnd = 0;
	isReady = 1;
	isWaiting = 0;
//	LCD_ShowString();// 选择
//	LCD_ShowString();// 取消选择
//	LCD_ShowString();// 连接电脑
}

typedef struct chess{
	int x;
	int y;
} ChessTypedef;

ChessTypedef BlackChess[5],WhiteChess[5],Choice;
ChessTypedef Last_Choice;

uint8_t chess_table[5][5];
void Chess_Init(){
	int i;
	color_round = BLACK;
	chess_move = 1;
	wayClear = 1;
	isSelect = 0;
	Choice.x = 0;
	Choice.y = 0;
	for(i = 0; i < 5; i++){
		BlackChess[i].x = i;
		BlackChess[i].y = 0;
		WhiteChess[i].x = i;
		WhiteChess[i].y = 4;
		chess_table[0][i] = 1;
		chess_table[4][i] = 2;
	}
}

void LCD_Draw_Choice(u16 x0,u16 y0,u16 color)
{	
	int a,b;
	int di;
	a=0;b=16;	  
	LCD_Change_Brash_Color(color);
	di=3-(15<<1);             //判断下个点位置的标志
	while(a<=b)
	{
//		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
//		LCD_DrawPoint(x0+a,y0+b);             //6 
//		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
//		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  

//在指定位置画一个实心圆
//(x,y):坐标
//r    :半径
void LCD_Draw_ALLCircle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		int i = a,p = b;
		while(i>0){		
			LCD_DrawPoint(x0+b,y0-i);
			LCD_DrawPoint(x0-i,y0+b);
			i--;
	}		
		while(p>0){		
			LCD_DrawPoint(x0-a,y0-p);
			LCD_DrawPoint(x0-p,y0-a);
			LCD_DrawPoint(x0+a,y0-p);
			LCD_DrawPoint(x0-p,y0+a);
			LCD_DrawPoint(x0+a,y0+p);
			LCD_DrawPoint(x0+p,y0+a);
			p--;
	}
		a++;
		//Bresenham算法画圆    
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
	LCD_DrawPoint(x0,y0); //圆心坐标
}

void Draw_Chess(){
	int i;
	for(i = 0; i < 5; i++){
		LCD_Change_Brash_Color(BLACK);
		if(BlackChess[i].x>=0)LCD_Draw_ALLCircle(blank+BlackChess[i].x*length/4,blank+BlackChess[i].y*high/4,CHESS_SIZE);
		LCD_Change_Brash_Color(WHITE);
		if(WhiteChess[i].x>=0)LCD_Draw_ALLCircle(blank+WhiteChess[i].x*length/4,blank+WhiteChess[i].y*high/4,CHESS_SIZE);
		delay_ms(10);
	}
}
// 清除上个图案
void Clear_Last() {
	int i;
	int t;
	LCD_Draw_Choice(blank+Last_Choice.x*length/4,blank+Last_Choice.y*high/4,BRRED);
	LCD_Change_Brash_Color(GRAY);
	LCD_DrawPoint(blank+Last_Choice.x*length/4-16,blank+Last_Choice.y*high/4);
	LCD_DrawPoint(blank+Last_Choice.x*length/4+16,blank+Last_Choice.y*high/4);
}

ChessTypedef way[4];

void LCD_Draw_Way(){
	int i;
	int t;
	if(isSelect){
		LCD_Change_Brash_Color(RED);
		if(Choice.x > 0 && chess_table[Choice.y][Choice.x - 1]==0) {
			way[0].x = Choice.x - 1;
			way[0].y = Choice.y;
			LCD_Draw_ALLCircle(blank+(Choice.x - 1)*length/4,blank+Choice.y*high/4,CHOICE_SIZE);
		}
		else {
			way[0].x = -1;
		}
		if(Choice.x < 4 && chess_table[Choice.y][Choice.x + 1]==0) {
			way[1].x = Choice.x + 1;
			way[1].y = Choice.y;
			LCD_Draw_ALLCircle(blank+(Choice.x + 1)*length/4,blank+Choice.y*high/4,CHOICE_SIZE);
		}
		else {
			way[1].x = -1;
		}
		if(Choice.y > 0 && chess_table[Choice.y - 1][Choice.x]==0) {
			way[2].x = Choice.x;
			way[2].y = Choice.y - 1;
			LCD_Draw_ALLCircle(blank+Choice.x*length/4,blank+(Choice.y - 1)*high/4,CHOICE_SIZE);
		}
		else {
			way[2].x = -1;
		}
		if(Choice.y < 4 && chess_table[Choice.y + 1][Choice.x]==0) {
			way[3].x = Choice.x;
			way[3].y = Choice.y + 1;
			LCD_Draw_ALLCircle(blank+Choice.x*length/4,blank+(Choice.y + 1)*high/4,CHOICE_SIZE);
		}
		else {
			way[3].x = -1;
		}
		wayClear = 0;
	}
	else if(!isSelect && !wayClear){
		for(i = 0; i<4;i++){
			if(way[i].x!=-1){
				LCD_Change_Brash_Color(BRRED);
				LCD_Draw_ALLCircle(blank+way[i].x*length/4,blank+way[i].y*high/4,CHOICE_SIZE);
			}
			LCD_Change_Brash_Color(GRAY);
			for(t = -CHESS_SIZE; t<=CHESS_SIZE;t++){
				LCD_DrawPoint(blank+way[i].x*length/4,blank+way[i].y*high/4+t);
				LCD_DrawPoint(blank+way[i].x*length/4,blank+way[i].y*high/4-t);
				LCD_DrawPoint(blank+way[i].x*length/4+t,blank+way[i].y*high/4);
				LCD_DrawPoint(blank+way[i].x*length/4-t,blank+way[i].y*high/4);
			}
		}
		wayClear = 1;
	}
}

void Chess_Update(void){
	Clear_Last();
	LCD_Change_Brash_Color(RED);
	LCD_Draw_Choice(blank+Choice.x*length/4,blank+Choice.y*high/4,DARKBLUE);
	LCD_Draw_Way();
//	LCD_Background();
	if(chess_move){
		Draw_Chess();
		chess_move = 0;
	}
	
}

int exist(){
	if(color_round == BLACK && chess_table[Choice.y][Choice.x]==1) return 1;
	else if(color_round == WHITE && chess_table[Choice.y][Choice.x]==2) return 1;
	else return 0;
}


void Update_Choice(int x,int y){
	if(isSelect) LCD_Draw_Way();
	Last_Choice.x = Choice.x;
	Last_Choice.y = Choice.y;
	Choice.x = x;
	Choice.y = y;
}

void clear_chess(int x,int y){
	int i;
	LCD_Change_Brash_Color(BRRED);
	LCD_Draw_ALLCircle(blank+x*length/4,blank + y*high/4,CHESS_SIZE);
	LCD_Change_Brash_Color(GRAY);
	for (i = -CHESS_SIZE;i <= CHESS_SIZE;i++){
		LCD_DrawPoint(blank+x*length/4+i,blank + y*high/4);
		LCD_DrawPoint(blank+x*length/4-i,blank + y*high/4);
		LCD_DrawPoint(blank+x*length/4,blank + y*high/4+i);
		LCD_DrawPoint(blank+x*length/4,blank + y*high/4-i);
	}
	
}

void Board_proccess(){
	if(isBoard && color_round == WHITE && !isEnd){
		int i;
		int j;
		int t;
		int x,y,pos_x;
		int count1,count2,count3,count4;
		for(i = 24; i >= 0; i--){
			if(chess_table[i/5][i%5] == 1 && WhiteChess[i%5].x!=-1) {
				if(WhiteChess[i%5].y - 1 >= 0 && chess_table[WhiteChess[i%5].y - 1][WhiteChess[i%5].x] == 0){
					clear_chess(WhiteChess[i%5].x,WhiteChess[i%5].y);
					chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x] = 0;
					chess_table[WhiteChess[i%5].y - 1][WhiteChess[i%5].x] = 2;
					WhiteChess[i%5].y = WhiteChess[i%5].y - 1;
					chess_move = 1;
					x = WhiteChess[i%5].x;
					y = WhiteChess[i%5].y;
					color_round = BLACK;
					break;
				}
			}
		}
		if(chess_move != 1){
			for(i = 24; i >= 0; i--){
				if(chess_table[i/5][i%5] == 2 && WhiteChess[i%5].x!=-1) {
					if(WhiteChess[i%5].x + 1 < 5 && chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x + 1] == 0){
						clear_chess(WhiteChess[i%5].x,WhiteChess[i%5].y);
						chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x] = 0;
						chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x + 1] = 2;
						WhiteChess[i%5].x = WhiteChess[i%5].x + 1;
						chess_move = 1;
						x = WhiteChess[i%5].x;
						y = WhiteChess[i%5].y;
						color_round = BLACK;
						break;
					}
					else if(WhiteChess[i%5].x - 1 >= 0 && chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x - 1] == 0){
						clear_chess(WhiteChess[i%5].x,WhiteChess[i%5].y);
						chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x] = 0;
						chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x - 1] = 2;
						WhiteChess[i%5].x = WhiteChess[i%5].x - 1;
						chess_move = 1;
						x = WhiteChess[i%5].x;
						y = WhiteChess[i%5].y;
						color_round = BLACK;
						break;
					}
				}
			}
		}
		if(chess_move != 1){
			for(i = 24; i >= 0; i--){
				if(chess_table[i/5][i%5] == 2 && WhiteChess[i%5].x!= -1) {
					if(WhiteChess[i%5].y + 1 < 5 && chess_table[WhiteChess[i%5].y + 1][WhiteChess[i%5].x] == 0){
						clear_chess(WhiteChess[i%5].x,WhiteChess[i%5].y);
						chess_table[WhiteChess[i%5].y][WhiteChess[i%5].x] = 0;
						chess_table[WhiteChess[i%5].y + 1][WhiteChess[i%5].x] = 2;
						WhiteChess[i%5].y = WhiteChess[i%5].y + 1;
						chess_move = 1;
						x = WhiteChess[i%5].x;
						y = WhiteChess[i%5].y;
						color_round = BLACK;
						break;
					}
				}
			}
		}
		if(chess_move != 1){
			for(i = 4; i >= 0; i--){
				if(WhiteChess[i].x != -1){
					if(WhiteChess[i].y - 1 >= 0 && chess_table[WhiteChess[i].y - 1][WhiteChess[i].x] == 0){
						clear_chess(WhiteChess[i].x,WhiteChess[i].y);
						chess_table[WhiteChess[i].y][WhiteChess[i].x] = 0;
						chess_table[WhiteChess[i].y - 1][WhiteChess[i].x] = 2;
						WhiteChess[i].y = WhiteChess[i].y - 1;
						chess_move = 1;
						x = WhiteChess[i%5].x;
						y = WhiteChess[i%5].y;
						color_round = BLACK;
						break;
					}
					else if(WhiteChess[i].y + 1 < 5 && chess_table[WhiteChess[i].y + 1][WhiteChess[i].x] == 0){
						clear_chess(WhiteChess[i].x,WhiteChess[i].y);
						chess_table[WhiteChess[i].y][WhiteChess[i].x] = 0;
						chess_table[WhiteChess[i].y + 1][WhiteChess[i].x] = 2;
						WhiteChess[i].y = WhiteChess[i].y + 1;
						chess_move = 1;
						x = WhiteChess[i%5].x;
						y = WhiteChess[i%5].y;
						color_round = BLACK;
						break;
					}
					else if(WhiteChess[i].x - 1 >= 0 && chess_table[WhiteChess[i].y][WhiteChess[i].x - 1] == 0){
						clear_chess(WhiteChess[i].x,WhiteChess[i].y);
						chess_table[WhiteChess[i].y][WhiteChess[i].x] = 0;
						chess_table[WhiteChess[i].y][WhiteChess[i].x - 1] = 2;
						WhiteChess[i].x = WhiteChess[i].x - 1;
						chess_move = 1;
						x = WhiteChess[i%5].x;
						y = WhiteChess[i%5].y;
						color_round = BLACK;
						break;
					}
					else if(WhiteChess[i].x + 1 < 5 && chess_table[WhiteChess[i].y][WhiteChess[i].x + 1] == 0){
						clear_chess(WhiteChess[i].x,WhiteChess[i].y);
						chess_table[WhiteChess[i].y][WhiteChess[i].x] = 0;
						chess_table[WhiteChess[i].y][WhiteChess[i].x + 1] = 2;
						WhiteChess[i].x = WhiteChess[i].x + 1;
						chess_move = 1;
						x = WhiteChess[i%5].x;
						y = WhiteChess[i%5].y;
						color_round = BLACK;
						break;
					}
				}
			}
		}
		
		if(chess_move) {
			count1 = 0;
			count2 = 0;
			count3 =0;
			count4 = 0;
			for(t = 0;t < 5; t ++){
				count1 += chess_table[y][t];
				if(chess_table[y][t] == 1) pos_x = t;
				if(t - x <= 2 && t - x >= 0) count2 += chess_table[y][t];
				if(t - x <= 1 && t - x >= -1) count3 += chess_table[y][t];
				if(t - x <= 0 && t - x >= -2) count4 += chess_table[y][t];
			}
			if(count1 == 5 && (count2 == 5 || count3 == 5 || count4 ==5)){
				for(i = 0; i < 5; i++){
					if(BlackChess[i].x==pos_x && BlackChess[i].y == y) {
						BlackChess[i].x = -1;
						chess_table[y][pos_x] = 0;
						clear_chess(pos_x,y);
					}
				}
			}
			Chess_Update();
		}
	}
}

void Computer_proccess(){
	if(isComputer && color_round == WHITE && !isEnd && !isWaiting){
		char str[200];
		sprintf(str,"s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%de",
		chess_table[0][0],chess_table[0][1],chess_table[0][2],chess_table[0][3],chess_table[0][4],
		chess_table[1][0],chess_table[1][1],chess_table[1][2],chess_table[1][3],chess_table[1][4],
		chess_table[2][0],chess_table[2][1],chess_table[2][2],chess_table[2][3],chess_table[2][4],
		chess_table[3][0],chess_table[3][1],chess_table[3][2],chess_table[3][3],chess_table[3][4],
		chess_table[4][0],chess_table[4][1],chess_table[4][2],chess_table[4][3],chess_table[4][4]);
		RS232SendStr(str);
		isWaiting = 1;
	}
}

void move(int x,int y)
{
	
	int t;
	int i;
	int pos_correct =0;
	for(t = 0; t < 5; t++){
		if(way[t].x ==x && way[t].y == y){
			pos_correct = 1;
			break;
		}
	}
	if(chess_table[Choice.y][Choice.x]==1 && pos_correct){
		int count1 = 0;
		int count2 = 0;
		int count3 = 0;
		int count4 = 0;
		int pos_x =0;
		chess_table[Choice.y][Choice.x] = 0;
		for(i = 0; i <5 ;i++){
			if(BlackChess[i].x==Choice.x && BlackChess[i].y==Choice.y) break;
		}
		chess_table[y][x] = 1;
		clear_chess(BlackChess[i].x,BlackChess[i].y);
		BlackChess[i].x=x;
		BlackChess[i].y=y;
		isSelect = !isSelect;
		chess_move = 1;
		color_round = WHITE;
		for(t = 0;t < 5; t ++){
			count1 += chess_table[y][t];
			if(chess_table[y][t] == 2) pos_x = t;
			if(t - x <= 2 && t - x >= 0) count2 += chess_table[y][t];
			if(t - x <= 1 && t - x >= -1) count3 += chess_table[y][t];
			if(t - x <= 0 && t - x >= -2) count4 += chess_table[y][t];
		}
		if(count1 == 4 && (count2 == 4 || count3 == 4 || count4 ==4)){
			for(i = 0; i < 5; i++){
				if(WhiteChess[i].x==pos_x && WhiteChess[i].y == y){
					WhiteChess[i].x = -1;
					chess_table[y][pos_x] = 0;
					clear_chess(pos_x,y);
				}
			}
		}
		Chess_Update();
		count1 =0;
		for(i = 0; i < 5;i++){
			if(WhiteChess[i].x!=-1)count1++;
		}
		if(count1 == 1){
			LCD_Change_Brash_Color(RED);
			LCD_ShowString(200,200,160,16,16,(uint8_t*)"BLACK WIN");
			isEnd = 1;
		}
	}
	else if(chess_table[Choice.y][Choice.x]==2 && pos_correct){
		int count1 = 0;
		int count2 = 0;
		int count3 = 0;
		int count4 = 0;
		int pos_x =0;
		chess_table[Choice.y][Choice.x] = 0;
		for(i = 0; i <5 ;i++){
			if(WhiteChess[i].x==Choice.x && WhiteChess[i].y==Choice.y) break;
		}
		chess_table[y][x] = 2;
		clear_chess(WhiteChess[i].x,WhiteChess[i].y);
		WhiteChess[i].x=x;
		WhiteChess[i].y=y;
		isSelect = !isSelect;
		chess_move = 1;
		color_round = BLACK;
		for(t = 0;t < 5; t ++){
			count1 += chess_table[y][t];
			if(chess_table[y][t] == 1) pos_x = t;
			if(t - x <= 2 && t - x >= 0) count2 += chess_table[y][t];
			if(t - x <= 1 && t - x >= -1) count3 += chess_table[y][t];
			if(t - x <= 0 && t - x >= -2) count4 += chess_table[y][t];
		}
		if(count1 == 5 && (count2 == 5 || count3 == 5 || count4 ==5)){
			for(i = 0; i < 5; i++){
				if(BlackChess[i].x==pos_x && BlackChess[i].y == y) {
					BlackChess[i].x = -1;
					chess_table[y][pos_x] = 0;
					clear_chess(pos_x,y);
				}
			}
		}
		Chess_Update();
		count1 =0;
		for(i = 0; i < 5;i++){
			if(WhiteChess[i].x!=-1)count1++;
		}
		if(count1 == 1){
			LCD_Change_Brash_Color(RED);
			LCD_ShowString(200,200,160,16,16,(uint8_t*)"WHITE WIN");
			isEnd = 1;
		}
	}
	
}

void REC_process(){
	int x1,y1,x2,y2;
	int pos_x;
	int count1,count2,count3,count4;
	int t;
	int i;
	if(color_round == WHITE && !isEnd){
		
		if(USART_RX_BUF[0] >= '0' && USART_RX_BUF[0] <= '9'){
			x1 = USART_RX_BUF[0] - '0';
			y1 = USART_RX_BUF[1] - '0';
			x2 = USART_RX_BUF[2] - '0';
			y2 = USART_RX_BUF[3] - '0';
			clear_chess(x1,y1);
			for(i = 0; i <5 ; i ++){
				if(WhiteChess[i].x == x1 && WhiteChess[i].y == y1) break;
			}
			chess_table[y1][x1] = 0;
			chess_table[y2][x2] = 2;
			WhiteChess[i].x = x2;
			WhiteChess[i].y = y2;
			count1 = 0;
			count2 = 0;
			count3 =0;
			count4 = 0;
			for(t = 0;t < 5; t ++){
				count1 += chess_table[y2][t];
				if(chess_table[y2][t] == 1) pos_x = t;
				if(t - x2 <= 2 && t - x2 >= 0) count2 += chess_table[y2][t];
				if(t - x2 <= 1 && t - x2 >= -1) count3 += chess_table[y2][t];
				if(t - x2 <= 0 && t - x2 >= -2) count4 += chess_table[y2][t];
			}
			if(count1 == 5 && (count2 == 5 || count3 == 5 || count4 ==5)){
				for(i = 0; i < 5; i++){
					if(BlackChess[i].x==pos_x && BlackChess[i].y == y2) {
						BlackChess[i].x = -1;
						chess_table[y2][pos_x] = 0;
						clear_chess(pos_x,y2);
					}
				}
			}
			chess_move = 1;
			Chess_Update();
			RX_count = 0;
			isWaiting = 0;
			color_round=BLACK;
		}

	}
}
extern int flagx,flagy;
extern ax,by;
int main(void)
{
	char ID[20];
	BoardInit();
	UART1_Init();
	Base_Timer3_Config(10000 - 1,72 - 1); // 10ms
	LCD_Configuration();
	Touch_test();
	LCD_Init();
	Chess_Init();
	LCD_Background();
	Chess_Update();
	
//	state = key_line();
	while(1) {
		// 串口接收处理
		if(isReady){
			Computer_proccess();
		}
		if(Flag){
			REC_process();
			Flag = 0;
		}
		if(isBoard){
			Board_proccess();
		}
		if(flagx && flagy){
			if(!isSelect)Update_Choice(ax,by);
			else {
				move(ax,by);
			}
			Chess_Update();
			flagx = 0;
			flagy = 0;
		}
//		game_run();
	}
}


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/