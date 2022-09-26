# Http 和 JavaAPI 入门
## JavaAPI操作
**首要：** 添加 `maven` 依赖
```xml
<dependencies>
    <dependency>
        <groupId>org.elasticsearch</groupId>
        <artifactId>elasticsearch</artifactId>
        <version>7.8.0</version>
    </dependency>
    <!-- elasticsearch 客户端 -->
    <dependency>
        <groupId>org.elasticsearch.client</groupId>
        <artifactId>elasticsearch-rest-high-level-
			client</artifactId>
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
</dependencies>
```

Hello, world 案例
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
    UpdateResponse response = client.update(request, 
	    RequestOptions.DEFAULT);
    
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

#### 查询
```java
ConnectElasticsearch.connect(client -> {
	// 1) 创建请求对象
	GetRequest request = new GetRequest().index("user").id("1001");

	// 2) 客户端发送请求: 获取响应
	GetResponse response = client.get(request, 
		ReqestOptions.DEFAULT);

	// 3) 结果信息
	System.out.println("_index:" + response.getIndex());
    System.out.println("_type:" + response.getType());
    System.out.println("_id:" + response.getId());
    System.out.println("source:" + response.getSourceAsString());
})

```

控制台输出：
```log
_index:user
_type:_doc
_id:1001
source:{"name":"zhangsan","age":30,"sex":"男"}
```

#### 删除
```java
ConnnectElasticsearch.connect(client -> {
	// 1) 创建请求对象
	DeleteRequest request = new DeleteRequest()
							.index("user").id("1001");

	// 2) 客户端发送请求：获取响应
	client.delete(request, RequestOptions.DEFAULT);

	// 3) 打印结果
	System.out.println(response.toString());
})
```

控制台输出：
```log
DeleteResponse[index=user,type=_doc,id=1001,version=16,result=deleted,shards=ShardInfo{total=2, successful=1, failures=[]}]
```

### 文档批量
#### 批量新增
```java
 ConnectElasticsearch.connect(client -> {
    // 1) 创建批量新增请求对象
    BulkRequest request = new BulkRequest();
    request.add(new IndexRequest().index("user").id("1001")
	    .source(XContentType.JSON, "name", "zhangsan"));
    request.add(new IndexRequest().index("user").id("1002")
        .source(XContentType.JSON, "name", "lisi"));
    request.add(newIndexRequest().index("user").id("1003")
	    .source(XContentType.JSON, "name", "wangwu"));
   
	// 2) 客户端发送请求: 获取响应对象
    BulkResponse responses = client.bulk(request, 
		RequestOptions.DEFAULT);
    
    // 3) 打印结果信息
    System.out.println("took:" + responses.getTook());
    System.out.println("items:" + responses.getItems());
});
```

控制台输出
```log
took:294ms
items:[Lorg.elasticsearch.action.bulk.BulkItemResponse;@2beee7ff]
```

#### 批量删除
```java
ConnectElasticsearch.connect(client -> {
    // 1) 创建批量删除请求对象
	BulkRequest request = new BulkRequest();
	request.add(new DeleteRequest().index("user").id("1001"));
	request.add(new DeleteRequest().index("user").id("1002"));
	request.add(new DeleteRequest().index("user").id("1003"));
    // 2) 客户端发送请求，获取响应对象
	BulkResponse responses = client.bulk(request,
							RequestOptions.DEFAULT);
	// 3) 打印结果信息
    System.out.println("took:" + responses.getTook());
    System.out.println("items:" + responses.getItems());
});
```

控制台输出
```log
took:108ms
items:[Lorg.elasticsearch.action.bulk.BulkItemResponse;@7b02881e]
```

### 文档高级查询
#### 全量查询
```java
ConnectElasticsearch.connect(client -> {
    // 创建搜索请求对象
    SearchRequest request = new SearchRequest().indices("user");
    
    // 构建查询的请求体
    SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
    
    // 查询所有数据
    sourceBuilder.query(QueryBuilders.matchAllQuery());
    request.source(sourceBuilder);
    SearchResponse response = client.search(request, 
    RequestOptions.DEFAULT);
    
    // 查询匹配
    SearchHits hits = response.getHits();
    System.out.println("took:" + response.getTook());
    System.out.println("timeout:" + response.isTimedOut());
    System.out.println("total:" + hits.getTotalHits());
    System.out.println("MaxScore:" + hits.getMaxScore());
    System.out.println("hits========>>");
    //输出每条查询的结果信息
    for (SearchHit hit : hits) {
            System.out.println(hit.getSourceAsString());
        }
	System.out.println("<<========");
});
```

控制台输出：
```log
took:2ms
timeout:false
total:6 hits
MaxScore:1.0
hits========>>
{"name":"zhangsan","age":"10","sex":"女"}
{"name":"lisi","age":"30","sex":"女"}
{"name":"wangwu1","age":"40","sex":"男"}
{"name":"wangwu2","age":"20","sex":"女"}
{"name":"wangwu3","age":"50","sex":"男"}
{"name":"wangwu4","age":"20","sex":"男"}
<<======== 
```

#### 条件查询


#### 分页查询


#### 查询排序


#### 组合查询


#### 范围查询


#### 模糊查询


#### 高亮查询


#### 最大值查询


#### 分组查询
