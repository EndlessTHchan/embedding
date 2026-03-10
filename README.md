# STM32 五子棋嵌入式项目

> 基于 STM32F103 开发板的 5×5 五子棋游戏，支持双人对战、内置 AI 自动对战、联机电脑 AI 三种模式，配备 TFT 触摸屏与 UART 通信。

---

## 目录

- [项目概述](#项目概述)
- [硬件环境](#硬件环境)
- [引脚配置](#引脚配置)
- [软件架构](#软件架构)
- [LCD 屏幕刷新机制](#lcd-屏幕刷新机制)
- [五子棋游戏算法](#五子棋游戏算法)
- [通信协议](#通信协议)
- [库与外设驱动](#库与外设驱动)
- [项目文件结构](#项目文件结构)
- [构建与烧录](#构建与烧录)

---

## 项目概述

本项目运行于 **ALIENTEK MiniSTM32F103** 系列开发板，实现了一个小型 5×5 五子棋游戏。游戏规则以吃子为胜负判断条件（白棋或黑棋剩余 1 枚时另一方获胜），并提供三种对战模式：

| 模式 | 描述 |
|------|------|
| **双人对战** | 两名玩家通过触摸屏轮流操作 |
| **内置 AI（Board 模式）** | 白棋由本地简单策略 AI 自动移动 |
| **联机电脑 AI（Computer 模式）** | 通过 UART 将棋盘状态发送至上位机，由外部 AI 计算最优走法后返回 |

---

## 硬件环境

| 项目 | 说明 |
|------|------|
| **主控 MCU** | STM32F103（72 MHz，Cortex-M3） |
| **LCD 屏** | 2.4" / 2.8" / 3.5" / 4.3" / 7" TFT 液晶屏 |
| **LCD 驱动 IC** | ILI9341 / ILI9325 / NT35310 / NT35510 / SSD1963 / RM68042 / LGDP4531 等 |
| **串口通信** | USART1（RS232 / 上位机 AI） |
| **开发工具** | Keil MDK（flowlight.uvprojx） |

---

## 引脚配置

### LCD 控制总线（16 位并口）

LCD 采用 16 位并行数据总线，控制信号及数据线分配如下：

| 信号 | 引脚 | 说明 |
|------|------|------|
| `LCD_LED`（背光） | **PC0** | 高电平点亮 |
| `LCD_CS`（片选） | **PD7** | 低有效 |
| `LCD_RS`（数据/命令切换） | **PD4** | 低=命令，高=数据 |
| `LCD_WR`（写控制） | **PD5** | 低脉冲写入 |
| `LCD_RD`（读控制） | **PD11** | 低脉冲读取 |
| `RESET`（复位） | **PD12** | 高电平有效 |

**16 位数据总线（D0–D15）映射：**

| 数据位 | 引脚 |
|--------|------|
| D0, D1 | PD14, PD15 |
| D2, D3 | PD0, PD1 |
| D4–D12 | PE7–PE15 |
| D13, D14, D15 | PD8, PD9, PD10 |

> 数据读写函数 `DATAOUT()` / `DATAIN()` 通过位操作将 16 位数据映射到上述分散引脚，实现并行读写。

---

### UART 串口（USART1）

| 信号 | 引脚 | 说明 |
|------|------|------|
| `TX` | **PA9** | 复用推挽输出，50 MHz |
| `RX` | **PA10** | 浮空输入 |

**串口参数：**

```
波特率：115200 bps
数据位：8 bit
停止位：1 bit
校验位：无
硬件流控：无
```

---

### LED 指示灯（GPIOE）

| LED | 引脚 | 控制说明 |
|-----|------|----------|
| LED0 | **PE8** | 低电平点亮（通过 BSRR 操作） |
| LED1 | **PE9** | 低电平点亮 |
| LED2 | **PE10** | 低电平点亮 |
| LED3 | **PE11** | 低电平点亮 |

---

### 数码管（4 位，动态扫描）

| 器件 | 引脚 | 说明 |
|------|------|------|
| 段码输出（a–g, dp） | **PD0–PD7** | 推挽输出，低电平有效 |
| 位选控制 | **PC10–PC13** | 低电平选通对应位 |

---

### 矩阵键盘（GPIOE）

| 功能 | 引脚 |
|------|------|
| 行输出（扫描行） | **PE0–PE3** |
| 列输入（检测列） | **PE4–PE6**（下拉输入） |

外部中断 **EXTI4 / EXTI5–9** 挂载在 GPIOE 引脚，上升沿触发（NVIC 抢占优先级 2，子优先级 2），用于快速响应按键。

---

### SPI（SPI2，保留/扩展用）

| 信号 | 引脚 | 说明 |
|------|------|------|
| SCK | **PB13** | 时钟 |
| MISO | **PB14** | 主入从出 |
| MOSI | **PB15** | 主出从入 |
| CS | **PC3** | 软件控制片选 |

> SPI2 配置为主模式，8 位数据，模式 0（CPOL=Low，CPHA=1Edge），预分频 Fclk/2，MSB 先行。主要用于扩展 Flash（ENC25Q80）等外设。

---

## 软件架构

```
main()
 ├── BoardInit()          → SystemInit()，系统时钟初始化
 ├── UART1_Init()         → 串口及 NVIC 配置
 ├── Base_Timer3_Config() → TIM3 配置（帧超时检测）
 ├── LCD_Configuration()  → LCD GPIO 时钟与控制引脚初始化
 ├── Touch_test()         → 触摸屏校准/识别
 ├── LCD_Init()           → 识别 LCD 驱动 IC，完成初始化序列
 ├── Chess_Init()         → 棋盘数据初始化（chess_table[5][5]）
 ├── LCD_Background()     → 绘制棋盘网格及 UI 控件
 ├── Chess_Update()       → 首次绘制所有棋子
 └── while(1)            → 主循环
      ├── Computer_proccess()  → Computer 模式：发送棋盘到上位机
      ├── REC_process()        → 解析 UART 接收数据，更新白棋位置
      ├── Board_proccess()     → Board 模式：本地 AI 自动移动白棋
      └── 触摸处理             → 更新光标位置 / 执行落子 move()
```

---

## LCD 屏幕刷新机制

### 驱动层（lcd.c）

LCD 驱动采用**16 位并行接口**，支持多种主流驱动 IC，通过读取 LCD ID 自动适配：

**底层读写流程：**

```
写数据：RS=HIGH → CS=LOW → DATAOUT(16bit) → WR_CLR → WR_SET → CS=SET
写命令：RS=LOW  → CS=LOW → DATAOUT(reg)  → WR_CLR → WR_SET → CS=SET
读数据：RS=HIGH → CS=LOW → RD_CLR → [dummy read] → RD_CLR → DATAIN() → RD_SET → CS=SET
```

**坐标写入（LCD_SetCursor / LCD_DrawPoint）：**
1. 发送 `setxcmd`（如 `0x2A`）+ X 坐标高/低字节
2. 发送 `setycmd`（如 `0x2B`）+ Y 坐标高/低字节
3. 发送写 GRAM 命令（`wramcmd`，如 `0x2C`）
4. 写入 16 位 RGB565 颜色值

**扫描方向（LCD_Scan_Dir）：**  
支持 8 种扫描方向（L2R_U2D / L2R_D2U / R2L_U2D / R2L_D2U / U2D_L2R / U2D_R2L / D2U_L2R / D2U_R2L），默认 **L2R_U2D（从左到右，从上到下）**。

**分辨率支持：**

| LCD IC | 竖屏分辨率 | 横屏分辨率 |
|--------|-----------|-----------|
| ILI9341 | 240×320 | 320×240 |
| NT35310 / RM68042 | 320×480 | 480×320 |
| NT35510 | 480×800 | 800×480 |
| SSD1963 | 480×800 | 800×480 |

---

### 应用层绘图（main.c）

游戏层**不使用填充块刷新**，而是采用**逐点 Bresenham 算法**绘图，最大程度减少总线占用：

#### 实心圆（棋子）— `LCD_Draw_ALLCircle(x0, y0, r)`

采用改进版 **Bresenham 圆弧算法**，从 `(a=0, b=r)` 出发，每步判断 `di`：

```c
di = 3 - (r << 1);   // 初始决策量
while (a <= b) {
    // 对当前 (a,b) 八分圆所有对称点逐一填充列
    // 对圆心到圆周的每一列调用 LCD_DrawPoint()
    a++;
    if (di < 0)  di += 4*a + 6;
    else       { di += 10 + 4*(a-b); b--; }
}
```

> 算法通过对 `a`、`b` 两个区间内的所有像素点逐一描绘，实现填充圆效果，无需填充矩形块。

#### 空心圆（光标）— `LCD_Draw_Choice(x0, y0, color)`

仅绘制圆弧的左右对称点（省略上下），用于标记当前选中的棋子位置，使用相同 Bresenham 算法但只绘制部分对称点。

#### 直线（棋盘网格）— `LCD_DrawLine(x1, y1, x2, y2)`

棋盘网格使用 `LCD_DrawLine()` 函数（Bresenham 直线算法）绘制，仅在初始化时绘制一次，后续不重绘网格。

#### 局部擦除优化

棋子移动时，仅清除上一个位置（`clear_chess()`），用背景色（灰色）重画对应交叉点，而非刷新整个屏幕：

```c
void clear_chess(int x, int y) {
    LCD_Draw_ALLCircle(..., BRRED);   // 用棕红色覆盖旧棋子
    LCD_DrawPoint(..., GRAY);         // 用灰色重画网格交叉线
}
```

#### 空白棋盘格子布局

棋盘为 4×4 格（5×5 交叉点），在像素坐标 `[blank, blank+length] x [blank, blank+high]` 范围内：

```c
#define length  200   // 棋盘像素宽度
#define high    200   // 棋盘像素高度
#define blank   20    // 边距

// 交叉点像素坐标：
// x_pixel = blank + col * length/4
// y_pixel = blank + row * high/4
```

---

## 五子棋游戏算法

### 数据结构

```c
uint8_t chess_table[5][5];  // 棋盘状态：0=空，1=黑棋，2=白棋

typedef struct { int x, y; } ChessTypedef;
ChessTypedef BlackChess[5];   // 黑棋5枚，记录每枚位置
ChessTypedef WhiteChess[5];   // 白棋5枚，记录每枚位置
ChessTypedef Choice;          // 当前选中的棋子坐标
ChessTypedef way[4];          // 当前选中棋子的可移动方向（上/下/左/右）
```

初始布局：黑棋在第 0 行（y=0），白棋在第 4 行（y=4）。

### 玩家操作流程（`move()`）

```
1. 触摸检测 → 坐标 (ax, by)
   ├── 未选中状态：Update_Choice(ax, by) 选中该位置棋子
   └── 已选中状态：move(ax, by)
        ├── 验证目标位置是否在 way[] 可移动列表中
        ├── 更新 chess_table[][]：原位清空，新位置置 1 or 2
        ├── 更新 BlackChess[] / WhiteChess[] 数组
        ├── chess_move = 1（触发重绘）
        ├── 翻转 color_round（交换回合）
        └── 吃子检测（见下文）
```

### 吃子判断逻辑

每次落子后，检测对方棋子是否被三枚己方棋子包围（同行连续 3 子）。以黑棋落子为例，检测白棋能否被吃：

```c
// 对落子后所在行，统计各类连续配置
count1 = 同行所有棋子之和（连续性计数）
count2 = 落子点右侧3格内棋子之和
count3 = 落子点两侧共3格内棋子之和
count4 = 落子点左侧3格内棋子之和

if (count1 == 4 && (count2==4 || count3==4 || count4==4)) {
    // 找到被包围的对方棋子并删除（坐标置-1，清除显示）
}
```

> **胜利条件**：当一方棋子数量减少到仅剩 1 枚时，另一方获胜，LCD 显示 "BLACK WIN" 或 "WHITE WIN"。

### Board 模式 AI（`Board_proccess()`）

轮到白棋时，按以下优先级贪心搜索移动：

1. **进攻优先**：尝试将白棋向有黑棋的行推进（正前方路径）
2. **侧移**：若正前方受阻，左右横向移动
3. **后退保留**：前两者均失败时，向后方空位移动，保持棋子存活

每次只移动一枚棋子，移动后切换回合为黑棋。

### Computer 模式（`Computer_proccess()` + `REC_process()`）

```
白棋轮次触发
     │
     ▼
Computer_proccess()：
  → 将 chess_table[5][5] 序列化为字符串：
    "s01020102...e"（25个数字，由 's' 开始 'e' 结束）
  → 通过 RS232SendStr() 发送给上位机

上位机 AI 计算最优走法后返回：
    格式：x1y1x2y2（4位数字）
    x1,y1=白棋原坐标，x2,y2=目标坐标
     │
     ▼
REC_process()：
  → 解析 USART_RX_BUF[0..3]
  → 更新白棋坐标并检测吃子
  → chess_move = 1（触发重绘）
  → isWaiting = 0（恢复轮次监听）
```

---

## 通信协议

### MCU → 上位机（棋盘状态上报）

```
格式：s<d00><d01><d02><d03><d04><d10>...<d44>e
示例：s0102000000000000000020000e
       │└─25个格子状态（0=空,1=黑,2=白）─┘│
       's'开始                         'e'结束
```

### 上位机 → MCU（走法回传）

```
格式：<x1><y1><x2><y2>（4个ASCII数字字符）
示例：0213  →  白棋从 (2,1) 移动到 (1,3)
```

### 帧超时检测（TIM3）

UART 接收采用 **TIM3 超时机制**判断一帧数据接收完成：
- 首字节到来时启动 TIM3（`ARR=9999, PSC=71` → 周期 10ms）
- 每收到一字节重置计数器 `TIM3->CNT = 0`
- TIM3 溢出时认为帧结束，置位 `Flag = 1` 通知主循环处理

---

## 库与外设驱动

| 库/模块 | 文件 | 说明 |
|---------|------|------|
| **STM32 标准外设库** | `stm32f10x.h`, `stm32f10x_gpio/usart/spi/exti/tim.h` | 官方 HAL 库，配合寄存器直接操作双模式使用 |
| **LCD 驱动** | `src/lcd.c` + `inc/lcd.h` | 正点原子 ALIENTEK TFT LCD 驱动 V3.0，支持多 IC 自动识别 |
| **字体库** | `inc/FONT.H` | ASCII 点阵字体（12×12 / 16×16 / 24×24 像素） |
| **GPIO 控制** | `src/GPIO.c` | LED、数码管、矩阵键盘 GPIO 初始化与控制，支持寄存器直接操作与标准库两种模式（宏 `#define Base_Control / StdLib_Control` 切换） |
| **外部中断** | `src/EXTI.c` | EXTI4/5/6 配置，基于矩阵键盘列中断 |
| **定时器** | `src/TIM.c` | TIM2 / TIM3 / TIM6 基本定时器，支持 ARR+PSC 自由配置 |
| **串口通信** | `src/UART.c` | USART1 初始化，中断接收 + TC 中断发送，200 字节环形缓冲区 |
| **SPI 总线** | `src/SPI.c` | SPI2 主模式，8 MHz（Fclk/2），用于扩展存储器 |
| **IIC 总线** | `src/IIC.c` | I2C 软/硬件实现（触摸控制器通信） |
| **系统配置** | `src/sys.c` + `inc/sys.h` | 位带操作宏、位寻址封装、延时函数 |

### 定时器配置说明

```c
// TIM3：UART 帧超时定时器（主循环启动，接收完帧后自动关闭）
Base_Timer3_Config(10000 - 1, 72 - 1);
// → 周期 = (10000 × 72) / 72MHz = 10ms

// 可扩展：TIM2 / TIM6 用于其他周期任务
Base_Timer2_Config(arr, psc);   // 通用定时
Base_Timer6_Config(arr, psc);   // 基本定时（无 PWM）
```

---

## 项目文件结构

```
embedding/
├── flowlight.uvprojx          # Keil MDK 工程文件
├── flowlight.uvoptx           # 工程选项文件
├── src/                       # 应用源文件
│   ├── main.c                 # 主程序：游戏逻辑、棋盘绘制
│   ├── lcd.c                  # TFT LCD 驱动（多 IC 支持）
│   ├── GPIO.c                 # GPIO 初始化：LED、键盘、数码管
│   ├── UART.c                 # USART1 串口驱动与中断处理
│   ├── TIM.c                  # 基本定时器 TIM2/3/6 配置
│   ├── EXTI.c                 # 外部中断 EXTI4/5/6 配置
│   ├── SPI.c                  # SPI2 驱动（扩展 Flash）
│   ├── IIC.c                  # I2C 驱动（触摸控制器）
│   ├── sys.c                  # 系统初始化与延时
│   └── key.c                  # 按键扫描（保留）
├── inc/                       # 头文件
│   ├── lcd.h                  # LCD 宏定义、引脚定义、函数声明
│   ├── sys.h                  # 位带宏、类型定义
│   ├── FONT.H                 # ASCII 点阵字体数据
│   ├── IIC.h                  # IIC 接口声明
│   ├── main.h                 # 全局声明
│   ├── KEY.h                  # 按键头文件
│   └── LED.h                  # LED 头文件
├── driver/                    # STM32 标准外设库
│   ├── src/                   # 外设库源文件（GPIO、USART、SPI 等）
│   └── inc/                   # 外设库头文件
├── startup/                   # 启动文件（汇编）
├── exti.c                     # 根目录外部中断（备用）
└── Objects/                   # 编译输出目录
```

---

## 构建与烧录

1. 使用 **Keil MDK 5**（或兼容版本）打开 `flowlight.uvprojx`
2. 选择目标 `flowlight_Target 1`
3. 点击 **Build**（F7）进行编译
4. 通过 **ST-Link / J-Link** 连接开发板，点击 **Download**（F8）烧录
5. 上电后 LCD 自动初始化并显示棋盘界面

> **注意**：若需使用 Computer 模式（联机 AI），请确保上位机通过 **RS232 转 USB 串口** 连接开发板，并运行配套的上位机 AI 程序，波特率配置为 **115200 bps**。

---

*基于 STMicroelectronics 固件库 V3.0.0 | ALIENTEK TFT-LCD 驱动 V3.0 | 正点原子开发板*
