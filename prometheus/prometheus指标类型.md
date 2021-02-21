---
Title: "prometheus指标类型"
Keywords: "指标类型用途以及实际用例"
Description: ""
Label: "prometheus指标类型"
---



- Counter 计数器
- Gauge 仪表盘
- Histogram 直方图
- Summary 摘要



**Counter 只增不减的计数器**

Counter类型的指标其工作方式和计数器一样，只增不减（除非系统发生重置）。常见的监控指标，如http_requests_total，node_cpu都是Counter类型的监控指标。推荐使用_total作为后缀。

rate获取http请求量的增长率

```
rate(http_requests_total[5m])
```

访问量前10的HTTP地址

```
topk(10, http_requests_total)
```



**Gauge: 可增加的仪表盘**

Gauge类型的指标反应系统的当前状态，当前可用的内存大小，空闲内存大小

计算CPU温度在2小时内的差异

```
delta(cpu_temp_celsius{host="zeus"}[2h])
```

预测系统磁盘空间在4小时之后的剩余情况

```
predict_linear(node_filesystem_free{job="node"}[1h], 4 * 3600)
```



**Histogram**



**Summary**

