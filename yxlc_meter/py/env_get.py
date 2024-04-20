# -*- coding: utf-8 -*-
"""
Created on Wed Jan 17 15:54:13 2024

@author: husts
"""

import requests
import json

src_server = 'http://175.27.239.14:20010'
src_req_url = '%s/api/env/get'%(src_server)
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'})
print(src_r.text)