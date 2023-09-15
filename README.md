# bluetooth-car_Stm32f103C8t6
这是一个基于 stm32f103c8t6 和 uniapp 的物联网蓝牙小车项目，本项目主要包含两大功能，一个是蓝牙控制小车运动，一个是避障小车模式。

stm32 单片机➕uniapp 项目，蓝牙/避障小车，项目用到的模块有 stm32f103c8t6 最小系统板，hc-08 蓝牙模块，两个 tb6612 电机驱动模块，hc-sr04 超声波模块，0.96 寸 oled 显示屏，一个舵机，4 个直流电机。

stm32单片机项目可以使用Keil打开，因为使用了freertos系统，所以启动文件要改成自己的，并且Device要选择自己的板子型号。

https://github.com/liumubo/bluetooth-car_frontEnd/assets/114791868/f7ed1c30-f509-468a-990e-aec4baf6f01d
