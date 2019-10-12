#!/usr/bin/env bash

rm -rf .git/
rsync ./* douyacun:${deploy_dir}
ssh douyacun "export _DOUYACUN_CONF='${deploy_dir}/configs/douyacun.yml' && /data/www/api.douyacun.com/bin/douyacun deploy -dir ${deploy_dir}"