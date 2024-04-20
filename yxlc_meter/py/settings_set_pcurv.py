# -*- coding: utf-8 -*-
"""
Created on Mon Jan 22 23:12:21 2024

@author: husts
"""

import requests
import json

src_server = 'http://175.27.239.14:20000'
#src_server = 'http://10.8.0.30:8000'
#src_server = 'http://192.168.8.147:8000'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/settings/set'%(src_server)

# SETTINGS_CMD_SET_CHG_CELLV = 0,
# SETTINGS_CMD_SET_DHG_CELLV = 1,
# SETTINGS_CMD_SET_PCURV = 2
d = {"cmd":2}
for i in range(96):
    d["%d"%(i+1)] = 30
src_s = json.dumps(d)
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'},data=src_s)
print(src_r)