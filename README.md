# ICS2022 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to https://nju-projectn.github.io/ics-pa-gitbook/ics2022/

To initialize, run
```bash
bash init.sh subproject-name
```
See `init.sh` for more details.

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Abstract-Machine](https://github.com/NJU-ProjectN/abstract-machine)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)


## 编译过程
初始化项目
git branch -m master
bash init.sh nemu
bash init.sh abstract-machine

刷新环境变量
source ~/.bashrc

配置nemu
cd nemu
make menuconfig
编译
make

运行
make run

如果缺少库安装
最后来到配置弹窗配置ISA架构.
缺少高版本llvm手动编译之后安装, 然后配置llvm-config,自动查找相关路径

在编译过程发现编译flag缺少如下
#add by ljm 
CFLAGS += -fPIC
CXXFLAGS += -fPIC
LDFLAGS += -L/usr/lib/x86_64-linux-gnu -ltinfo -lpthread -lz
#方便调试错误 LDFLAGS += -v


