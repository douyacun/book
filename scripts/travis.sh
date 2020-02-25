#!/usr/bin/env bash
rsync -avz $(pwd)/ douyacun:${dir}/book/
ssh douyacun "${dir}/api.douyacun.com/bin/douyacun deploy -conf ${dir}/api.douyacun.com/configs/douyacun.yml -dir ${dir}/book/"