#!/usr/bin/env bash
rsync -avz ./* douyacun:${deploy_dir}
ssh douyacun "export _DOUYACUN_CONF='${config_file}' && ${douyacun} deploy -dir ${deploy_dir}"