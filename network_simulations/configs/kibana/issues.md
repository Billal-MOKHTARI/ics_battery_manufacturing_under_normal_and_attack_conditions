### Issue 1
When the switch is directly connected to another interface of Logstash, the communication fails. To resolve this issue, I connected the Cisco switch to an Ethernet switch, which is then connected to the switch where the Logstash server is attached.

### Issue 2
If there is an error in creating the **Index Pattern**, you should follow these instructions:
1. [http://192.168.122.217:5603/app/kibana#/dev_tools/console?_g=()](http://@ip:5603/app/kibana#/dev_tools/console?_g=()) (replace `@ip` by your Kibana server ipv4 address).
2. Run the script `unlock_kibana_index.sh` located in the `BaTTwin/network_simulations/configs/docker_containers/kibana/scripts`.


### Bugs
```log
log   [09:04:43.436] [warning][telemetry] Error scheduling task, received index [.kibana_task_manager_1] blocked by: [FORBIDDEN/12/index read-only / allow delete (api)];: [cluster_block_exception] index [.kibana_task_manager_1] blocked by: [FORBIDDEN/12/index read-only / allow delete (api)];
  log   [09:04:43.438] [warning][maps] Error scheduling telemetry task, received index [.kibana_task_manager_1] blocked by: [FORBIDDEN/12/index read-only / allow delete (api)];: [cluster_block_exception] index [.kibana_task_manager_1] blocked by: [FORBIDDEN/12/index read-only / allow delete (api)];
```

### Resolve bugs

```bash
PUT .kibana_1/_settings
{
  "index.blocks.read_only_allow_delete": false,
  "index.blocks.read_only": false
}

PUT .kibana_1/_settings
{
  "index.blocks.read_only_allow_delete": false
}
GET .kibana_1/_settings
```

Wanted result

```json
{ ".kibana_1" : { "settings" : { "index" : { "number_of_shards" : "1", "auto_expand_replicas" : "0-1", "blocks" : { "read_only_allow_delete" : "false", "read_only" : "false" }, "provided_name" : ".kibana_1", "creation_date" : "1738328416151", "number_of_replicas" : "0", "uuid" : "zS_FHfxaSZe85wDBxQsyIw", "version" : { "created" : "7040299" } } } } }

```