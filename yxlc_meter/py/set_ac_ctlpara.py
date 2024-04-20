# -*- coding: utf-8 -*-
"""
Created on Sat Jan 20 16:23:50 2024

@author: husts
"""

import requests
import json

src_server = 'http://10.8.0.30:8000'
#src_server = 'http://192.168.8.147:8000'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/ac/set_ctlpara'%(src_server)

src_s = json.dumps({"heattemp":15,"heatgap":2,"cooltemp":22,"coolgap":2})
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'},data=src_s)
print(src_r)