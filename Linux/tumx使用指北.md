在后台服务器经常通过频繁的切换terminal窗口进行不同的任务的管理, tmux是一个非常棒的管理工具.

> 在运行所有快捷键之前需要通过 `Ctrl + b` 激活控制台, 文中 `A = Ctrl + b`

### 基本概念

![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202211101014515.png)
-   可以创建多个`session`
-   一个session中可以有多个`window(窗口)`
-   一个`window`里可以有多个`pane(面板)`

### 会话操作

`tmux new -s sims` : 创建一个名为`sims`的`session(会话)`

`tmux ls` : 查看所有会话

`tmux a` : 恢复到最近一次会话

`tmux a -t sims` : 恢复到名为`sims`的会话

`tmux kill-session -t sims` : 删除到名为`sims`的会话

`tmux kill-server` : 删除所有`session`

`tmux rename-session -t sims sims-mbp` : 在系统中, 重命名sims为sims-mbp

`A+ $` : 在session中, 重命名session

`A+ d` : 退出当前会话, 通过attach可以回到该会话

`A+ D` : 选择要退出的会话(多会话时)

`A+ s` : 选择要切换的会话(多会话时)

`A+ Ctrl + z` : 挂起当前会话

### 窗口操作

`A+ c` : 创建新的窗口

`A+ &` : 关闭当前窗口

`A+ 数字(0,1,2)` : 切换到指定窗口

`A+ p` : 切换到上一个窗口

`A+ n` : 切换到下一个窗口

`A+ l` : 两个窗口之间进行切换

`A+ w` : 列表切换形式窗口

`A+ ,` : 重命名窗口

`A+ .` : 修改窗口编号

`A+ f` : 在所有的窗口中查找文本

### 面板操作

`A+ "` : 平分为上下两块

`A+ %` : 平分为左右两块

`A+ x` : 关闭当前面板

`A+ !` : 将当前面板创建为新窗口

`A+ o` : 切换到当前面板的下一个面板

`A+ 方向键` : 切换面板

`A+ Space` : 循环切换布局

`A+ Ctrl + o` : 顺时针切换窗口

`A+ {` : 将当前面板前移

`A+ }` : 将当前面板前移

### 鼠标

```
setw -g mouse-resize-pane on  
setw -g mouse-select-pane on  
setw -g mouse-select-window on  
setw -g mode-mouse on  
```

-   开启用鼠标拖动调节pane的大小（拖动位置是pane之间的分隔线）
-   开启用鼠标点击pane来激活该pane
-   开启用鼠标点击来切换活动window（点击位置是状态栏的窗口名称）
-   开启window/pane里面的鼠标支持（也即可以用鼠标滚轮回滚显示窗口内容，此时还可以用鼠标选取文本）

在tmux里面按`Ctrl + b`+ `:`进入命令行模式 : 执行`source ~/.tmux.conf`即可生效