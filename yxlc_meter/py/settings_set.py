# -*- coding: utf-8 -*-
"""
Created on Sun Jan 21 15:00:43 2024

@author: husts
"""

import requests
import json

src_server = 'http://175.27.239.14:20010'
#src_server = 'http://10.8.0.30:8000'
#src_server = 'http://192.168.8.147:8000'
#src_server = 'http://192.168.2.136:8000'
src_req_url = '%s/api/settings/set'%(src_server)

SETTINGS_CMD_SET_CHG_CELLV = 0
SETTINGS_CMD_SET_DHG_CELLV = 1
SETTINGS_CMD_SET_PCURV = 2
SETTINGS_CMD_SET_CHGGATELIM = 3,
SETTINGS_CMD_SET_CHGTRANSLIM = 4,
SETTINGS_CMD_SET_DHGGATELIM = 5,
SETTINGS_CMD_SET_DHGTRANSLIM = 6,
SETTINGS_CMD_SET_SN = 100,
SETTINGS_CMD_SET_TEST = 1000
src_s = json.dumps({"cmd":7,"param":"3dcf2052332a4b0c8e9a8c8015a3c353"})
src_r = requests.get(src_req_url, headers={'Accept': 'application/json'},data=src_s)
print(src_r)