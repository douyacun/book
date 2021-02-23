---
Title: "prometheus收集主机数据"
Keywords: "prometheus,grafana,node_exporter"
Description: "使用node_exporter采集主机数据"
Label: "node_exporter"
Cover: "assert/prometheus-node-exporter.jpg"
---

![image-20210223212719619](assert/image-20210223212719619.png)

prometheus提供了node_exporter来收集主机数据

node_exporter下载地址: [https://github.com/prometheus/node_exporter/releases](https://github.com/prometheus/node_exporter/releases)

supervisor维护进程运行

```ini
[program:node_exporter]
command=/data/node_exporter/node_exporter-1.1.1.linux-amd64/node_exporter
autostart=true
autorestart=true
user=root
numprocs=1
redirect_stderr=true
stdout_logfile=/data/node_exporter/logs/access.log
```

grafana dashboard: [https://grafana.com/grafana/dashboards/8919](https://grafana.com/grafana/dashboards/8919)

