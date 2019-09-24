## mac安装php

[One Line Installation](https://php-osx.liip.ch/)

All you need to do to install the whole thing is opening up a Shell and execute the following line:

PHP 7.1 (Current stable) - 10.10 and later

curl -s https://php-osx.liip.ch/install.sh | bash -s 7.1
PHP 7.0 (Old stable) - 10.10 and later

curl -s https://php-osx.liip.ch/install.sh | bash -s 7.0
PHP 5.6 (Old stable) - 10.8 and later

curl -s https://php-osx.liip.ch/install.sh | bash -s 5.6


## mac 安装composer
```
brew update
brew tap josegonzalez/homebrew-php
brew tap homebrew/versions
brew install php55-intl
brew install josegonzalez/php/composer
```

## 如果已经安装 `php5.6`

```blade
curl -s https://php-osx.liip.ch/install.sh | bash -s 7.1

export PATH=/usr/local/php5/bin:$PATH
```
