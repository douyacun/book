#!/usr/bin/env bash

rm -rf .git/
rsync ./* douyacun:${deploy_dir}
ssh douyacun "/data/www/api.douyacun.com/bin/douyacun deploy -dir ${deploy_dir}"