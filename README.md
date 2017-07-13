# OSLabs
<h2>南京大学操作系统课程实验/OperatingSystemLabs_NJU<br></br></h2>
<h3>实验网站 Website：http://dislab.nju.edu.cn/taohuang/</h3>
<h3>实验内容 Content</h3>
<ul><li>Lab1实验要求<ul>
<li>1.1. 在实模式下实现一个Hello World程序</li>
<li>1.2. 在保护模式下实现一个Hello World程序</li>
<li>1.3. 在保护模式下加载磁盘中的Hello World程序运行</li>
</ul></li></ul>
<ul><li>Lab2实验要求<ul>
<li>2.1. 实现系统调用库函数<code>printf</code></li>
<li>2.2. 完善<code>printf</code>的格式化输出</li>
</ul></li></ul>
<ul><li>Lab3实验要求<ul>
<li>3.1. 实现进程切换机制</li>
<li>3.2. 实现<code>FORK</code>、<code>SLEEP</code>、<code>EXIT</code>系统调用</a></li>
</ul></li></ul>
<ul><li>Lab4实验要求<ul>
<li>4.1. 实现<code>SEM_INIT</code>、<code>SEM_POST</code>、<code>SEM_WAIT</code>、<code>SEM_DESTROY</code>系统调用</a></li>
</ul></li></ul>
<h3>实验环境 Environment：Ubuntu + QEMU</h3>
<h3>实验框架 Frame</h3>
<pre><code>+lab1
|---+bootloader
|   |---boot.h                          #磁盘I/O接口
|   |---boot.c                          #加载磁盘上的用户程序
|   |---start.s                         #引导程序
|   |---Makefile
|---+utils
|   |---genboot.pl                      #生成MBR
|---+app
|   |---app.s                           #用户程序
|   |---Makefile
|---Makefile</code></pre>
<pre><code>+Lab2,3,4
|---+bootloader             #引导程序
|   |---...
|---+utils
|   |---genBoot.pl          #生成引导程序
|   |---genKernel.pl        #生成内核程序
|---+kernel
|   |---+include            #头文件
|   |---+kernel             #内核代码
|   |   |---doIrq.S         #中断处理
|   |   |---i8259.c         #重设主从8259A
|   |   |---idt.c           #初始化中断描述表
|   |   |---irqHandle.c     #中断处理函数
|   |   |---kvm.c           #初始化 GDT 和加载用户程序
|   |   |---serial.c        #初始化串口输出
|   |---+lib
|   |---main.c              #主函数
|   |---Makefile
|---+app                    #用户代码
|   |---main.c              #主函数
|   |---Makefile
|---+lib                    #库函数
|   |---lib.h
|   |---types.h
|   |---syscall.c           #系统调用入口
|---Makefile</code></pre>
