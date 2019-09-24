---
Title: ci_gitlab_自动部署 
Keywords: ci,gitlab,自动部署
Description: gitlab ci自动部署
Author: douyacun
Date: 2018-10-30 15:31:59
LastEditTime: 2019-02-02 17:08:26
---
# gitlab ci部署

ubuntu
```
curl -L https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.deb.sh | sudo bash
apt-get install gitlab-runner
```

centos
```
curl -L https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.rpm.sh | sudo bash
yum install gitlab-runner
```

## 注意点
- 不要用gitlab提供的公共runner，不安全， project/setting/`CI/CD`/Runners/  ： `Disable shared Runners`
- 在自己服务器上部署`gitlab runners`, gitlab-runner register, 成功之后可以在roject/setting/`CI/CD`/Runners 下的 `Runners activated for this project` 看到注册成功的runners，如果runners不是绿点，看下面, register时：
  + Specify the following URL during the Runner setup： `https://gitlab.com/`
  + Use the following registration token during setup: `project/setting/`CI/CD`/Runners/` 下面有
  + tag： 这个是指名含有该tag的`runner jobs`才会执行，完整实践之后会有了解
- 配置项目拉取公钥，`project/setting/Repository/Deploy Keys`，这个在gitlab-runner register之后会在服务器上生成gitlab-runner用户，ssh-keygen会生成需要密钥， 配置到前面的 `Deploy keys`中就行了
- `.gitlab-ci.yml` 的作用不多说， google搜一下一堆

## 贴一下 `.gitlab-ci.yml`
```
#定义stage：程序运行阶段
image: dychub/nvm
before_script:
    - touch ~/.ssh/id_rsa && chmod 600 ~/.ssh/id_rsa
    - echo "$SSH_PRIVATE_KEY" > ~/.ssh/id_rsa
    - echo -e "Host local\n\thostname 192.168.0.1\n\tport 22\n\tuser dev\n\tIdentityFile  ~/.ssh/id_rsa\n\tStrictHostKeyChecking no\n\tUserKnownHostsFile /dev/null" > ~/.ssh/config
    - cat /etc/issue
stages:
- deploy

cache:
  key: ${CI_COMMIT_REF_NAME}
  paths:
  - node_modules/

deploy to production:
  stage: deploy
  script:
    - . ~/.nvm/nvm.sh
    - nvm use default
    - cnpm install
    - npm run build
    - rsync -avzr build/ local:/var/test/
  only:
  - develop
```

## 总结
1. runner不管是shell还是docker， 都是运行在Linux系统上的，归根结底都是shell，docker可能面临: docker和服务器文件传递的问题， 就是采用上面的 *before_script:* 中的 ssh 配置，记得安装`openssl-clients`