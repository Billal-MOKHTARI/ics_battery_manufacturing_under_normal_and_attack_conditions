PUT .kibana/_settings
{
  "index": {
    "blocks.read_only_allow_delete": false
  }
}

PUT .kibana_1/_settings
{
  "index.blocks.read_only_allow_delete": false,
  "index.blocks.read_only": false
}