#!/usr/bin/env bash
rsync -avz ./* douyacun:${dir}/book
ssh douyacun "${dir}/api.douyacun.com/bin/doyacun deploy -config ${dir}/api.douyacun.com/configs/douyacun.yml -dir ${dir}/book/"