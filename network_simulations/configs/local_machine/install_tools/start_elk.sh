sudo systemctl start elasticsearch.service
sudo systemctl start kibana.service
sudo /usr/share/logstash/bin/logstash -f /etc/logstash/conf.d/logstash.conf --path.settings /etc/logstash
