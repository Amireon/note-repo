>https://blog.csdn.net/whatday/article/details/112169945?ops_request_misc=&request_id=&biz_id=102&utm_term=python%20%E4%BB%A3%E7%90%86%E8%BF%90%E8%A1%8C%E7%A8%8B%E5%BA%8F&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-0-112169945.142^v83^insert_down1,201^v4^add_ask,239^v2^insert_chatgpt&spm=1018.2226.3001.4187

本文介绍几种在Python里使用代理的方式，假定代理运行在本机，HTTP代理端口为1231, SOCKS5代理端口为8080。



## HTTP全局代理
### 1.环境变量方式

在Linux中，在命令行里配置如下环境变量，然后执行Python脚本，Python在进行网络请求时就会使用配置的代理。

```bash
export http_proxy="http://127.0.0.1:1231"
export https_proxy="https://127.0.0.1:1231"
```

在Windows中，cmd，powershell，git-bash设置代理的方式是不一样的 (因为不同shell"设置环境变量"的语法不同)

posershell
```shell
$env:http_proxy="http://127.0.0.1:1231"
$env:https_proxy="https://127.0.0.1:1231"
```

git-bash
```shell
export http_proxy="http://127.0.0.1:1231"
export https_proxy="https://127.0.0.1:1231"
```

cmd
```python
set http_proxy='127.0.0.1:port'
set https_proxy='127.0.0.1:port'
```

### 2.代码实现方式

也可以在Python代码里添加如下内容，效果与上面的方式相同：

```python
import os

os.environ["http_proxy"] = "http://127.0.0.1:1231"
os.environ["https_proxy"] = "http://127.0.0.1:1231"
```

## SOCKS全局代理

通过设置环境变量的方式通常只能使用HTTP代理。要使用全局SOCKS代理可以使用tsocks.
安装tsocks后，编辑`/etc/tsocks.conf`，以使用端口为8080的本地SOCKS5代理为例：

```conf
server = 127.0.0.1
server_port = 8080 
server_type = 5
```

配置完成后在原来的脚本执行命令前添加tsocks即可使用，例如:

```python
tsocks python3 myscript.py
```

## 针对部分请求设置代理

前面几种方式会代理所有的HTTP请求，如果只想让部分请求使用代理，可以使用requests的proxies参数：

```python
import requests

proxies = {'http': "socks5://127.0.0.1:8080",
           'https': "socks5://127.0.0.1:8080"}
print(requests.get(url, proxies=proxies).content)
```

例如下载图片:

```python
with requests.get(url, proxies=proxies, stream=True) as r:
    if r.status_code != 200:
        return
    with open(path, 'wb') as f:
        for chunk in r.iter_content(chunk_size=8192): 
            f.write(chunk)
```

