## Angular规范

Angular规范中定义的格式有3个内容：

Header Body Footer (): // 空一行 // 空一行

1 2 3 4 5 1、 Header Header部分有3个字段： type(必需), scope(可选), subject(必需)

- `type(必需)` ： Type of change，commit的类别；

- `scope(可选)`：Scope of this change， 此次commit的影响模块；

- `subject(必需)`：Short description，简短的描述此次代码变更的主要内容

（1）type

type用于说明commit的类别，常用的标识如下：

- `feat`：新功能（feature）

- `fix`：修补bug

- `docs`：文档（documentation）

- `style`： 格式（不影响代码运行的变动,空格,格式化,等等）
- `refactor`：重构（即不是新增功能，也不是修改bug的代码变动
- `perf`: 性能 (提高代码性能的改变)
- `test`：增加测试或者修改测试
- `build`: 影响构建系统或外部依赖项的更改(maven,gradle,npm 等等)
- `ci`: 对CI配置文件和脚本的更改
- `chore`：对非 src 和 test 目录的修改
- `revert`: Revert a commit 最常用的就是`feat` 和`fix`

（2）scope

`scope`用于说明 commit 影响的范围，比如数据层、控制层、视图层等等，视项目不同而不同。

（3）subject

`subject`是 commit 目的的简短描述，不超过50个字符，主要介绍此次代码变更的主要内容。

## 提交方式

### 1.使用template

`第一步`：创建一个`message_template`文件

```text
［需求／BUG］: 
［模块］: 
［说明］: 
［作者］:
```

`第二步`：设置模板

```text
git config --global [模板名] [模板绝对路径] // commit.template 后面是自己创建的模板文件的绝对路径
```

`第三步`：设计Commmit启动编辑器

```
git config --global core.editor vim // core.editor后面跟的是要设置的编辑器, 这里是vim
```

总结：需要每次使用命令行提交，如果是IDE提交他会弹出命令行

### 2.使用IDE

这里使用的是IntelliJ IDEA，类似的IDE也可以

`第一步`:安装插件

![img](https://ypic.oss-cn-hangzhou.aliyuncs.com/202304081648554.webp)

`第二步`：进入提交页面

![img](https://ypic.oss-cn-hangzhou.aliyuncs.com/202304081649599.webp)

`第三步`：在弹窗填好提交信息

![img](https://ypic.oss-cn-hangzhou.aliyuncs.com/202304081648958.webp)

`第四步`：提交信息

```text
docs(Git Commit 插件测试): 测试提交信息
1）修改了注释
```

> 转载: https://zhuanlan.zhihu.com/p/364615242