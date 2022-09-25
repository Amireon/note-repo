# Linux守护进程

在Linux服务器实际应用中，经常会有需要长时间执行的任务。这类任务若在前台运行，用户无法进行其他操作或者断开与服务器的连接，否则任务将被中止。
本文简要讲解 nohup, screen 几种方式

## nohup
nohup 命令的作用就是让后台工作在离开操作终端时，也能够正确地在后台执行

**基本格式:*
```shell
nohup Command [ Arg … ] [　& ]
```
参数说明
- Command：要执行的命令。
- Arg：一些参数，可以指定输出文件。
- &：让命令在后台执行，终端退出后命令仍旧执行。

**实例：**
以下命令在后台执行 `/usr/local/`目录下的 demo.sh 脚本：
```shell
nohup /root/demo.sh &
```

demo.sh 的内容如下：
```shell
pwd
```

在终端如果看到以下输出说明运行成功：
```
/usr/local
```

如果要停止运行，你需要使用以下命令查找到 nohup 运行脚本到 PID，然后使用 kill 命令来删除：
```shell
ps -aux | grep "demo.sh"
```
参数说明：
-   **a** : 显示所有程序
-   **u** : 以用户为主的格式来显示
-   **x** : 显示所有程序，不区分终端机

另外也可以使用如下方式：
```shell
ps -def | grep "demo.sh   # 查找此文件的进程
kill -9  进程号PID         # kill PID 来删除。
```

## screen
