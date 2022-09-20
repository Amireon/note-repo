
## JavaAPI操作

添加 `maven` 依赖
```xml
<dependencies>
    <dependency>
        <groupId>org.elasticsearch</groupId>
        <artifactId>elasticsearch</artifactId>
        <version>7.8.0</version>
    </dependency>
    <!-- elasticsearch 的客户端 -->
    <dependency>
        <groupId>org.elasticsearch.client</groupId>
        <artifactId>elasticsearch-rest-high-level-client</artifactId>
        <version>7.8.0</version>
    </dependency>
    <!-- elasticsearch 依赖 2.x 的 log4j -->
    <dependency>
        <groupId>org.apache.logging.log4j</groupId>
        <artifactId>log4j-api</artifactId>
        <version>2.8.2</version>
    </dependency>
    <dependency>
        <groupId>org.apache.logging.log4j</groupId>
        <artifactId>log4j-core</artifactId>
        <version>2.8.2</version>
    </dependency>
    <dependency>
        <groupId>com.fasterxml.jackson.core</groupId>
        <artifactId>jackson-databind</artifactId>
        <version>2.9.9</version>
    </dependency>
    <!-- junit 单元测试 -->
    <dependency>
        <groupId>junit</groupId>
        <artifactId>junit</artifactId>
        <version>4.12</version>
    </dependency>
</dependencies>
```

Hello, world! 案例
```java
public class HelloElasticsearch {
	public static void main(String[] args) throws IOExcepiton {
		RestHighLevelClient esClient = new RestHighLevelClient(
			RestClient.builder(new HttpHost("localhost", 9200, "http"))
		);
		System.out.println(esClient);
		// 关闭客户端连接
		esClient.close();
	}
}
```

### 重构
由于本小节中会频繁出现如下代码:
```java
// 连接 Elasticsearch
RestHighLevelClient client = new RestHighLevelClient(
	RestClient.builder(new HttpHost("lcoalhost", 9200, "http")));

// 关闭 elasticsearch 的客户端
client.close();

```

于是对其重构: 
```java
import org.elasticsearch.client.RestHighLevelClient;

public interface ElasticsearchTask {

    void doSomething(RestHighLevelClient client) throws Exception;

}
```

```java
public class ConnectElasticsearch {
	public static void(ElasticsearchTask task) {
		// 创建客户端
		RestHighLevelClient client = new RestHighLevelClient(
				RestClient.builder(new HttpHost("lcoalhost", 9200, "http")));
		try {
			task.doSomthing(client);
			client.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	} 
}
```

之后便可如下编写  `lambda` 表达式来让 Elasticsearch 完成一些操作：
```java
ConnectElasticsearch.connect(client -> {
	// 完成你想要的操作
});
```

### 索引

#### 创建
```java
// 创建客户端对象
RestHighLevelClient esClient = new RestHighLevelClient(
	RestClient.builder(new HttpHost("localhost", 9200, "http"))
);

// 创建索引 - 请求对象
CreateIndexRequest request = new CreateIndexRequest("user");
// 发送请求, 获取响应
CreateIndexResponse reqponse = client.indices().create(request, 
		RequestOptions.DEFAULT);
// 响应状态
boolean acknowledged = request.isAcknowledegd();
System.out.println("操作状态 = " + acknowledged);

// 关闭客户端连接
client.close();
```

控制台输出:
```
四月 09, 2021 2:12:08 下午 org.elasticsearch.client.RestClient logResponse
警告: request [PUT http://localhost:9200/user2?master_timeout=30s&include_type_name=true&timeout=30s] returned 1 warnings: [299 Elasticsearch-7.8.0-757314695644ea9a1dc2fecd26d1a43856725e65 "[types removal] Using include_type_name in create index requests is deprecated. The parameter will be removed in the next major version."]
操作状态 = true

Process finished with exit code 0
```

#### 查询

```java
RestHighLevelClient client = new RestHighLevelClient(  
        RestClient.builder(new HttpHost("localhost",9200, "http"))  
);  
  
// 查询索引 - 请求对象  
GetIndexRequest request = new GetIndexRequest("user");  
// 发送请求, 获取响应  
GetIndexResponse response = client.indices().get(request, RequestOptions.DEFAULT);  
  
System.out.println("aliases" + response.getAliases());  
System.out.println("mapping" + response.getMappings());  
System.out.println("setting" + response.getSettings());  
  
client.close();
```

控制台输出:
```
aliases{user=[]}
mapping{user=org.elasticsearch.cluster.metadata.MappingMetadata@ceb7e09a}
setting{user={"index.creation_date":"1663674896677","index.number_of_replicas":"1","index.number_of_shards":"1","index.provided_name":"user","index.uuid":"H1wy3gkmRhShFFCTDc4lsQ","index.version.created":"7040099"}}

Process finished with exit code 0
```

#### 删除
```java
RestHighLevelClient client = new RestHighLevelClient(  
        RestClient.builder(new HttpHost("localhost", 9200, "http"))  
);  
  
// 删除索引: 构造请求对象  
DeleteIndexRequest request = new DeleteIndexRequest("user");  
// 发送请求, 获取响应  
AcknowledgedResponse response = client.indices().delete(request, RequestOptions.DEFAULT);  
// 操作结果  
System.out.println("操作结果: " + response.isAcknowledged());  
  
client.close();
```

控制台输出:
```
操作结果: true

Process finished with exit code 0
```

### 文档

#### 新增
```java
ConnectElasticsearch.connect(client -> {  
    // 新增文档 - 请求对象  
    IndexRequest request = new IndexRequest();  
    // 设置索引及唯一标识  
    request.index("user").id("1001");  
  
    // 创建数据对象  
    User zhangsan = new User("ZhangSan", 30, "男");  
  
    // 向 es 插入数据格式:  json 格式  
    ObjectMapper mapper = new ObjectMapper();  
    String userJson = mapper.writeValueAsString(zhangsan);  
    request.source(userJson, XContentType.JSON);  

	// 客户端发送请求，获取响应对象
    IndexResponse response = client.index(request, RequestOptions.DEFAULT);  
  
    // 打印结果信息
    System.out.println("_index:" + response.getIndex());
    System.out.println("_id:" + response.getId());
    System.out.println("_result:" + response.getResult());
});
```

控制台输出:
```
_index:user
_id:1001
_result:UPDATED
```

#### 修改
```java
ConnectElasticsearch.connect(client -> {
    // 修改文档 - 请求对象
    UpdateRequest request = new UpdateRequest();
    
    // 配置修改参数
    request.index("zhangsan").id("1001");
    
    // 设置请求体，对数据进行修改
    request.doc(XContentType.JSON, "sex", "女");
    
    // 客户端发送请求，获取响应对象
    UpdateResponse response = client.update(request, RequestOptions.DEFAULT);
    
    System.out.println("_index:" + response.getIndex());
    System.out.println("_id:" + response.getId());
    System.out.println("_result:" + response.getResult());
});

```

控制台输出:
```text
_index:user
_id:1001
_result:UPDATED
```

