#!/usr/bin/python3
"""
种子转磁力连接
"""

import libtorrent
import os
import sys

session = libtorrent.session()

if len(sys.argv) < 2:
    print("usage: python3 %s <torrent>" % (sys.argv[0]))
    os._exit(0)

info = libtorrent.torrent_info(sys.argv[1])
print("magnet:?xt=urn:btih:%s&dn=%s" % (info.info_hash(), info.name()))
