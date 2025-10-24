curl -X PUT "http://10.1.1.2:9200/.kibana/_settings" \
  -H "Content-Type: application/json" \
  -d '{
    "index": {
      "blocks.read_only_allow_delete": null
    }
  }'
