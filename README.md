# HL10-SDK

## 目录结构

app: 公用应用程序文件，系统启动入口，主要适配不同MCU和不同的RTOS启动

bsp：主板驱动支持文件，放置各种MCU的原始驱动库，及BSP中间层各种接口，如I2C、SPI、RTC、Sleep、ADC、PWM等

kernel：内核相关文件，主要包含通用CMSIS接口和放置各种原始RTOS系统源码

libs：通用库文件，如独立的AT指令集、射频驱动、常用函数工具、FIFO、AES、OTA、加密、CRC(ModBus)

net：网络相关，如LoRa、GPRS、NB-IoT、短信、4G等接口定义和MAC协议

projects: 工程相关文件，可放置各种主板、定制化工程

## 开发环境
最新版本SDK支持Keil和IAR两种编译环境。IAR采用7.7版本，Keil采用5.25，其它版本环境未有时间验证，有更高版本软件请自行移植适配。

相关软件请自行在官网下载和安装：
IAR：https://www.iar.com/iar-embedded-workbench
Keil: http://www.keil.com/

仿真器可采用J-Link仿真或IAR支持的相关仿真器，采用SWD接口。

在编译之前请先安装MCU支持包，可从www.hdsc.com.cn官网自行下载，或咨询公司销售和技术支持。

## 调式说明
本评估板使用MCU仿真的话，需要注意两点。
1. MCU深度休眠时无法使用SWD调式，需要复位芯片以恢复SWD调式口功能进行程序仿真。
2. MCU启动支持BOOT选择开关，对应的端口是PD03，PD03低电平则为运行模式（可仿真调试），高电平为ISP烧录模式。

因此，建议在开发休眠功能之前，确保系统重启后能够仿真烧录（如果是运行即休眠业务逻辑，建议在调式阶段，启动后增加几秒的延时用于调试开发使用）

## 工程说明
例程经过很好的代码封装，模块化耦合度低，main文件为主程序入口。

上述代码中：
采用RTX系统多任务处理，除主任务外，分别AT Task（app_at.c）和Mac Task(app_mac.c)分别处理AT指令和无线收发。
platform为HL9、HL10相关外设操作。

## 二次开发参考
SDK包中集成了
1. AT指令集
2. AT模式软硬件切换方式
3. LoRa无线自动收发操作
4. 休眠无线唤醒
5. 低功耗串口自动唤醒
6. AES 128bit加密
7. Rejeee LPWAN协议
8. ADC采集示例
9. RTX系统接口，方便开发多任务操作

用户可以根据需要增删功能。

具体AT操作，请参考 Rejeee AT指令手册，开发文档手册在目录docs中。

## 修改记录

