# -*- coding: utf-8 -*-
"""
Created on Thu Jan 18 08:48:34 2024

@author: husts
"""

import requests
import json

src_server = 'http://10.8.0.30:8000'
#src_server = 'http://192.168.8.147:8000'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/env/set_ledmod'%(src_server)

src_s = json.dumps({"mode":4})
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'},data=src_s)
print(src_r)