# -*- coding: utf-8 -*-
"""
Created on Thu Jan 18 07:41:01 2024

@author: husts
"""

import requests
import json

src_server = 'http://192.168.8.147:8000'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/ac/set_man'%(src_server)

src_s = json.dumps({"mode":2,"temp":16})
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'},data=src_s)
print(src_r)