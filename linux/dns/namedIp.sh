#!/bin/sh
a=`ifconfig eth1|grep -w inet|awk '{print $2}'|tr -d 'addr:'`
sed -i "s/127.0.0.1/$a/g" /var/named/douyacun.zone
