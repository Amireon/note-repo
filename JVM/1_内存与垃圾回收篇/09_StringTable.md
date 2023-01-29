## 1.String的基本特性
- String：字符串，使用一对`“”`引起来表示，
- String被声明为final的，不可被继承
- String实现了Serializable接口：表示字符串是支持序列化的。实现了Comparable接口：表示String可以比较大小
- String在jdk8及以前内部定义了`final char value[]`用于存储字符串数据。JDK9时改为`byte[]`

### 1.1 JDK9 改变了 String 的结构

> **官方文档**：[http://openjdk.java.net/jeps/254](http://openjdk.java.net/jeps/254)

**为什么改为 byte[] 存储？**
1. String类的当前实现将字符存储在char数组中，每个字符使用两个字节(16位)。
2. 从许多不同的应用程序收集的数据表明，字符串是堆使用的主要组成部分，而且大多数字符串对象只包含拉丁字符（Latin-1）。这些字符只需要一个字节的存储空间，因此这些字符串对象的内部char数组中有一半的空间将不会使用，产生了大量浪费。
3. 之前 String 类使用 UTF-16 的 char[] 数组存储，现在改为 byte[] 数组 外加一个编码标识存储。该编码表示如果你的字符是ISO-8859-1或者Latin-1，那么只需要一个字节存。如果你是其它字符集，比如UTF-8，你仍然用两个字节存
4. 结论：String再也不用char[] 来存储了，改成了byte [] 加上编码标记，节约了一些空间
5. 同时基于String的数据结构，例如StringBuffer和StringBuilder也同样做了修改


###  1.2 String的不可变性
String：代表不可变的字符序列。简称：不可变性，解释如下
- 当对字符串重新赋值时，需要重写指定内存区域赋值，不能使用原有的value进行赋值。
- 当对现有的字符串进行连接操作时，也需要重新指定内存区域赋值，不能使用原有的value进行赋值。
- 当调用String的replace()方法修改指定字符或字符串时，也需要重新指定内存区域赋值，不能使用原有的value进行赋值。

- 通过字面量的方式（区别于new）给一个字符串赋值，此时的字符串值声明在字符串常量池中。

一道小题目
```java
public class StringExec {  
//    String str = new String("good");  
    String str = "good";  
    char[] ch = {'t', 'e', 's', 't'};  
  
    public void change(String s, char[] ch){  
        s = "test ok";  
        ch[0] = 'b';  
    }  
  
    public static void main(String[] args) {  
        StringExec ex = new StringExec();  
        ex.change(ex.str, ex.ch);  
        System.out.println(ex.str);  //good
        System.out.println(ex.ch);   //best
    }  
  
}
```

原因: 尽管传进的str是引用，但在字符串常量池中"good"并未消失，只是新建了"test ok"，并把其地址赋给了s，即只是改变了s的指向

### 1.3 StringTable
**字符串常量池是不会存储相同内容的字符串的**
- String的String Pool（字符串常量池）是一个固定大小的Hashtable，默认值大小长度是1009。如果放进String Pool的String非常多，就会造成Hash冲突严重，从而导致链表会很长，而链表长了后直接会造成的影响就是当调用String.intern()方法时性能会大幅下降。
- 使用`-XX:StringTablesize`可设置StringTable的长度
- 在JDK6中StringTable是固定的，就是1009的长度，所以如果常量池中的字符串过多就会导致效率下降很快，StringTablesize设置没有要求
- 在JDK7中，StringTable的长度默认值是60013，StringTablesize设置没有要求
- 在JDK8中，StringTable的长度默认值是60013，StringTable可以设置的最小值为1009


## 2.String的内存分配
- 在Java语言中有8种基本数据类型和一种比较特殊的类型String。这些类型为了使它们在运行过程中速度更快、更节省内存，都提供了一种常量池的概念。
- 常量池就类似一个Java系统级别提供的缓存。8种基本数据类型的常量池都是系统协调的，String类型的常量池比较特殊。它的主要使用方法有两种。
    - 直接使用双引号声明出来的String对象会直接存储在常量池中。比如：`String info="hello";`
    - 如果不是用双引号声明的String对象，可以使用String提供的intern()方法。这个后面重点谈

**字符串常量池的位置变更**
| 版本        | 位置   |
| ----------- | ------ |
| Java6及之前 | 永久代 |
| Java7       | Java堆 |
| Java8       | 堆     |

补充: 详情见方法区
- Java7中：字符串常量池概念原本使用得比较多，但是这个改动使得我们有足够的理由让我们重新考虑在Java 7中使用String.intern()。
- Java8使用元空间替代永久代，字符串常量在堆

> **官方文档**:[https://www.oracle.com/java/technologies/javase/jdk7-relnotes.html#jdk7changes](https://www.oracle.com/java/technologies/javase/jdk7-relnotes.html#jdk7changes)

**StringTable为什么要调整位置？**
1. 永久代的默认空间大小比较小
2. 永久代垃圾回收频率低，大量的字符串无法及时回收，容易进行Full GC产生STW或者容易产生OOM：PermGen Space
3. 堆中空间足够大，字符串可被及时回收

补充
- 在JDK 7中，interned字符串不再在Java堆的永久代中分配，而是在Java堆的主要部分（称为年轻代和年老代）中分配，与应用程序创建的其他对象一起分配。
- 此更改将导致驻留在主Java堆中的数据更多，驻留在永久生成中的数据更少，因此可能需要调整堆大小。

```java
/**  
 * jdk6中：  
 * -XX:PermSize=6m -XX:MaxPermSize=6m -Xms6m -Xmx6m  
 *  
 * jdk8中：  
 * -XX:MetaspaceSize=6m -XX:MaxMetaspaceSize=6m -Xms6m -Xmx6m  
 */  
public class StringTest3 {  
    public static void main(String[] args) {  
        //使用Set保持着常量池引用，避免full gc回收常量池行为  
        Set<String> set = new HashSet<String>();  
        //在short可以取值的范围内足以让6MB的PermSize或heap产生OOM了。  
        short i = 0;  
        while(true){  
            set.add(String.valueOf(i++).intern());  
        }  
    }  
}
```

**输出结果**: JDK8，"Java heap space"，字符串常量池真在堆里

```java
Exception in thread "main" java.lang.OutOfMemoryError: Java heap space  
	at java.util.HashMap.resize(HashMap.java:703)  
	at java.util.HashMap.putVal(HashMap.java:662)  
	at java.util.HashMap.put(HashMap.java:611)  
	at java.util.HashSet.add(HashSet.java:219)  
	at com.atguigu.java.StringTest3.main(StringTest3.java:22)  
  
Process finished with exit code 1
```


## 3.String的基本操作
Java语言规范里要求完全相同的字符串字面量，应同时满足
- 包含同样的Unicode字符序列（包含同一份码点序列的常量），
- 必须是指向同一个String类实例。

### 例3.1
```java
public class StringTest4 {  
    public static void main(String[] args) {  
        System.out.println();//2293  
        System.out.println("1");//2294  
        System.out.println("2");  
        System.out.println("3");  
        System.out.println("4");  
        System.out.println("5");  
        System.out.println("6");  
        System.out.println("7");  
        System.out.println("8");  
        System.out.println("9");  
        System.out.println("10");//2303  
        //如下的字符串"1" 到 "10"不会再次加载  
        System.out.println("1");//2304  
        System.out.println("2");//2304  
        System.out.println("3");  
        System.out.println("4");  
        System.out.println("5");  
        System.out.println("6");  
        System.out.println("7");  
        System.out.println("8");  
        System.out.println("9");  
        System.out.println("10");//2304  
    }  
}
```

Debug分析字符串常量池的变化

1.程序启动时已经加载了 2293 个字符串常量
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291557328.png)

2.加载了一个换行符（println），所以多了一个
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291558609.png)

3.加载了字符'1'至字符'9'
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291559068.png)

4.第二轮的'1'到'10'不会再次被加载
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291559527.png)

### 例3.2
官方示例代码 

```java
class Memory {  
    public static void main(String[] args) {//line 1  
        int i = 1;//line 2  
        Object obj = new Object();//line 3  
        Memory mem = new Memory();//line 4  
        mem.foo(obj);//line 5  
    }//line 9  
  
    private void foo(Object param) {//line 6  
        String str = param.toString();//line 7  
        System.out.println(str);  
    }//line 8  
}
```

附加知识：非静态方法中，第一个参数是'this'

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291605704.png)


## 4.字符串拼接操作
### 4.1 拼接结论
1. 常量与常量的拼接结果在常量池，原理是编译期优化
2. 常量池中不会存在相同内容的变量
3. 拼接前后，只要其中有一个是变量，结果就在堆中(不是指StringTable，是堆剩余部分）。变量拼接的原理是StringBuilder
4. 如果拼接的结果调用intern()方法，根据该字符串是否在常量池中存在，分为：
	- 如果存在，则返回字符串在常量池中的地址
	- 如果不存在，则在常量池中创建一份，并返回此对象的地址

**1.常量与常量拼接结果在常量池**

```java
@Test  
    public void test1(){  
        String s1 = "a" + "b" + "c";//编译期优化：等同于"abc"  
        String s2 = "abc"; //"abc"一定是放在字符串常量池中，将此地址赋给s2  
        /*  
         * 最终.java编译成.class,再执行.class  
         * String s1 = "abc";  
         * String s2 = "abc"  
         */  
        System.out.println(s1 == s2); //true  
        System.out.println(s1.equals(s2)); //true  
    }
```

从字节码指令看出：编译器做了优化，将 “a” + “b” + “c” 优化成了 “abc”

```java
0 ldc #2 <abc>  
2 astore_1  
3 ldc #2 <abc>  
```

**后三条**

```java
@Test  
    public void test2(){  
        String s1 = "javaEE";  
        String s2 = "hadoop";  
  
        String s3 = "javaEEhadoop";  
        String s4 = "javaEE" + "hadoop";//编译期优化  
        //如果拼接符号的前后出现了变量，则相当于在堆空间中new String()，具体的内容为拼接的结果：javaEEhadoop  
        String s5 = s1 + "hadoop";  
        String s6 = "javaEE" + s2;  
        String s7 = s1 + s2;  
  
        System.out.println(s3 == s4);//true  
        System.out.println(s3 == s5);//false  
        System.out.println(s3 == s6);//false  
        System.out.println(s3 == s7);//false  
        System.out.println(s5 == s6);//false  
        System.out.println(s5 == s7);//false  
        System.out.println(s6 == s7);//false  
        //intern():判断字符串常量池中是否存在javaEEhadoop值，如果存在，则返回常量池中javaEEhadoop的地址；  
        //如果字符串常量池中不存在javaEEhadoop，则在常量池中加载一份javaEEhadoop，并返回次对象的地址。  
        String s8 = s6.intern();  
        System.out.println(s3 == s8);//true  
    }
```

查看字节码文件: 凡是涉及到变量的字符串拼接，都会调用StringBuilder()方法

```java
 13 new #9 <java/lang/StringBuilder>
 16 dup
 17 invokespecial #10 <java/lang/StringBuilder.<init>>
 20 aload_1
 21 invokevirtual #11 <java/lang/StringBuilder.append>
 24 ldc #7 <hadoop>
 26 invokevirtual #11 <java/lang/StringBuilder.append>
 29 invokevirtual #12 <java/lang/StringBuilder.toString>
 32 astore 5
 34 new #9 <java/lang/StringBuilder>
 37 dup
 38 invokespecial #10 <java/lang/StringBuilder.<init>>
 41 ldc #6 <javaEE>
 43 invokevirtual #11 <java/lang/StringBuilder.append>
 46 aload_2
 47 invokevirtual #11 <java/lang/StringBuilder.append>
 50 invokevirtual #12 <java/lang/StringBuilder.toString>
 53 astore 6
 55 new #9 <java/lang/StringBuilder>
 58 dup
 59 invokespecial #10 <java/lang/StringBuilder.<init>>
 62 aload_1
 63 invokevirtual #11 <java/lang/StringBuilder.append>
 66 aload_2
 67 invokevirtual #11 <java/lang/StringBuilder.append>
 70 invokevirtual #12 <java/lang/StringBuilder.toString>
```


### 4.2 底层细节
**例1 StringBuilder**

```java
@Test  
public void test3(){  
    String s1 = "a";  
    String s2 = "b";  
    String s3 = "ab";  
    String s4 = s1 + s2;//  
    System.out.println(s3 == s4);//false  
}
```

如下是 `s1 + s2`的执行细节: 使用StringBuilder

```java
StringBuilder s = new StringBuilder();
s.append("a");
s.append("b");
s.toString(); // 约等于 new String("ab"), 不等价
```
  
补充：在jdk5.0之后使用的是StringBuilder,在jdk5.0之前使用的是StringBuffer  

字节码文件

```java
 9 new #9 <java/lang/StringBuilder>
12 dup
13 invokespecial #10 <java/lang/StringBuilder.<init>>
16 aload_1
17 invokevirtual #11 <java/lang/StringBuilder.append>
20 aload_2
21 invokevirtual #11 <java/lang/StringBuilder.append>
24 invokevirtual #12 <java/lang/StringBuilder.toString>
27 astore 4
```

**例2 非StringBuilder**
1. 如果拼接符号左右两侧都是字符串常量或常量引用，则不使用StringBuilder, 使用编译期优化
2. 针对于final修饰类、方法、基本数据类型、引用数据类型的量时，建议使用final

```java
 @Test  
    public void test4(){  
        final String s1 = "a";  
        final String s2 = "b";  
        String s3 = "ab";  
        String s4 = s1 + s2;  
        System.out.println(s3 == s4);//true  
    }
```

字节码文件: 可以看到直接取"ab", #16

```java
 0 ldc #14 <a>
 2 astore_1
 3 ldc #15 <b>
 5 astore_2
 6 ldc #16 <ab>
 8 astore_3
 9 ldc #16 <ab>
11 astore 4
```


### 4.3 效率对比
Q: 比较StringBuilder的append()方式与使用String直接拼接的效率
A: 通过StringBuilder的append()的方式添加字符串的效率要远高于使用String的字符串拼接方式

原因分析：
1. StringBuilder的append()的方式：整个过程中只创建一个StringBuilder对象
2. 使用String的字符串拼接方式：
    - 创建过多个StringBuilder和String（调的toString方法）的对象，内存占用更大；
    - 如果进行GC，需要花费额外时间（在拼接的过程中产生的一些中间字符串可能永远也用不到，会产生大量垃圾字符串）。

改进空间：
- 空参构造，`new StringBuilder()`，底层`new char[capacity]`, 若空间不足，需要扩容，
- 如果基本确定要添加的字符串长度不高于某个限定值highLevel的情况下，建议使用构造器实例化：`StringBuilder s = new StringBuilder(highLevel); //new char[highLevel]`,这样可以避免频繁扩容

实验代码

```java
    @Test  
    public void test6(){  
  
        long start = System.currentTimeMillis();  
  
//        method1(100000);//4014  
        method2(100000);//7  
  
        long end = System.currentTimeMillis();  
  
        System.out.println("花费的时间为：" + (end - start));  
    }  
  
    public void method1(int highLevel){  
        String src = "";  
        for(int i = 0;i < highLevel;i++){  
            src = src + "a";//每次循环都会创建一个StringBuilder,String  
        }   
  
    }  
  
    public void method2(int highLevel){  
        //只需要创建一个StringBuilder  
        StringBuilder src = new StringBuilder();  
        for (int i = 0; i < highLevel; i++) {  
            src.append("a");  
        }   
    }
```


## 5.intern()的使用
### 5.1 intern()的说明
```java
public native String intern();
```

1. intern是一个native方法，调用的是底层C的方法
2. 字符串常量池池最初是空的，由String类私有地维护。在调用intern方法时，如果池中已经包含了由equals(object)方法确定的与该字符串内容相等的字符串，则返回池中的字符串地址。否则，该字符串对象将被添加到池中，并返回对该字符串对象的地址。（这是源码里的大概翻译）
3. 如果不是用双引号声明的String对象，可以使用String提供的intern方法：intern方法会从字符串常量池中查询当前字符串是否存在，若不存在就会将当前字符串放入常量池中。比如：`String s = new string("hello").intern();

如果在任意字符串上调用String.intern方法，那么其返回结果所指向的那个类实例，必须和直接以常量形式出现的字符串实例完全相同。因此，下列表达式的值必定是true, `("a"+"b"+"c").intern() == "abc"`

通俗点讲，Interned String就是确保字符串在内存里只有一份拷贝，这样可以节约内存空间，加快字符串操作任务的执行速度。注意，这个值会被存放在字符串内部池（String Intern Pool）

如何保证变量s指向的是字符串常量池中的数据？
- 方式一：字面量定义的方式
```java
String s = "hello";
```
- 方式二：调用intern()
```java
String s = new String("hello").intern();
String s = new StringBuilder("hello").toStrign().intern();
```


**总结String的intern()的使用**
- jdk1.6中，将这个对象尝试放入StrignTable
	- 如果StringTable中有，则不放入，返回串池中已有的对象的地址
	- 如果没有，把此对象复制一份，放入串池，并返回串池中的对象地址
- jdk1.7起，将这个对象尝试放入串池
	- 如果串池中有，则不放入，返回串池中的对象的地址
	- 如果串池中没有，把对象的引用地址复制一份，放入串池，并返回串池中的引用地址(其实返回了自己


### 5.2 new String()的说明
**Q1: new String(“ab”)会创建几个对象？**
查看字节码, 两个，
- 对象1：new关键字在堆空间创建的, 返回这个对象
- 对象2：字符串常量池中的对象"ab"。 字节码指令：ldc  

```java
 0 new #2 <java/lang/String>
 3 dup
 4 ldc #3 <ab>
 6 invokespecial #4 <java/lang/String.<init>>
 9 astore_1
10 return
```

**Q2: new String('a') + new String('b') 会创建几个对象？**
- 对象1: new StringBuilder()
- 对象2: new String()
- 对象3: 常量池中的"a"
- 对象4: new String()
- 对象5: 常量池中的"b"

```java
 0 new #2 <java/lang/StringBuilder>
 3 dup
 4 invokespecial #3 <java/lang/StringBuilder.<init>>
 7 new #4 <java/lang/String>
10 dup
11 ldc #5 <a>
13 invokespecial #6 <java/lang/String.<init>>
16 invokevirtual #7 <java/lang/StringBuilder.append>
19 new #4 <java/lang/String>
22 dup
23 ldc #8 <b>
25 invokespecial #6 <java/lang/String.<init>>
28 invokevirtual #7 <java/lang/StringBuilder.append>
31 invokevirtual #9 <java/lang/StringBuilder.toString>
34 astore_1
35 return
```

但是还没结束，别忘了StringBuilder的toString()
- 对象5: new String("ab"), 但字符串常量池中没有"ab"

```java
 0 new #80 <java/lang/String>
 3 dup
 4 aload_0
 5 getfield #234 <java/lang/StringBuilder.value>
 8 iconst_0
 9 aload_0
10 getfield #233 <java/lang/StringBuilder.count>
13 invokespecial #291 <java/lang/String.<init>>
16 areturn
```


### 5.3 intern()面试难题
**Q1**
```java
String s = new String("1");  
s.intern();  
String s2 = "1";  
System.out.println(s == s2);//jdk6：false   jdk7/8：false  
```

重点: 在调用`s.intern()`之前，字符串常量池中已经存在了"1"
- 因此不会放入"1", s更不会被修改，而s2是字符串常量池中的"1"，
- 那么一个堆中的对象自然不等于一个StringTable中的对象

**Q2**
```java
String s3 = new String("1") + new String("1");  
s3.intern();  
String s4 = "11";  
System.out.println(s3 == s4);//jdk6：false  jdk7/8：true
```

1. s3变量记录的地址为：`new String("11")`  
2. 经过上面的分析，我们已经知道执行完pos_1的代码，在堆中有了一个`new String("11")`这样的String对象。但是在字符串常量池中没有"11"  
3. 接着执行`s3.intern()`，在字符串常量池中生成"11", 而字符串常量池的位置可是不一样的，
4. JDK6，字符串常量池在永久代，所以直接在永久代生成"11",也就有了新的地址, 那s3,s4自然不等 
5. JDK7即之后，字符串常量池被移动到了堆中，此时堆里已经有`new String（"11"）`了。出于节省空间的目的，直接将堆中的那个字符串的引用地址储存在字符串常量池中。没错，字符串常量池中存的是`new String（"11"）`在堆中的地址。所以在JDK7后续版本中，s3和s4指向的完全是同一个地址。


![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291813997.png)

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291813325.png)


### 5.4 intern()方法的练习
**练习1**

```java 
String s = new String("a") + new String("b");  
String s2 = s.intern();  
System.out.println(s2 == "ab");  
System.out.println(s == "ab");
```

输出:
- `s2 == "ab"`: jdk6:true  jdk8:true
- `s == "ab"`: jdk6:false  jdk8:true  

分析: 在第一行代码执行完以后，s等于`new String("ab")`, 字符串常量池中并没有"ab"  
1. jdk6中：在字符串常量池（此时在永久代）中创建一个字符串"ab"  
2. jdk8中：字符串常量池（此时在堆中）中没有创建字符串"ab", 而是创建一个引用，指向`new String("ab")`, 将此引用返回  

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291859710.png)


![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291900394.png)


**练习2**
只考虑jdk7及之后

```java
String s1 = new String("ab");  
s1.intern();  
String s2 = "ab";  
System.out.println(s1 == s2);
```

输出：false
分析: `new String("ab")`执行完以后，会在字符串常量池中会生成"ab"
那么s2指向的便是StringTable中的"ab", 而s1是堆中的"ab"

验证，利用`System.identityHashCode(Object obj)`方法来查看对象地址

```java
System.identityHashCode(s1) // 1327763628
System.identityHashCode(s2) // 1915503092
```

更改

```java
String s1 = new String("a") + new String("b");  
s1.intern();  
String s2 = "ab";  
System.out.println(s1 == s2);
```

输出: true
分析: `new String("a") + new String("b")`执行完以后，不会在字符串常量池中生成"ab", 但堆中存在`new String("ab")`对象
- `s1.intern()` copy 堆中的"ab"对象到StringTable中
- s2为StringTable中的"ab"对象，其实也是s1
- s1 = s2为真

验证

```java
System.identityHashCode(s1) // 1327763628
System.identityHashCode(s2) // 1327763628
```


### 5.5 intern()的空间效率
Q: 使用intern()测试执行效率, 空间使用上  
A: 对于程序中大量存在存在的字符串，尤其其中存在很多重复字符串时，使用intern()可以节省内存空间。  

```java
public class StringIntern2 {  
    static final int MAX_COUNT = 1000 * 10000;  
    static final String[] arr = new String[MAX_COUNT];  
  
    public static void main(String[] args) {  
        Integer[] data = new Integer[]{1,2,3,4,5,6,7,8,9,10};  
  
        long start = System.currentTimeMillis();  
        for (int i = 0; i < MAX_COUNT; i++) {  
//            arr[i] = new String(String.valueOf(data[i % data.length]));  
            arr[i] = new String(String.valueOf(data[i % data.length])).intern();  
  
        }  
        long end = System.currentTimeMillis();  
        System.out.println("花费的时间为：" + (end - start));  
  
        try {  
            Thread.sleep(1000000);  
        } catch (InterruptedException e) {  
            e.printStackTrace();  
        }  
        System.gc();  
    }  
}
```

1.直接 `new String()`：由于每个 String 对象都是 new 出来的，所以程序需要维护大量存放在堆空间中的 String 实例，程序内存占用也会变高

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291928467.png)

2使用 `intern()`：由于数组中字符串的引用都指向字符串常量池中的字符串，所以程序需要维护的 String 对象更少，内存占用也更低

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291929200.png)

**结论**：
1. 对于程序中大量使用存在的字符串时，尤其存在很多已经重复的字符串时，使用intern()方法能够节省很大的内存空间。
2. 大的网站平台，需要内存中存储大量的字符串。比如社交网站，很多人都存储：北京市、海淀区等信息。这时候如果字符串都调用intern() 方法，就会很明显降低内存的大小。


## 6.StringTable的垃圾回收

```java
/**  
 * String的垃圾回收:  
 * -Xms15m -Xmx15m -XX:+PrintStringTableStatistics -XX:+PrintGCDetails  
 */  
public class StringGCTest {  
    public static void main(String[] args) {  
        for (int j = 0; j < 100000; j++) {  
            String.valueOf(j).intern();  
        }  
    }  
}
```

输出结果：
- 在 PSYoungGen 区发生了垃圾回收
- Number of entries 和 Number of literals 明显没有 100000
- 以上两点均说明 StringTable 区发生了垃圾回收

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202301291931358.png)


## 7.G1中的String去重操作
**官方文档**：[http://openjdk.java.net/jeps/192](http://openjdk.java.net/jeps/192)

暂时了解一下，后面会详解垃圾回收器

**String去重操作的背景**
> 注意不是字符串常量池的去重操作，字符串常量池本身就没有重复的

对许多Java应用（有大的也有小的）做的测试得出以下结果：
- 堆存活数据集合里面String对象占了25%
- 堆存活数据集合里面重复的String对象有13.5%
- String对象的平均长度是45

许多大规模的Java应用的瓶颈在于内存，测试表明，在这些类型的应用里面，Java堆中存活的数据集合差不多25%是String对象。更进一步，这里面差不多一半String对象是重复的，重复的意思是说：`str1.equals(str2)= true`。堆上存在重复的String对象必然是一种内存的浪费。这个项目将在G1垃圾收集器中实现自动持续对重复的String对象进行去重，这样就能避免浪费内存。


**String 去重的的实现**

1. 当垃圾收集器工作的时候，会访问堆上存活的对象。对每一个访问的对象都会检查是否是候选的要去重的String对象。
2. 如果是，把这个对象的一个引用插入到队列中等待后续的处理。一个去重的线程在后台运行，处理这个队列。处理队列的一个元素意味着从队列删除这个元素，然后尝试去重它引用的String对象。
3. 使用一个Hashtable来记录所有的被String对象使用的不重复的char数组。当去重的时候，会查这个Hashtable，来看堆上是否已经存在一个一模一样的char数组。
4. 如果存在，String对象会被调整引用那个数组，释放对原来的数组的引用，最终会被垃圾收集器回收掉。
5. 如果查找失败，char数组会被插入到Hashtable，这样以后的时候就可以共享这个数组了。

**命令行选项**

1. `UseStringDeduplication(bool)`: 开启String去重，默认是不开启的，需要手动开启。
2. `PrintStringDeduplicationStatistics(bool)`: 打印详细的去重统计信息
3. `stringDeduplicationAgeThreshold(uintx)`: 达到这个年龄的String对象被认为是去重的候选对象

