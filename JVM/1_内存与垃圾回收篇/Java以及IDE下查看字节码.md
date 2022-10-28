Java以及IDEA下查看字节码的五种方法
>原文链接：[https://blog.csdn.net/21aspnet/article/details/88351875](https://blog.csdn.net/21aspnet/article/details/88351875)

## 1.最本质的是cmd下
```sh
javap -v 类名
```
说明：这是最原始的方法，效率低下，强烈不推荐使用。

## 2.IDEA集成命令行
> IDEA 里扩展工具快捷方式

在 tool setting 的 Program 输入工具的路径，这中间可以插入宏，比如`$JDK_PATH$`，不需要自己再手动输入jdk的路径了。  
在参数栏中输入 `-p $FileClass$` ，`$FileClass$` 代表要解析的 class文件。  
在Workding Directory中输入 `$OutputPath$` 代表项目的输出路径
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202210231620510.png)

## 3.使用IDEA自带插件
这种方法比较简单，直接view菜单下就有，不过信息不全。

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202210231622660.png)

## 4.jclasslib插件
jclasslib插件还是很方便的，setting里安装插件就行。
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202210231623420.png)

测试:
```java
public class TestApp {
    public static void main(String[] args) {
        int a = 0;
        a = a++;
        System.out.println(a);
        a = ++a;
        System.out.println(a);
        a += 2;
        System.out.println(a);
 
    }
}
```

输出：
```
0
1
3
```

字节码：
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202210231625375.png)

本地变量表：
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202210231625733.png)
