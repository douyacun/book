#!/usr/bin/env bash
rsync -avz ./* douyacun:${dir}/book/
rsync -avz ./.git douyacun:${dir}/book/
ssh douyacun "${dir}/api.douyacun.com/bin/douyacun deploy -conf ${dir}/api.douyacun.com/configs/douyacun.yml -dir ${dir}/book/"