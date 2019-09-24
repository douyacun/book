# "vboxsf" is not available

vagrant添加高级组件
```
vagrant plugin install vagrant-vbguest
```

虚拟机中
```
ll -ah /opt/VBoxGuestAdditions-5.1.20/other/
ll -ah /opt/VBoxGuestAdditions-5.1.20/lib/VBoxGuestAdditions/mount.*

ln -sf /opt/VBoxGuestAdditions-5.2.8/other/mount.vboxsf /sbin/mount.vboxsf
```


# 使用root登录
```
# 登录虚拟机
vagrant ssh
# 在虚拟机中把vagrant用户ssh key拷贝到root用户下
sudo cp -r ./.ssh/ /root/
sudo chown -R root.root /root/.ssh
exit

# 修改Vagrantfile中登录用户名
config.ssh.username = 'root'

# 重新reload
vagrant reload

# 登录后默认用户为root
vagrant ssh
```

# vagrant plugin 慢
install-vagrant-vbguest.sh
```shell
#!/usr/bin/env bash

GEMS=$(cat <<EOF
childprocess-0.5.8.gem
ffi-1.9.10.gem
little-plugger-1.1.4.gem
micromachine-1.1.0.gem
multi_json-1.11.1.gem
net-ssh-2.9.2.gem
rest-client-1.6.9.gem
vagrant-vbguest-0.11.0.gem
EOF
)

CACHE=$HOME/.vagrant.d/gems/cache
mkdir -p $CACHE

for GEM in $GEMS
do
    URL=https://ruby.taobao.org/gems/$GEM
    echo "Downloading: $URL"
    curl --progress-bar -o $CACHE/$GEM -L $URL
done

vagrant plugin install vagrant-vbguest
```