#!/bin/bash

curl --header "PRIVATE-TOKEN: $DOWNLOAD_TOKEN" "https://gitlab.com/api/v4/projects/ubexs/clients/engine/jobs/artifacts/master/download?job=build" -o libopenblox.zip

unzip libopenblox.zip

sed "1cprefix=$(pwd)/artifacts" artifacts/lib/pkgconfig/libopenblox.pc > libopenblox.pc
