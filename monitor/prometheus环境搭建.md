---
Title: Prometheus Grafana docker基础环境搭建
Keywords: prometheus,docker环境
Description: docker-compose管理 prometheus和Grafana
Author: douyacun
Date: 2020-05-20 18:00:45
Label: Prometheus
LastEditTime: 2020-05-20 18:00:55
---

`docker-compose.md`

```yml
version: '2.2'
services:
  prometheus:
    image: prom/prometheus
    container_name: prometheus
    ports:
      - "9090:9090"
    volumes:
      - /Users/feifei/Documents/utils/docker/prometheus/prometheus.yml:/etc/prometheus/prometheus.yml
    networks:
      - prometheus_network
  grafana:
    image: grafana/grafana
    container_name: grafana
    ports:
      - 3000:3000
    networks:
      - prometheus_network
networks:
  prometheus_network:
    driver: bridge
```

docker 启动一个容器默认网络是bridge模式，各容器相互隔离，这里指定2个容器在统一网络内



`prometheus.yml`

```yml
# my global config
global:
  scrape_interval:     15s # Set the scrape interval to every 15 seconds. Default is every 1 minute.
  evaluation_interval: 15s # Evaluate rules every 15 seconds. The default is every 1 minute.
  # scrape_timeout is set to the global default (10s).

# Alertmanager configuration
alerting:
  alertmanagers:
  - static_configs:
    - targets:
      # - alertmanager:9093

# Load rules once and periodically evaluate them according to the global 'evaluation_interval'.
rule_files:
  # - "first_rules.yml"
  # - "second_rules.yml"

# A scrape configuration containing exactly one endpoint to scrape:
# Here it's Prometheus itself.
scrape_configs:
  # The job name is added as a label `job=<job_name>` to any timeseries scraped from this config.
  - job_name: 'prometheus'

    # metrics_path defaults to '/metrics'
    # scheme defaults to 'http'.

    static_configs:
    - targets: ['docker.for.mac.host.internal:9003']
```

docker 调用宿主机服务，使用 `docker.for.mac.host.internal` 作为host

