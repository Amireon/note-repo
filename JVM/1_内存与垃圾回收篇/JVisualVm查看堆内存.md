使用JDK自带工具，可视化查看堆内存空间，使用方法如下：
1. 运行`jdk/bin`目录下的 jvisualvm.exe 文件，
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202211072005069.png)

2. 安装插件：工具 -> 插件 -> 安装Visual GC插件
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202211072007761.png)

3. 运行如下代码：
```java
public class HeapDemo {  
    public static void main(String[] args) {  
        System.out.println("start...");  
        try {  
            TimeUnit.MINUTES.sleep(30);  
        } catch (InterruptedException e) {  
            e.printStackTrace();  
        }  
  
        System.out.println("end...");  
    }  
}
```

t
