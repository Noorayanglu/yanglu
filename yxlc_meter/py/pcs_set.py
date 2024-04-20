# -*- coding: utf-8 -*-
"""
Created on Thu Jan 18 07:23:49 2024

@author: husts
"""

import requests
import json

src_server = 'http://175.27.239.14:20004'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/pcs/set'%(src_server)

PCS_CMD_START = 0
PCS_CMD_STOP = 1
PCS_CMD_SET_APS = 2
src_s = json.dumps({"cmd":PCS_CMD_STOP, "param":0})
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'},data=src_s)
print(src_r)