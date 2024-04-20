# -*- coding: utf-8 -*-
"""
Created on Wed Jan 17 20:51:00 2024

@author: husts
"""

import requests
import json

src_server = 'http://192.168.8.147:8000'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/settings/set'%(src_server)

src_s = json.dumps({"device_name":"mason", "log_enabled":True,"brightness":3})
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'}, data=src_s)
print(src_r)