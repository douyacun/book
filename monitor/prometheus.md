---
Title: 
Keywords: 
Description: 
Author: douyacun
Date: 2020-05-20 18:00:45
Label: 
LastEditTime: 2020-05-20 18:00:55
---


`docker-composer.md`
```yml
version: '2.2'
services:
  prometheus:
    image: prom/prometheus
    container_name: prometheus
    ports:
      - "9090:9090"
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml
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