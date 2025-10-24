In this document, I will be showing you how to correctely configure the network simulation on GNS3.


### Install ELK on the main server

1. Run `install_elasticsearch.sh`
2. Run `install_kibana.sh`
3. Run `install_logstash`

To correctely run logstash, we should first throw a look at the two configuration files:
- `/etc/logstash/logstash.yml`: settings files which specify options that control Logstash startup and execution.
- `/etc/logstash/pipelines.yml`: pipeline configuration files.

`/etc/logstash/pipelines.yml` contains two parameters, `pipeline.id` and `path.config` which is set, by default, to `/etc/logstash/conf.d/*.conf`. This is where the pipelines should be put.

The pipeline configuration file should be written as follows:

```conf

input { 
  ...
}

filter {
  ...
}

output {
  ...
  elasticsearch {
    hosts => ["https://localhost:9200"]
    user => "elasticsearch"
    password => "your_password" 
    ssl => true 
    cacert => "/etc/elasticsearch/certs/http_ca.crt"
  }
  stdout { codec => rubydebug }
}

```

4. Run `start_elk.sh`
5. Verify that `logstash` is running using this command line `sudo systemctl status logstash.service`.
6. Copy this line on the same terminal in which the logstash is started `127.0.0.1 - - [11/Dec/2013:00:01:45 -0800] "GET /xampp/status.php HTTP/1.1" 200 3891 "http://cadenza/xampp/navi.php" "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:25.0) Gecko/20100101 Firefox/25.0"`.


If there is an error connecting to Kibana server, retart it using this command line : `sudo systemctl restart kibana.service` or `sudo systemctl retart kibana`.