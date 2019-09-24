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
