讨论下新版本 Selenium （4.x版本）的部分新语法。

>当然如果没有更新过或是不使用最新版本的 Selenium 是不受影响的，还可以使用以前的写法。

## 改动一：获取webdriver对象
旧版本 Selenium 语法：
```python
from selenium import webdriver
driver_path = '/home/yan/Python/chromeselenium/chromeselenium/chromedriver'
driver = webdriber.Chrome(executable_path=driver_path)
```
也可以是如下形式：
```python
driver_path = "http://chromedriver.storage.googleapis.com/index.html?path=107.0.5304.62/chromedriver_win32.zip"
```
`executable_path`：Selenium 驱动的存放路径，只有指定出该路径，Selenium 才能正常工作

 Selenium 经过版本更新之后，使用如上写法，系统就会报错，如下所示：
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202211080925924.png)
意思是：`executable_path` 已被弃用，请传入一个 Service 对象，于是我们就需要修改为如下代码：

**新版本 Selenium 语法**：
```python
from selenium import webdriver
from selenium.webdriver.chrome.service import Service  # 新增

path = "http://chromedriver.storage.googleapis.com/index.html?path=107.0.5304.62/chromedriver_win32.zip"
service = Service(executable_path=path)
driver = webdriver.Chrome(service=service)
```

## 改动二：Selenium元素定位
以百度首页为例，先访问 `http://www.baidu.com` ：
```python
from selenium import  webdriver
from selenium.webdriver.common.by import By
# 打开驱动
driver = webdriver.Firefox()
url = "http://www.baidu.com"
# 访问网址
driver.get(url)
```

旧版本 Selenium 中我们使用的方法是 `driver.find_element_方法名("value")`，
- `方法名`：`by_id`、`by_name`、`by_class_name`等等
- `value`：传入的值

以百度搜索框为例，右键点击百度搜索框点击检查则可看其HTML源代码中属性`id="kw"`，以旧版本的写法使用id值查找搜索框应该是：
```python
inputTag = driver.find_element_by_id("kw")
```
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202211072152613.png)

在版本没有更新之前，通常情况下运行都是能够正确定位到对应的元素，但是Selenium经过版本升级之后，运行后会报错（其他报错也是类似下面的报错信息），运行后会报错，此处报错如下：
![](https://ypic.oss-cn-hangzhou.aliyuncs.com/202211072153806.png)

根据官方最新文档，将代码进行修改，格式由 `driver.find_element_方法名("value")`变为 
`driver.find_element(By.类型, "value")`，具体改动如下：

新版本Selenium代码，首先在文件头部引入如下代码
```python
from selenium.webdriver.common.by import By
```

然后新版的八种选择方式如下：
- 利用ID查找
```python
# inputTag = driver.find_element_by_id("value")  
# 改为：
inputTag = driver.find_element(By.ID, "value")
```
- 利用类名查找
```python
# inputTags = driver.find_element_by_class_name("value")  
# 改为：
inputTag = driver.find_element(By.CLASS_NAME, "value")
```
- 利用name属性查找
```python
# inputTag = driver.find_element_by_name("value")  
# 改为：
inputTag = driver.find_element(By.NAME, "value")
```
- 利用标签名查找
```python
# inputTag = driver.find_element_by_tag_name("value")  
# 改为：
inputTag = driver.find_element(By.TAG_NAME, "value")
```
- 利用xpath查找
```python
# inputTag = driver.find_element_by_xpath("value")  
# 改为：
inputTag = driver.find_element(By.XPATH, "value")
```
- 利用CSS选择器查找
```python
# inputTag = driver.find_element_by_css_selector("value")  
# 改为：
inputTag = driver.find_element(By.CSS_SELETOR, "value")
```

