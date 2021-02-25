```
Complete output from command python setup.py egg_info:
Traceback (most recent call last):
	File "<string>", line 1, in <module>
	File "/tmp/pip-build-qsfss06d/opencv-python/setup.py", line 10, in <module>
		import skbuild
ModuleNotFoundError: No module named 'skbuild'
```

python3 升级

```
pip3 install --upgrade pip
```

python2.7

```
pip install --upgrade pip
```



pip 代理

`~/.pip/pip.conf`

```
[global]
trusted-host =  mirrors.aliyun.com
index-url = https://mirrors.aliyun.com/pypi/simple
```

