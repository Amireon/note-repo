## JVM的整体结构
![](https://raw.githubusercontent.com/Anlieh/PicBucket/master/202210161837496.png)


## JVM的结构模型
Java 编译器输入的指令流基木上是一种基于**栈的指令集架构**，另外一种指令集架构则是基于**寄存器的指令集架构**，这两种架构之间的区别:

基于栈式架构的特点
- 设计和实现更简单，适用于资源受限的系统
- 避开了寄存器的分配难题：使用零地址:指令方式分配
- 指令流中的指令大部分是零地址指令，其执行过程依赖于操作枝。指令集更小，编译器容易实现。
- 不需要硬件支持，可移植性更好，更好实现跨平台

基于寄存器架构的特点
- 典型的应用是x86的二讲制指令集：比如传统的 PC 以及 Android 的 Davlik 虚拟机
- 指令集架构则完全依赖硬件，可移植性差
- 性能优秀和执行更高效
- 花费更少的指令去完成一项操作
- 在大部分情况下，基于寄存器架构的指令集往往都以一地力指令、二地址指令和三地址指令为主，而基于栈式架构的指令集却是以零地址指令为主。

**举例**
举例1:
同样执行2+3这种逻辑操作，其指令分别如下
基于栈的计算流程（以Java虚拟机为例）
```
iconst_2 //常量2入栈
1store_1
iconst_3 //常量3入核
1store_2
iload_1
fload_2
iadd     //常量2、3出栈，执行相加
istore_0 //结束
```

而基于寄存器的计算流程
```
mov eax, 2 //将 eax 寄存器的值设为1
add eax, 3 //将 eax 寄存器的值加3
```

**总结**
<font color="red">由于跨平台性的设计，Jav a的指令都是根据栈来设计的。</font>不同平台CPU架构不同，所以不能设计为基于寄存器的。优点是跨平台，指令集小，编译器容易实现，缺点是性能下降，实现同样的功能需要更多的指令。
时至今日，尽管嵌入式平台已经不是Java程序的主流运行平台了（准确来说应该是 HotSpotVM 的宿主环境己经不局限于嵌入式平台了），那么为什么不更换为基于寄存器的架构呢？

<font color="green">栈：跨平台，指令集小、指令多、执行性能比寄存器差</font>

## JVM的生命周期
**虚拟机的启动**
Java虚拟机的启动是通过引导类加载器(bootstrap class loader)创建一个初始类(initial class）来完成的，这个类是由虚拟机的具体实现指定的

**虚拟机的执行**
- 一个运行中的 Java 虚拟机有着一个清晰的任务：执行 Java 程序。
- 程序开始执行时他才运行，程序结束时他就停止。
- <font color="red">执行一个所谓的Java程序的时候，真正执行的是一个叫做 Java 虚拟机的进程。</font>

**虚拟机的退出**
有如下的几种情况:
- 程序正常执行结束
- 程序在执行过程中遇到了异常或错误而异常终止
- 由于操作系统出现错误而导致Java虚拟机进程终止
- 某线程调用 Runtime 类或 System 类的exit方法，或 Runtime 类的halt方法，并且Java安全管理器也允许这次exit或halt操作
除此之外，JNI（ Java Native Interface）规范描述了用JNI Invocation API来加载或卸载 Java虚拟机时，Java虚拟机的退出情况


## JVM发展历程
### Sun Classic VM
1.  早在1996年Java1.0版本的时候，Sun公司发布了一款名为sun classic VM的Java虚拟机，它同时也是**世界上第一款商用Java虚拟机**，JDK1.4时完全被淘汰。
2.  这款虚拟机内部只提供解释器，没有即时编译器，因此效率比较低。【即时编译器会把热点代码的本地机器指令缓存起来，那么以后使用热点代码的时候，效率就比较高】
3.  如果使用JIT编译器，就需要进行外挂。但是一旦使用了JIT编译器，JIT就会接管虚拟机的执行系统。解释器就不再工作，解释器和编译器不能配合工作。
    -   我们将字节码指令翻译成机器指令也是需要花时间的，如果只使用JIT，就需要把所有字节码指令都翻译成机器指令，就会导致翻译时间过长，也就是说在程序刚启动的时候，等待时间会很长。
    -   而解释器就是走到哪，解释到哪。
4.  现在Hotspot内置了此虚拟机。

### Exact VM
1.  为了解决上一个虚拟机问题，jdk1.2时，Sun提供了此虚拟机。
2.  Exact Memory Management：准确式内存管理
    -   也可以叫Non-Conservative/Accurate Memory Management
    -   虚拟机可以知道内存中某个位置的数据具体是什么类型。
3.  具备现代高性能虚拟机的维形
    -   热点探测（寻找出热点代码进行缓存）
    -   编译器与解释器混合工作模式
4.  只在Solaris平台短暂使用，其他平台上还是classic vm，英雄气短，终被Hotspot虚拟机替换

### HotSpot VM（重点）
1.  HotSpot历史
    -   最初由一家名为“Longview Technologies”的小公司设计
    -   1997年，此公司被Sun收购；2009年，Sun公司被甲骨文收购。
    -   JDK1.3时，HotSpot VM成为默认虚拟机
2.  目前**Hotspot占有绝对的市场地位，称霸武林**。
    -   不管是现在仍在广泛使用的JDK6，还是使用比例较多的JDK8中，默认的虚拟机都是HotSpot
    -   Sun/oracle JDK和openJDK的默认虚拟机
    -   因此本课程中默认介绍的虚拟机都是HotSpot，相关机制也主要是指HotSpot的GC机制。（比如其他两个商用虚机都没有方法区的概念）
3.  从服务器、桌面到移动端、嵌入式都有应用。
4.  名称中的HotSpot指的就是它的热点代码探测技术。
    -   通过计数器找到最具编译价值代码，触发即时编译或栈上替换
    -   通过编译器与解释器协同工作，在最优化的程序响应时间与最佳执行性能中取得平衡

### JRockit（商用三大虚拟机之一）
1.  专注于服务器端应用：它可以不太关注程序启动速度，因此JRockit内部不包含解析器实现，全部代码都靠即时编译器编译后执行。
2.  大量的行业基准测试显示，JRockit JVM是世界上最快的JVM：使用JRockit产品，客户已经体验到了显著的性能提高（一些超过了70%）和硬件成本的减少（达50%）。
3.  优势：全面的Java运行时解决方案组合
    -   JRockit面向延迟敏感型应用的解决方案JRockit Real Time提供以毫秒或微秒级的JVM响应时间，适合财务、军事指挥、电信网络的需要
    -   Mission Control服务套件，它是一组以极低的开销来监控、管理和分析生产环境中的应用程序的工具。
4.  2008年，JRockit被Oracle收购。
5.  Oracle表达了整合两大优秀虚拟机的工作，大致在JDK8中完成。整合的方式是在HotSpot的基础上，移植JRockit的优秀特性。
6.  高斯林：目前就职于谷歌，研究人工智能和水下机器人

### IBM的J9（商用三大虚拟机之一）
1.  全称：IBM Technology for Java Virtual Machine，简称IT4J，内部代号：J9
2.  市场定位与HotSpot接近，服务器端、桌面应用、嵌入式等多用途VM广泛用于IBM的各种Java产品。
3.  目前，有影响力的三大商用虚拟机之一，也号称是世界上最快的Java虚拟机。
4.  2017年左右，IBM发布了开源J9VM，命名为openJ9，交给Eclipse基金会管理，也称为Eclipse OpenJ9
5.  OpenJDK -> 是JDK开源了，包括了虚拟机

### KVM和CDC/CLDC Hotspot
1.  Oracle在Java ME产品线上的两款虚拟机为：CDC/CLDC HotSpot Implementation VM    
2.  KVM（Kilobyte）是CLDC-HI早期产品    
3.  目前移动领域地位尴尬，智能机被Android和iOS二分天下。    
4.  KVM简单、轻量、高度可移植，面向更低端的设备上还维持自己的一片市场    
    -   智能控制器、传感器
    -   老人手机、经济欠发达地区的功能手机
5.  所有的虚拟机的原则：一次编译，到处运行。

### Azul VM
1.  前面三大“高性能Java虚拟机”使用在**通用硬件平台上**
2.  这里Azul VW和BEA Liquid VM是与**特定硬件平台绑定**、软硬件配合的专有虚拟机：高性能Java虚拟机中的战斗机。
3.  Azul VM是Azul Systems公司在HotSpot基础上进行大量改进，运行于Azul Systems公司的专有硬件Vega系统上的Java虚拟机。
4.  每个Azul VM实例都可以管理至少数十个CPU和数百GB内存的硬件资源，并提供在巨大内存范围内实现可控的GC时间的垃圾收集器、专有硬件优化的线程调度等优秀特性。
5.  2010年，Azul Systems公司开始从硬件转向软件，发布了自己的Zing JVM，可以在通用x86平台上提供接近于Vega系统的特性。

### Liquid VM
1.  高性能Java虚拟机中的战斗机。
2.  BEA公司开发的，直接运行在自家Hypervisor系统上
3.  Liquid VM即是现在的JRockit VE（Virtual Edition）。**Liquid VM不需要操作系统的支持，或者说它自己本身实现了一个专用操作系统的必要功能，如线程调度、文件系统、网络支持等**。
4.  随着JRockit虚拟机终止开发，Liquid vM项目也停止了。

### Apache Marmony
1.  Apache也曾经推出过与JDK1.5和JDK1.6兼容的Java运行平台Apache Harmony。
2.  它是IElf和Intel联合开发的开源JVM，受到同样开源的Open JDK的压制，Sun坚决不让Harmony获得JCP认证，最终于2011年退役，IBM转而参与OpenJDK
3.  虽然目前并没有Apache Harmony被大规模商用的案例，但是它的Java类库代码吸纳进了Android SDK。

### Micorsoft JVM
1.  微软为了在IE3浏览器中支持Java Applets，开发了Microsoft JVM。
2.  只能在window平台下运行。但确是当时Windows下性能最好的Java VM。
3.  1997年，Sun以侵犯商标、不正当竞争罪名指控微软成功，赔了Sun很多钱。微软WindowsXP SP3中抹掉了其VM。现在Windows上安装的jdk都是HotSpot。

### Taobao JVM
1.  由AliJVM团队发布，阿里覆盖云计算、金融、物流、电商等众多领域，需要解决高并发、高可用、分布式的复合问题，有大量的开源产品。
2.  **基于OpenJDK开发了定制版本AlibabaJDK**，简称AJDK，是整个阿里Java体系的基石。
3.  基于OpenJDK Hotspot VM发布的国内第一个优化、深度定制且开源的高性能服务器版Java虚拟机。
    -   创新的GCIH（GCinvisible heap）技术实现了off-heap，**即将生命周期较长的Java对象从heap中移到heap之外，并且GC不能管理GCIH内部的Java对象，以此达到降低GC的回收频率和提升GC的回收效率的目的**。
    -   GCIH中的**对象还能够在多个Java虚拟机进程中实现共享**
    -   使用crc32指令实现JvM intrinsic降低JNI的调用开销
    -   PMU hardware的Java profiling tool和诊断协助功能
    -   针对大数据场景的ZenGC
4.  Taobao JVM 应用在阿里产品上性能高，**硬件严重依赖inte1的cpu，损失了兼容性，但提高了性能**
-   目前已经在淘宝、天猫上线，把Oracle官方JVM版本全部替换了。

###  Dalvik VM
1.  谷歌开发的，应用于Android系统，并在Android2.2中提供了JIT，发展迅猛。
2.  **Dalvik VM 只能称作虚拟机，而不能称作“Java 虚拟机”**，它没有遵循 Java虚拟机规范
3.  不能直接执行 Java 的 Class 文件
4.  基于寄存器架构，不是 JVM 的栈架构。
5.  执行的是编译以后的dex（Dalvik Executable）文件。执行效率比较高。
6. 它执行的dex（Dalvik Executable）文件可以通过class文件转化而来，使用Java语法编写应用程序，可以直接使用大部分的Java API等。
7.  Android 5.0 使用支持提前编译（Ahead of Time Compilation，AoT）的ART VM替换Dalvik VM。

### Graal VM（未来虚拟机）
1.  2018年4月，Oracle Labs公开了GraalvM，号称 “**Run Programs Faster Anywhere**”，勃勃野心。与1995年java的”write once，run anywhere”遥相呼应。
2.  GraalVM在HotSpot VM基础上增强而成的**跨语言全栈虚拟机，可以作为“任何语言”**的运行平台使用。语言包括：Java、Scala、Groovy、Kotlin；C、C++、Javascript、Ruby、Python、R等
3.  支持不同语言中混用对方的接口和对象，支持这些语言使用已经编写好的本地库文件
4.  工作原理是将这些语言的源代码或源代码编译后的中间格式，通过解释器转换为能被Graal VM接受的中间表示。Graal VM提供Truffle工具集快速构建面向一种新语言的解释器。在运行时还能进行即时编译优化，获得比原生编译器更优秀的执行效率。
5.  **如果说HotSpot有一天真的被取代，Graalvm希望最大**。但是Java的软件生态没有丝毫变化。
 