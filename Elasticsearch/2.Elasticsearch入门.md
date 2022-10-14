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
```java
public class QueryDoc {
	public static final ElasticsearchTask SEARCH_BY_CONDITION = 
		client -> {
		 // 创建搜索请求对象
        SearchRequest request = new SearchRequest();
        request.indices("user");
        // 构建查询的请求体
    SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        sourceBuilder.query(QueryBuilders.termQuery("age", "30"));
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
        System.out.println("<<============");	
	}

	public static void main(String[] args) { 
		ConnectElasticsearch.connect(SEARCH_BY_CONDITION); 
	}
}
```

控制台输出：
```
took:1ms
timeout:false
total:1 hits
MaxScore:1.0
hits========>>
{"name":"lisi","age":"30","sex":"女"}
<<============
```

#### 分页查询
```java
public class QueryDoc {
    
	public static final ElasticsearchTask SEARCH_BY_PAGING = client -> {
        // 创建搜索请求对象
        SearchRequest request = new SearchRequest();
        request.indices("user");
        // 构建查询的请求体
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        sourceBuilder.query(QueryBuilders.matchAllQuery());
        // 分页查询
        // 当前页其实索引(第一条数据的顺序号)， from
        sourceBuilder.from(0);

        // 每页显示多少条 size
        sourceBuilder.size(2);
        request.source(sourceBuilder);
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        // 查询匹配
        SearchHits hits = response.getHits();
        System.out.println("took:" + response.getTook());
        System.out.println("timeout:" + response.isTimedOut());
        System.out.println("total:" + hits.getTotalHits());
        System.out.println("MaxScore:" + hits.getMaxScore());
        System.out.println("hits========>>");
        for (SearchHit hit : hits) {
            //输出每条查询的结果信息
            System.out.println(hit.getSourceAsString());
        }
        System.out.println("<<========");
    };
    
    public static void main(String[] args) {
        ConnectElasticsearch.connect(SEARCH_BY_CONDITION);
    }

}
```

控制台输出
```
took:1ms
timeout:false
total:6 hits
MaxScore:1.0
hits========>>
{"name":"zhangsan","age":"10","sex":"女"}
{"name":"lisi","age":"30","sex":"女"}
<<========

```

#### 查询排序
```java
public class QueryDoc {
    
	public static final ElasticsearchTask SEARCH_WITH_ORDER = client -> {
        // 创建搜索请求对象
        SearchRequest request = new SearchRequest();
        request.indices("user");

        // 构建查询的请求体
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        sourceBuilder.query(QueryBuilders.matchAllQuery());
        // 排序
        sourceBuilder.sort("age", SortOrder.ASC);
        request.source(sourceBuilder);
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        // 查询匹配
        SearchHits hits = response.getHits();
        System.out.println("took:" + response.getTook());
        System.out.println("timeout:" + response.isTimedOut());
        System.out.println("total:" + hits.getTotalHits());
        System.out.println("MaxScore:" + hits.getMaxScore());
        System.out.println("hits========>>");
        for (SearchHit hit : hits) {
        //输出每条查询的结果信息
            System.out.println(hit.getSourceAsString());
        }
        System.out.println("<<========");
    };

    public static void main(String[] args) {
        ConnectElasticsearch.connect(SEARCH_WITH_ORDER);
    }

}
```

控制台输出
```
took:1ms
timeout:false
total:6 hits
MaxScore:NaN
hits========>>
{"name":"zhangsan","age":"10","sex":"女"}
{"name":"wangwu2","age":"20","sex":"女"}
{"name":"wangwu4","age":"20","sex":"男"}
{"name":"lisi","age":"30","sex":"女"}
{"name":"wangwu1","age":"40","sex":"男"}
{"name":"wangwu3","age":"50","sex":"男"}
<<========

```

#### 组合查询
```java
public class QueryDoc {
    
	public static final ElasticsearchTask SEARCH_BY_BOOL_CONDITION = client -> {
        // 创建搜索请求对象
        SearchRequest request = new SearchRequest();
        request.indices("user");
        // 构建查询的请求体
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        BoolQueryBuilder boolQueryBuilder = QueryBuilders.boolQuery();
        // 必须包含
        boolQueryBuilder.must(QueryBuilders.matchQuery("age", "30"));
        // 一定不含
        boolQueryBuilder.mustNot(QueryBuilders.matchQuery("name", "zhangsan"));
        // 可能包含
        boolQueryBuilder.should(QueryBuilders.matchQuery("sex", "男"));
        sourceBuilder.query(boolQueryBuilder);
        request.source(sourceBuilder);
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        // 查询匹配
        SearchHits hits = response.getHits();
        System.out.println("took:" + response.getTook());
        System.out.println("timeout:" + response.isTimedOut());
        System.out.println("total:" + hits.getTotalHits());
        System.out.println("MaxScore:" + hits.getMaxScore());
        System.out.println("hits========>>");
        for (SearchHit hit : hits) {
            //输出每条查询的结果信息
            System.out.println(hit.getSourceAsString());
        }
        System.out.println("<<========");

    };

    public static void main(String[] args) {
        ConnectElasticsearch.connect(SEARCH_BY_BOOL_CONDITION);
    }
}

```

控制台输出
```
took:28ms
timeout:false
total:1 hits
MaxScore:1.0
hits========>>
{"name":"lisi","age":"30","sex":"女"}
<<========

Process finished with exit code 0

```

#### 范围查询
```java
public class QueryDoc {
    
	public static final ElasticsearchTask SEARCH_BY_RANGE = client -> {
        // 创建搜索请求对象
        SearchRequest request = new SearchRequest();
        request.indices("user");
        // 构建查询的请求体
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        RangeQueryBuilder rangeQuery = QueryBuilders.rangeQuery("age");
        // 大于等于
        //rangeQuery.gte("30");
        // 小于等于
        rangeQuery.lte("40");
        sourceBuilder.query(rangeQuery);
        request.source(sourceBuilder);
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        // 查询匹配
        SearchHits hits = response.getHits();
        System.out.println("took:" + response.getTook());
        System.out.println("timeout:" + response.isTimedOut());
        System.out.println("total:" + hits.getTotalHits());
        System.out.println("MaxScore:" + hits.getMaxScore());
        System.out.println("hits========>>");
        for (SearchHit hit : hits) {
        //输出每条查询的结果信息
            System.out.println(hit.getSourceAsString());
        }
        System.out.println("<<========");
    };

    public static void main(String[] args) {
        ConnectElasticsearch.connect(SEARCH_BY_RANGE);
    }

}
```

控制台输出
```
took:1ms
timeout:false
total:5 hits
MaxScore:1.0
hits========>>
{"name":"zhangsan","age":"10","sex":"女"}
{"name":"lisi","age":"30","sex":"女"}
{"name":"wangwu1","age":"40","sex":"男"}
{"name":"wangwu2","age":"20","sex":"女"}
{"name":"wangwu4","age":"20","sex":"男"}
<<========

Process finished with exit code 0

```

#### 模糊查询
```java
public class QueryDoc {
    
    public static final ElasticsearchTask SEARCH_BY_FUZZY_CONDITION = client -> {
        // 创建搜索请求对象
        SearchRequest request = new SearchRequest();
        request.indices("user");
        // 构建查询的请求体
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        sourceBuilder.query(QueryBuilders.fuzzyQuery("name","wangwu").fuzziness(Fuzziness.ONE));
        request.source(sourceBuilder);
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        // 查询匹配
        SearchHits hits = response.getHits();
        System.out.println("took:" + response.getTook());
        System.out.println("timeout:" + response.isTimedOut());
        System.out.println("total:" + hits.getTotalHits());
        System.out.println("MaxScore:" + hits.getMaxScore());
        System.out.println("hits========>>");
        for (SearchHit hit : hits) {
            //输出每条查询的结果信息
            System.out.println(hit.getSourceAsString());
        }
        System.out.println("<<========");
    };


    public static void main(String[] args) {
//        ConnectElasticsearch.connect(SEARCH_ALL);
//        ConnectElasticsearch.connect(SEARCH_BY_CONDITION);
//        ConnectElasticsearch.connect(SEARCH_BY_PAGING);
//        ConnectElasticsearch.connect(SEARCH_WITH_ORDER);
//        ConnectElasticsearch.connect(SEARCH_BY_BOOL_CONDITION);
//        ConnectElasticsearch.connect(SEARCH_BY_RANGE);
        ConnectElasticsearch.connect(SEARCH_BY_FUZZY_CONDITION);
    }

}
```

控制台输出
```
took:152ms
timeout:false
total:4 hits
MaxScore:1.2837042
hits========>>
{"name":"wangwu1","age":"40","sex":"男"}
{"name":"wangwu2","age":"20","sex":"女"}
{"name":"wangwu3","age":"50","sex":"男"}
{"name":"wangwu4","age":"20","sex":"男"}
<<========

Process finished with exit code 0

```

#### 高亮查询
```java
public class QueryDoc {
    
    public static final ElasticsearchTask SEARCH_WITH_HIGHLIGHT = client -> {
        // 高亮查询
        SearchRequest request = new SearchRequest().indices("user");
        //2.创建查询请求体构建器
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        //构建查询方式：高亮查询
        TermsQueryBuilder termsQueryBuilder =
                QueryBuilders.termsQuery("name","zhangsan");
        //设置查询方式
        sourceBuilder.query(termsQueryBuilder);
        //构建高亮字段
        HighlightBuilder highlightBuilder = new HighlightBuilder();
        highlightBuilder.preTags("<font color='red'>");//设置标签前缀
        highlightBuilder.postTags("</font>");//设置标签后缀
        highlightBuilder.field("name");//设置高亮字段
        //设置高亮构建对象
        sourceBuilder.highlighter(highlightBuilder);
        //设置请求体
        request.source(sourceBuilder);
        //3.客户端发送请求，获取响应对象
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        //4.打印响应结果
        SearchHits hits = response.getHits();
        System.out.println("took::"+response.getTook());
        System.out.println("time_out::"+response.isTimedOut());
        System.out.println("total::"+hits.getTotalHits());
        System.out.println("max_score::"+hits.getMaxScore());
        System.out.println("hits::::>>");
        for (SearchHit hit : hits) {
            String sourceAsString = hit.getSourceAsString();
            System.out.println(sourceAsString);
            //打印高亮结果
            Map<String, HighlightField> highlightFields = hit.getHighlightFields();
            System.out.println(highlightFields);
        }
        System.out.println("<<::::");
    };


    public static void main(String[] args) {
        ConnectElasticsearch.connect(SEARCH_WITH_HIGHLIGHT);
    }

}
```

控制台输出
```
took::672ms
time_out::false
total::1 hits
max_score::1.0
hits::::>>
{"name":"zhangsan","age":"10","sex":"女"}
{name=[name], fragments[[<font color='red'>zhangsan</font>]]}
<<::::


```


#### 最大值查询
```java
public class QueryDoc {
    
    public static final ElasticsearchTask SEARCH_WITH_MAX = client -> {
        // 高亮查询
        SearchRequest request = new SearchRequest().indices("user");
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        sourceBuilder.aggregation(AggregationBuilders.max("maxAge").field("age"));
        //设置请求体
        request.source(sourceBuilder);
        //3.客户端发送请求，获取响应对象
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        //4.打印响应结果
        SearchHits hits = response.getHits();
        System.out.println(response);
    };

    public static void main(String[] args) {
        ConnectElasticsearch.connect(SEARCH_WITH_MAX);
    }

}
```

控制台输出
```
{"took":16,"timed_out":false,"_shards":{"total":1,"successful":1,"skipped":0,"failed":0},"hits":{"total":{"value":6,"relation":"eq"},"max_score":1.0,"hits":[{"_index":"user","_type":"_doc","_id":"1001","_score":1.0,"_source":{"name":"zhangsan","age":"10","sex":"女"}},{"_index":"user","_type":"_doc","_id":"1002","_score":1.0,"_source":{"name":"lisi","age":"30","sex":"女"}},{"_index":"user","_type":"_doc","_id":"1003","_score":1.0,"_source":{"name":"wangwu1","age":"40","sex":"男"}},{"_index":"user","_type":"_doc","_id":"1004","_score":1.0,"_source":{"name":"wangwu2","age":"20","sex":"女"}},{"_index":"user","_type":"_doc","_id":"1005","_score":1.0,"_source":{"name":"wangwu3","age":"50","sex":"男"}},{"_index":"user","_type":"_doc","_id":"1006","_score":1.0,"_source":{"name":"wangwu4","age":"20","sex":"男"}}]},"aggregations":{"max#maxAge":{"value":50.0}}}

```


#### 分组查询
```java
public class QueryDoc {

	public static final ElasticsearchTask SEARCH_WITH_GROUP = client -> {
        SearchRequest request = new SearchRequest().indices("user");
        SearchSourceBuilder sourceBuilder = new SearchSourceBuilder();
        sourceBuilder.aggregation(AggregationBuilders.terms("age_groupby").field("age"));
        //设置请求体
        request.source(sourceBuilder);
        //3.客户端发送请求，获取响应对象
        SearchResponse response = client.search(request, RequestOptions.DEFAULT);
        //4.打印响应结果
        SearchHits hits = response.getHits();
        System.out.println(response);
    };

    public static void main(String[] args) {
        ConnectElasticsearch.connect(SEARCH_WITH_GROUP);
    }

}
```

控制台输出
```
{"took":10,"timed_out":false,"_shards":{"total":1,"successful":1,"skipped":0,"failed":0},"hits":{"total":{"value":6,"relation":"eq"},"max_score":1.0,"hits":[{"_index":"user","_type":"_doc","_id":"1001","_score":1.0,"_source":{"name":"zhangsan","age":"10","sex":"女"}},{"_index":"user","_type":"_doc","_id":"1002","_score":1.0,"_source":{"name":"lisi","age":"30","sex":"女"}},{"_index":"user","_type":"_doc","_id":"1003","_score":1.0,"_source":{"name":"wangwu1","age":"40","sex":"男"}},{"_index":"user","_type":"_doc","_id":"1004","_score":1.0,"_source":{"name":"wangwu2","age":"20","sex":"女"}},{"_index":"user","_type":"_doc","_id":"1005","_score":1.0,"_source":{"name":"wangwu3","age":"50","sex":"男"}},{"_index":"user","_type":"_doc","_id":"1006","_score":1.0,"_source":{"name":"wangwu4","age":"20","sex":"男"}}]},"aggregations":{"lterms#age_groupby":{"doc_count_error_upper_bound":0,"sum_other_doc_count":0,"buckets":[{"key":20,"doc_count":2},{"key":10,"doc_count":1},{"key":30,"doc_count":1},{"key":40,"doc_count":1},{"key":50,"doc_count":1}]}}}

Process finished with exit code 0
```
