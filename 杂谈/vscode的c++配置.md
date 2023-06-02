VSCode是一款微软出的轻量级编辑器，它本身只是一款文本编辑器而已，所有的功能都是以插件扩展的形式所存在，想用什么功能就安装对应的扩展即可，非常方便。

但是同样由于VSCode只是一款文本编辑器，不仅需要安装对应编程语言的扩展，还需要安装相应的编译器或者解释器。

C/C++的编译器有很多种，这里选择开源的MinGW编译器,可以从[sourceforge的mingw项目](https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/)下载

如果已有编译器，可以在终端中输入`gcc -v`，显示gcc的安装位置

```shell
gcc -v
Using built-in specs.
COLLECT_GCC=D:\MinGW64\bin\gcc.exe
```



### (1).配置编译器

接下来配置编译器路径，按快捷键Ctrl+Shift+P调出命令面板，输入C/C++，选择“Edit Configurations(UI)”进入配置。这里配置两个选项： - 编译器路径：D:/mingw-w64/x86_64-8.1.0-win32-seh-rt_v6-rev0/mingw64/bin/g++.exe

```text
这里的路径根据大家自己安装的Mingw编译器位置和配置的环境变量位置所决定。
```

- IntelliSense 模式：gcc-x64

`launch.json`

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [

        {
            "name": "gdb",
            "preLaunchTask": "g++.exe build active file",//调试前执行的任务，就是之前配置的tasks.json中的label字段
            "type": "cppdbg",//配置类型，只能为cppdbg
            "request": "launch",//请求配置类型，可以为launch（启动）或attach（附加）
            "program": "${fileDirname}\\${fileBasenameNoExtension}.exe",//调试程序的路径名称
            "args": [],//调试传递参数
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,//true显示外置的控制台窗口，false显示内置终端
            "MIMode": "gdb",
            "miDebuggerPath": "D:\\MinGW64\\bin\\gdb.exe",//改成你的gcc编译器安装位置
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

### (2).配置构建任务

接下来，创建一个tasks.json文件来告诉VS Code如何构建（编译）程序。该任务将调用g++编译器基于源代码创建可执行文件。 按快捷键Ctrl+Shift+P调出命令面板，输入tasks，选择“Tasks:Configure Default Build Task”：

`tasks.json`

```json
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "g++.exe build active file",
            "command": "D:\\MinGW64\\bin\\g++.exe", //改成你的gcc编译器安装位置
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}.exe"
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": "build",
            "detail": "编译器: D:\\MinGW64\\bin\\g++.exe" //改成你的gcc编译器安装位置
        }
    ],
    "version": "2.0.0"
}
```





