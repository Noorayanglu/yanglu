# -*- coding: utf-8 -*-
"""
Created on Tue Feb  6 08:56:31 2024

@author: husts
"""

import requests
import json

src_server = 'http://175.27.239.14:20008'
#src_server = 'http://10.8.0.30:8000'
#src_server = 'http://192.168.8.147:8000'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/meter/get'%(src_server)
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'})
print(src_r.text)