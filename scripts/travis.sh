#!/usr/bin/env bash
ls -lah
rsync -avz ./* douyacun:${dir}/book/
ssh douyacun "${dir}/api.douyacun.com/bin/douyacun deploy -conf ${dir}/api.douyacun.com/configs/douyacun.yml -dir ${dir}/book/"