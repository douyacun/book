---
Title: linux-设置使用wifi
LegacyId: 157396dc0268dae9bd9c1dd975b748e6
topic: devops
Keywords:
Label:
description: "- iwlist wlan0 scan 扫描网络 - 得安装 wpa_supplicant ``` # wpa_passphrase [wifi] [password] /etc/my_wpa_supplicant.conf 会生成 net"
Date: 2019-09-24T20:04:29+08:00
LastEditTime: 2021-01-14T20:53:34+08:00
Publish: false
Lang: zh
---

# linux 设置使用wifi


- iwlist wlan0 scan   扫描网络



- 得安装 wpa_supplicant
    ```
    # wpa_passphrase [wifi] [password]

    /etc/my_wpa_supplicant.conf
    会生成
    network={
            ssid="yourWifi"
            psk="abce1234"
            psk=xxxxxxxxxxxxxxxxxxxxx
    }

    # wpa_passphrase [wifi] [password] >
    ```
