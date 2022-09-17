#国赛
## 问题 1
![](https://raw.githubusercontent.com/Anlieh/PicBucket/master/20220915181404.png)		

思路：
1）卡方检验
2）
![](https://raw.githubusercontent.com/Anlieh/PicBucket/master/20220916160306.png)

> 从”3 花岗岩风化程度遥感快速划分方法“ ：总计可以判断结论的规律
> 划分范围：


2）预测已风化的样品在未风化前的组成成分
从附件一中列出纹饰、颜色、类型一致的一类样品，然后这样的数据一定是多组的，然后是否可以认为同类样品中已风化的样品的数据是否与未风化的样品是一致的？

对于某些组合可能是全未风化的或者是已风化的，可以采用（1）的关系分析，使用影响不大的数据组作为替代，例如如下情况，就可以用第一组数据求第二组数据，而且对于全部风化的类来说没有必要进行聚类分析，
![](https://raw.githubusercontent.com/Anlieh/PicBucket/master/20220916212816.png)

处理思路：
	- 统计分析学？众数，三分点，或者直接平均值？结果得要区间
	- 再再次聚类？得到唯一簇中心，坐标可以当做未风化的初值，或许可以多次聚类，
		- 将簇中心再次聚类？（套娃 ×）
		- 或者将多个簇中心的坐标当做区间的上下界



## 问题 2
![](https://raw.githubusercontent.com/Anlieh/PicBucket/master/20220915183724.png)

1）分类规律：根据化学组成分类
	找出化学成分含量对两类玻璃分类的判断点，在哪个点或范围内可以认为是哪种类型玻璃
	Sheet2：化学组成  ===>>   突出化学物质的含量分为两大类，某个取值范围
	构建一个可解释的机器学习分类模型，例如决策树、逻辑回归，以类型(高钾玻璃、铅钡玻璃)为Y，尽可能构建足够多的特征X，形成可解释的分类规律，
	
2）亚类划分：
	优化 （1）的分类依据，根据占比稀少的化学物质的含量分类	
> 通过钙铝及其它微量元素将钾玻璃分成3个亚类,它们可能有不同的配方及制造中心;
	[1]斯琴毕力格,李青会,干福熹.激光剥蚀-电感耦合等离子体-原子发射光谱/质谱法分析中国古代钾玻璃组分[J].分析化学,2013,41(09):1328-1333.

3）分类结果的合理性和敏感性：直接使用模型自带的评价指标



## 问题 3
![](https://raw.githubusercontent.com/Anlieh/PicBucket/master/20220915201330.png)
1）依据`问题2` 的结果进行分类：先从高钾和铅钡入手，然后亚类划分：直接套入（2）的模型？

2）敏感性分析：难不成还是继续用模型自带的评价指标？
将分类后的数据按亚类分别填入问题2中聚类分析所用的数据，再次聚类分析，查看显著性和中心点坐标


## 问题 4
![](https://raw.githubusercontent.com/Anlieh/PicBucket/master/20220915185532.png)

关联性分析


## 代码

### 卡方检验：
```python
from scipy.stats import chi2_contingency  
import numpy as np  

x = [[82,48], [55,30]]
obs = np.array(x)  
print(chi2_contingency(obs, correction=True))
```

### 灰色预测
```python
# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
 
 
def GM11(x, n):
    '''
    灰色预测
    x：序列，numpy对象
    n: 需要往后预测的个数
    '''
    x1 = x.cumsum()  # 一次累加
    z1 = (x1[:len(x1) - 1] + x1[1:]) / 2.0  # 紧邻均值
    z1 = z1.reshape((len(z1), 1))
    B = np.append(-z1, np.ones_like(z1), axis=1)
    Y = x[1:].reshape((len(x) - 1, 1))
    # a为发展系数 b为灰色作用量
    [[a], [b]] = np.dot(np.dot(np.linalg.inv(np.dot(B.T, B)), B.T), 
					    Y)  # 计算参数
    result = (x[0] - b / a) * np.exp(-a * (n - 1)) - (x[0] - b / a) 
		    * np.exp(-a * (n - 2))
    S1_2 = x.var()  # 原序列方差
    e = list()  # 残差序列
    for index in range(1, x.shape[0] + 1):
        predict = (x[0] - b / a) * np.exp(-a * (index - 1)) - 
			        (x[0] - b / a) * np.exp(-a * (index - 2))
        e.append(x[index - 1] - predict)
    S2_2 = np.array(e).var()  # 残差方差
    C = S2_2 / S1_2  # 后验差比
    if C <= 0.35:
        assess = '后验差比<=0.35，模型精度等级为好'
    elif C <= 0.5:
        assess = '后验差比<=0.5，模型精度等级为合格'
    elif C <= 0.65:
        assess = '后验差比<=0.65，模型精度等级为勉强'
    else:
        assess = '后验差比>0.65，模型精度等级为不合格'
    # 预测数据
    predict = list()
    for index in range(x.shape[0] + 1, x.shape[0] + n + 1):
        predict.append((x[0] - b / a) * np.exp(-a * (index - 1)) - 
				        (x[0] - b / a) * np.exp(-a * (index - 2)))
    predict = np.array(predict)
    return {
        'a': {'value': a, 'desc': '发展系数'},
        'b': {'value': b, 'desc': '灰色作用量'},
        'predict': {'value': result, 'desc': '第%d个预测值' % n},
        'C': {'value': C, 'desc': assess},
        'predict': {'value': predict, 'desc': '往后预测%d个的序列' % (n)},
    }
 
 
if __name__ == "__main__":
    data = np.array([1.2, 2.2, 3.1, 4.5, 5.6, 6.7, 7.1,
				     8.2, 9.6, 10.6, 11, 12.4, 13.5, 14.7, 15.2])
    x = data[0:10]  # 输入数据
    y = data[10:]  # 需要预测的数据
    result = GM11(x, len(y))
    predict = result['predict']['value']
    predict = np.round(predict, 1)
    print('真实值:', y)
    print('预测值:', predict)
    print(result)
 
```

###  相关性检验
```matlab
clc;clear;

%%读取数据
data0=xlsread('表单1.xlsx','B2:E55');

%%检验纹饰与表面有无风化的相关性
%纹饰A-1，B-2，C-3，无风化-0，有风化-1，高钾-0，铅钡-1，紫-1，深绿2，深蓝-3，浅绿-4
%浅蓝-5，绿-6，蓝绿-7，黑-8
%%数据标准化
mu=mean(data0),sig=std(data0)%求均值与标准差
rr=corrcoef(data0)%求相关系数矩阵
data=zscore(data0);%数据标准化
a=data(:,[1,4]);b=data(:,[4:end]);%提出标准化后的自变量与因变量

%%检验纹饰与表面有无风化的相关性
fprintf('********纹饰*******\n');

%%de-纹饰
de=data(:,1);
Y=data(:,4);
[rho,pval] = corr(de,Y,'Tail','right')
[rho,pval] = corr(de,Y,'Tail','left')
%rh0=0.0623,说明纹饰与有无风化之间具有极低的正相关性，p=0.3273,高于显著性水平0.05
%这些结果表明接受纹饰与风化与否之间不存在相关性的原假设，并得出几乎无相关性的结论。

%%检验纹饰与表面有无风化的相关性
fprintf('********类型*******\n');
%%type-类型
type=data(:,2);
Y=data(:,4);
[rho,pval] = corr(type,Y,'Tail','right')
%rh0=0.3162,说明类型与有无风化之间具有较低的正相关性，p=0.0099,低于显著性水平0.05
%这些结果表明拒绝类型与风化与否之间不存在相关性的原假设，并得出相关性大于0的结论。
%5cl-颜色
fprintf('********颜色*******\n');

cl=data(:,3);
Y=data(:,4);
[rho,pval] = corr(cl,Y,'Tail','right')
%rh0=0.1283,说明颜色与有无风化之间具有极低的正相关性，p=0。1775,高于显著性水平0.05
%这些结果表明接受颜色与风化与否之间不存在相关性的原假设，并得出几乎无相关性的结论。
```

### 未命名
```matlab
mu =

    2.1481    0.6667    4.8704    0.5556


sig =

    0.9398    0.4758    2.0192    0.5016


rr =

    1.0000    0.1125   -0.4570    0.0623
    0.1125    1.0000   -0.4189    0.3162
   -0.4570   -0.4189    1.0000    0.1283
    0.0623    0.3162    0.1283    1.0000

********纹饰*******

rho =

    0.0623


pval =

    0.3273


rho =

    0.0623


pval =

    0.6727

********类型*******

rho =

    0.3162


pval =

    0.0099

********颜色*******

rho =

    0.1283


pval =

    0.1775

>>
```

### 聚类算法

#### K-means
```python
# k_means.py
# -*- coding: utf-8 -*-
# @Author: huzhu
# @Date:   2019-10-29 09:31:43
# @Last Modified by:   huzhu
# @Last Modified time: 2019-11-14 21:14:20

import codecs
from numpy import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from sklearn.datasets import make_moons
import matplotlib.animation as animation
from sklearn.cluster import KMeans

def load_data(path):
	"""
	@brief      Loads a data.
	@param      path  The path
	@return     data set
	"""
	data_set = list()
	with codecs.open(path) as f:
		for line in f.readlines():
			data = line.strip().split("\t")
			flt_data = list(map(float, data))
			data_set.append(flt_data)
	return data_set

def dist_eucl(vecA, vecB):
	"""
	@brief      the similarity function
	@param      vecA  The vector a
	@param      vecB  The vector b
	@return     the euclidean distance
	"""
	return sqrt(sum(power(vecA - vecB, 2)))

def rand_cent(data_mat, k):
	"""
	@brief      select random centroid
	@param      data_mat  The data matrix
	@param      k
	@return     centroids
	"""
	n = shape(data_mat)[1]
	centroids = mat(zeros((k, n)))
	for j in range(n):
		minJ = min(data_mat[:,j]) 
		rangeJ = float(max(data_mat[:,j]) - minJ)
		centroids[:,j] = mat(minJ + rangeJ * random.rand(k,1))
	return centroids

def kMeans(data_mat, k, dist = "dist_eucl", create_cent = "rand_cent"):
	"""
	@brief      kMeans algorithm
	@param      data_mat     The data matrix
	@param      k            num of cluster
	@param      dist         The distance funtion
	@param      create_cent  The create centroid function
	@return     the cluster
	"""
	m = shape(data_mat)[0]
	# 初始化点的簇
	cluster_assment = mat(zeros((m, 2)))  # 类别，距离
	# 随机初始化聚类初始点
	centroid = eval(create_cent)(data_mat, k)
	cluster_changed = True
	# 遍历每个点
	while cluster_changed:
		cluster_changed = False
		for i in range(m):
			min_index = -1
			min_dist = inf
			for j in range(k):
				distance = eval(dist)(data_mat[i, :], centroid[j, :])
				if distance < min_dist:
					min_dist = distance
					min_index = j
			if cluster_assment[i, 0] != min_index:
				cluster_changed = True
				cluster_assment[i, :] = min_index, min_dist**2
		# 计算簇中所有点的均值并重新将均值作为质心
		for j in range(k):
			per_data_set = data_mat[nonzero(cluster_assment[:,0].A == j)[0]]
			centroid[j, :] = mean(per_data_set, axis = 0)
	return centroid, cluster_assment

def plot_cluster(data_mat, cluster_assment, centroid):
	"""
	@brief      plot cluster and centroid
	@param      data_mat        The data matrix
	@param      cluster_assment  The cluste assment
	@param      centroid        The centroid
	@return     
	"""
	plt.figure(figsize=(15, 6), dpi=80)
	plt.subplot(121)
	plt.plot(data_mat[:, 0], data_mat[:, 1], 'o')
	plt.title("source data", fontsize=15)
	plt.subplot(122)
	k = shape(centroid)[0]
	colors = [plt.cm.get_cmap("Spectral")(each) for each in linspace(0, 1, k)]
	for i, col in zip(range(k), colors):
	    per_data_set = data_mat[nonzero(cluster_assment[:,0].A == i)[0]]
	    plt.plot(per_data_set[:, 0], per_data_set[:, 1], 'o', markerfacecolor=tuple(col),
	             markeredgecolor='k', markersize=10)
	for i in range(k):
		plt.plot(centroid[:,0], centroid[:,1], '+', color = 'k', markersize=18)
	plt.title("K-Means Cluster, k = 3", fontsize=15)
	plt.show()

def plot_noncov():
	"""
	@brief      绘制非凸优化函数图像
	@return     { description_of_the_return_value }
	"""
	fig = plt.figure()
	ax = fig.gca(projection='3d')
	x1 = linspace(-2,2,100)
	x2 = linspace(-2,2,100)
	mu1 = array([1,1])
	mu2 = array([-1,-1])
	Z = zeros((len(x1), len(x2)))
	for i in range(len(x1)):
		for j in range(len(x2)):
			itemx = x1[i]
			itemy = x2[j]
			z1 = dist_eucl(mu1, [itemx, itemy])
			z2 = dist_eucl(mu2, [itemx, itemy])
			Z[i,j] = min(z1,z2)
	X1, X2 = meshgrid(x1, x2)
	ax.plot_surface(X1, X2, Z, rstride=1, cstride=1, cmap='rainbow')
	plt.show()

def test_diff_k():
	plt.figure(figsize=(15, 4), dpi=80)
	data_mat = mat(load_data("data/testSet2_kmeans.txt"))
	centroid, cluster_assment = kMeans(data_mat, 2)
	plt.subplot(131)
	k = shape(centroid)[0]
	colors = [plt.cm.Spectral(each) for each in linspace(0, 1, k)]
	for i, col in zip(range(k), colors):
	    per_data_set = data_mat[nonzero(cluster_assment[:,0].A == i)[0]]
	    plt.plot(per_data_set[:, 0], per_data_set[:, 1], 'o', markerfacecolor=tuple(col),
	             markeredgecolor='k', markersize=10)
	for i in range(k):
		plt.plot(centroid[:,0], centroid[:,1], '+', color = 'k', markersize=18)
	plt.title("K-Means Cluster, k = 2", fontsize=15)
	
	centroid, cluster_assment = kMeans(data_mat, 3)
	plt.subplot(132)
	k = shape(centroid)[0]
	colors = [plt.cm.Spectral(each) for each in linspace(0, 1, k)]
	for i, col in zip(range(k), colors):
	    per_data_set = data_mat[nonzero(cluster_assment[:,0].A == i)[0]]
	    plt.plot(per_data_set[:, 0], per_data_set[:, 1], 'o', markerfacecolor=tuple(col),
	             markeredgecolor='k', markersize=10)
	for i in range(k):
		plt.plot(centroid[:,0], centroid[:,1], '+', color = 'k', markersize=18)
	plt.title("K-Means Cluster, k = 3", fontsize=15)

	centroid, cluster_assment = kMeans(data_mat, 4)
	plt.subplot(133)
	k = shape(centroid)[0]
	colors = [plt.cm.Spectral(each) for each in linspace(0, 1, k)]
	for i, col in zip(range(k), colors):
	    per_data_set = data_mat[nonzero(cluster_assment[:,0].A == i)[0]]
	    plt.plot(per_data_set[:, 0], per_data_set[:, 1], 'o', markerfacecolor=tuple(col),
	             markeredgecolor='k', markersize=10)
	for i in range(k):
		plt.plot(centroid[:,0], centroid[:,1], '+', color = 'k', markersize=18)
	plt.title("K-Means Cluster, k = 4", fontsize=15)
	plt.show()

def plot_fig(data_mat):
	"""
	@brief      绘制并保存gif图
	@param      data_mat  The data matrix
	@param      k         { parameter_description }
	@return     { description_of_the_return_value }
	"""
	centroid_list = list()
	cluster_assment_list = list()
	def sub_kMeans(data_mat, k, dist = "dist_eucl", create_cent = "rand_cent"):
		m = shape(data_mat)[0]
		# 初始化点的簇
		cluster_assment = mat(zeros((m, 2)))  # 类别，距离
		# 随机初始化聚类初始点
		centroid = eval(create_cent)(data_mat, k)
		cluster_changed = True
		# 遍历每个点
		while cluster_changed:
			centroid_list.append(array(centroid))
			cluster_assment_list.append(array(cluster_assment))
			cluster_changed = False
			for i in range(m):
				min_index = -1
				min_dist = inf
				for j in range(k):
					distance = eval(dist)(data_mat[i, :], centroid[j, :])
					if distance < min_dist:
						min_dist = distance
						min_index = j
				if cluster_assment[i, 0] != min_index:
					cluster_changed = True
					cluster_assment[i, :] = min_index, min_dist**2
			# 计算簇中所有点的均值并重新将均值作为质心
			for j in range(k):
				per_data_set = data_mat[nonzero(cluster_assment[:,0].A == j)[0]]
				centroid[j, :] = mean(per_data_set, axis = 0)
		return centroid_list,cluster_assment_list

	centroid_list,cluster_assment_list = sub_kMeans(data_mat,4)

	fig, ax = plt.subplots()
	plt.scatter(data_mat[:, 0].flatten().A[0], data_mat[:, 1].flatten().A[0])
	plt.title("K-Means Cluster Process", fontsize=15)
	def update(i):
		try:
			ax.lines.pop()
		except Exception:
			pass
		centroid = matrix(centroid_list[i])
		cluster_assment = matrix(cluster_assment_list[i])
		k = shape(centroid)[0]
		colors = [plt.cm.Spectral(each) for each in linspace(0, 1, k)]
		for i, col in zip(range(k), colors):
			per_data_set = data_mat[nonzero(cluster_assment[:,0].A == i)[0]]
			line, = plt.plot(per_data_set[:, 0], per_data_set[:, 1], 'o', markerfacecolor=tuple(col),markeredgecolor='k', markersize=10)
		line, = plt.plot(centroid[:,0], centroid[:,1], '*', color = 'k', markersize=18)
		return line,

	anim = animation.FuncAnimation(fig, update, frames=len(centroid_list),interval=1000, repeat_delay=1000)
	plt.show() 
	anim.save('test_animation.gif',writer='pillow')

def kmeans_lib():
	data_mat = mat(load_data("data/testSet2_kmeans.txt"))
	estimator = KMeans(n_clusters=3)#构造聚类器
	estimator.fit(data_mat)#聚类
	label_pred = estimator.labels_ #获取聚类标签
	print(label_pred)
	centroids = estimator.cluster_centers_ #获取聚类中心
	inertia = estimator.inertia_ # 获取聚类准则的总和
	plot_cluster(data_mat, mat(label_pred), centroids)
	print(centroids)
	print(inertia)

if __name__ == '__main__':
	#data_mat = mat(load_data("data/testSet_kmeans.txt"))
	data_mat = mat(load_data("data/testSet2_kmeans.txt"))
	#data_mat,c = make_moons(n_samples=1000,noise=0.1)  
	centroid, cluster_assment = kMeans(data_mat, 3)
	sse = sum(cluster_assment[:,1])
	print("sse is ", sse)
	plot_cluster(data_mat, cluster_assment, centroid)
	#plot_fig(data_mat)
	#plot_noncov()
	#test_diff_k()
	#kmeans_lib()
```

#### K-means++
```python
# kmeans++.py
# -*- coding: utf-8 -*-
# @Author: huzhu
# @Date:   2019-10-29 09:31:43
# @Last Modified by:   huzhu
# @Last Modified time: 2019-11-11 21:22:13

import codecs
from numpy import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def load_data(path):
	"""
	@brief      Loads a data.
	@param      path  The path
	@return     data set
	"""
	data_set = list()
	with codecs.open(path) as f:
		for line in f.readlines():
			data = line.strip().split("\t")
			flt_data = list(map(float, data))
			data_set.append(flt_data)
	return data_set

def dist_eucl(vecA, vecB):
	"""
	@brief      the similarity function
	@param      vecA  The vector a
	@param      vecB  The vector b
	@return     the euclidean distance
	"""
	return sqrt(sum(power(vecA - vecB, 2)))

def get_closest_dist(point, centroid):
	"""
	@brief      Gets the closest distance.
	@param      point     The point
	@param      centroid  The centroid
	@return     The closest distance.
	"""
	# 计算与已有质心最近的距离
	min_dist = inf
	for j in range(len(centroid)):
		distance = dist_eucl(point, centroid[j])
		if distance < min_dist:
			min_dist = distance
	return min_dist

def kpp_cent(data_mat, k):
	"""
	@brief      kmeans++ init centor
	@param      data_mat  The data matrix
	@param      k   num of cluster      
	@return     init centroid
	"""
	data_set = data_mat.getA()
	# 随机初始化第一个中心点
	centroid = list()
	centroid.append(data_set[random.randint(0,len(data_set))])
	d = [0 for i in range(len(data_set))]
	for _ in range(1, k):
		total = 0.0
		for i in range(len(data_set)):
			d[i] = get_closest_dist(data_set[i], centroid)
			total += d[i]
		total *= random.rand()
		# 选取下一个中心点
		for j in range(len(d)):
			total -= d[j]
			if total > 0:
				continue
			centroid.append(data_set[j])
			break
	return mat(centroid)

def kpp_Means(data_mat, k, dist = "dist_eucl", create_cent = "kpp_cent"):
	"""
	@brief      kpp means algorithm
	@param      data_mat     The data matrix
	@param      k            num of cluster
	@param      dist         The distance funtion
	@param      create_cent  The create centroid function
	@return     the cluster
	"""
	m = shape(data_mat)[0]
	# 初始化点的簇
	cluste_assment = mat(zeros((m, 2)))  # 类别，距离
	# 随机初始化聚类初始点
	centroid = eval(create_cent)(data_mat, k)
	cluster_changed = True
	# 遍历每个点
	while cluster_changed:
		cluster_changed = False
		for i in range(m):
			min_index = -1
			min_dist = inf
			for j in range(k):
				distance = eval(dist)(data_mat[i, :], centroid[j, :])
				if distance < min_dist:
					min_dist = distance
					min_index = j
			if cluste_assment[i, 0] != min_index:
				cluster_changed = True
				cluste_assment[i, :] = min_index, min_dist**2
		# 计算簇中所有点的均值并重新将均值作为质心
		for j in range(k):
			per_data_set = data_mat[nonzero(cluste_assment[:,0].A == j)[0]]
			centroid[j, :] = mean(per_data_set, axis = 0)
	return centroid, cluste_assment

def plot_cluster(data_mat, cluste_assment, centroid):
	"""
	@brief      plot cluster and centroid
	@param      data_mat        The data matrix
	@param      cluste_assment  The cluste assment
	@param      centroid        The centroid
	@return     
	"""
	plt.figure(figsize=(15, 6), dpi=80)
	plt.subplot(121)
	plt.plot(data_mat[:, 0], data_mat[:, 1], 'o')
	plt.title("source data", fontsize=15)
	plt.subplot(122)
	k = shape(centroid)[0]
	colors = [plt.cm.Spectral(each) for each in linspace(0, 1, k)]
	for i, col in zip(range(k), colors):
	    per_data_set = data_mat[nonzero(cluste_assment[:,0].A == i)[0]]
	    plt.plot(per_data_set[:, 0], per_data_set[:, 1], 'o', markerfacecolor=tuple(col),
	             markeredgecolor='k', markersize=10)
	for i in range(k):
		plt.plot(centroid[:,0], centroid[:,1], '+', color = 'k', markersize=18)
	plt.title("k-Means++ Cluster, k = 3", fontsize=15)
	plt.show()


if __name__ == '__main__':
	data_mat = mat(load_data("data/testSet2_kmeans.txt"))
	centroid, cluster_assment = kpp_Means(data_mat, 3)
	sse = sum(cluster_assment[:,1])
	print("sse is ", sse)
	plot_cluster(data_mat, cluster_assment, centroid)

```

