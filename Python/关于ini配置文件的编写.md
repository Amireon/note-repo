> https://blog.csdn.net/qdPython/article/details/120225316

`ini`文件即 Initalization File 初始化文件，在应用程序及框架中常作为配置文件使用，是一种静态纯文本文件，使用记事本即可编辑。

配置文件的主要功能就是存储一批变量和变量值，在ini文件中使用`section`对变量进行分组，基本格式如下:

```ini
# filename: config.ini
[user]
name=admin
password=123456
is_admin=true

[mysql]
host=10.10.10.10
port=3306
db=apitest
user=root
password=123456

[log]
file=run.log
level=info
```

ini文件中使用`#`或者`;`添加注释，最好独占一行，不能写在变量后面

上述配置文件共有三个section，分别是user, mysql, log, 配置文件的主要作用是抽离环境变量等信息，避免硬编码带来的不便

## 如何读取

读取ini配置文件需要使用Python3自带的configparser库，使用示例如下

```python
from configparser import ConfigParser

conf = ConfigParser()  # 需要实例化一个ConfigParser对象
conf.read('config.ini', encoding='utf-8')  # 需要添加上config.ini的路径，不需要open打开，直接给文件路径就读取，
```

ConfigParser对象的每个section段的数据类似于一个字典，可以使用`[‘变量名’]`或者`parser.get(‘变量名’)`获取对应的值，**parser返回的值都是字符串格式**

其他常用的读取方法如下：

- `conf.sections()`: 获取所有的section名，结果[‘user’, ‘mysql’, ‘log’]
- `conf['mysql']['port']`: 获取section端port变量的值，字符串格式
- `conf['mysql'].get('port')`: 同上，字符串格式
- `conf.get('mysql', 'port')`: 同上，字符串格式
- `conf['mysql'].getint('port')`: 获取对应变量的整型值
- `conf['mysql'].getfloat('port')`: 获取对应变量的浮点型值
- `conf['user'].getboolean('is_admin')`: 获取对应变量的布尔值，支持配置为yes/no, on/‘off, true/false 和 1/0，都可以转化为Python中的True/False
- `conf.has_section(section)`：检查是否有该section
- `conf.options(section)`：输出section中所有的变量名
- `conf.has_option(section, option)`：检查指定section下是否有该变量值

如果想遍历一个section所有的变量和值，可以像遍历字典意义操作，示例如下:
```python
for key, value in conf['mysql'].items():
    print(key, value)
```

**注意**：ini文件中的变量名是大小写不敏感的，而Section名是大小写敏感的。

## 公共变量
假如我们每个Section变量组都有一批相同的重复变量，便可以定义在`[DEFAULT]`区域中.
ini 提供了 `[DEFAULT]` section，该区域的作用是定义公共变量，使得其他section都可以访问该区域的字段

示例：使用 DEFAULT 定义公共变量:

```ini
[DEFAULT]
base_url=http://localhost:7777
user=admin
password=123456

[dev]
# 开发环境

[prod]
# 生产环境
base_url=http://www.abc.com
```

- 定义的公共变量: `base_url`, `user`, `admin`
- 如何访问：`任意的section_name + 字段名`, 如`conf[dev][user]`
- 覆盖问题：在`[prod]`中也定义了字段base_url，那么该区域的base_url会被覆盖掉, 类似于类继承

## 参数化
在ini文件中我们还可以使用`%(变量名)s`的占位符进行参数化，这种特性被称为Interpolation(插值)。

比如一个接口，不同的参数对应不同的场景，示例如下:
```ini
[DEFAULT]
url = %(base_url)s/get?a=%(a)s&b=%(b)s
base_url=https://httpbin.org

[dev]
# 开发环境
base_url=http://localhost:5555
a=1
b=2

[prod]
# 生产环境
a=kevin
b=male
```

在`[DEFAULT]`中提供了一个参数化的公用变量，要求提供三个参数: `%(base_url)s`, `%(a)s`, `%(b)s`
并且设置了base_url变量的默认值为https://httpbin.org

注意：每个section段中，加上默认变量base_url，**必须提供所有参数化变量的值**，比如此例中每个段必须设置a和b的值，否则会报错。

在Python脚本中打印`conf['prod']['url']`，可以得到组装后的url。
```python
https://httpbin.org/get?a=kevin&b=male
```

注：可以使用ExtendedInterpolation()来支持${变量}占位符方式，示例如下：

```python
from configparser import ExtendedInterpolation
conf = ConfigParser(interpolation=ExtendedInterpolation(),  inline_comment_prefixes=['#', ';'],allow_no_value=True)
```

## 如何修改

ConfigParser() 对象常用的修改方法如下：

- `add_section(section)`：添加section
- `set(section, option, value)`: 在指定section中添加变量和变量值，section必须存在或已添加，value必须为字符串形式，可以设置DEFAULT字段的值
- `remove_option(section, option)`：移除指定section下指定变量
- `remove_section(section)`：移除section
- `write(f)`：保存ini文件

```python
from configparser import ConfigParser

conf = ConfigParser()

# conf.read('httpbin.ini', encoding='utf-8') # 如果新建的话就不需要read,如果修改则需要使用read打开

conf.set('DEFAULT', 'url', '%(base_url)s/get?a=%(a)s&b=%(b)s')  # 可以设置DEFAULT段的值
conf.set('DEFAULT', 'base_url', 'https://httpbin.org')  # 可以设置DEFAULT段的值
conf.add_section('dev')
conf.set('dev', 'base_url', 'http://localhost:5555')
conf.set('dev', 'a', '1')  # 值必须是字符串
conf.set('dev', 'b', '2')

conf['prod-case1'] = {'a': 1, 'b': 2}  # 直接使用字典添加多个变量
conf['prod-case2'] = {'a': 'kevin', 'b': 'male'}

# 保存csv
with open('httpbin.ini', 'w', encoding='utf-8') as f:
    conf.write(f)
```

更改后的ini文件内容
```ini
[DEFAULT]
url = %(base_url)s/get?a=%(a)s&b=%(b)s
base_url = https://httpbin.org

[dev]
base_url = http://localhost:5555
a = 1
b = 2

[prod-case1]
a = 1
b = 2

[prod-case2]
a = kevin
b = male
```
