 #!/bin/bash

set -e

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/frameworks

docker build -t chigraph/frameworks-build .

DOCKER_PROCESS=$(docker run -d chigraph/frameworks-build bash -c 'while true; do sleep 1000; done')

docker exec -t $DOCKER_PROCESS bash -c 'cd / && git clone https://github.com/chigraph/chigraph --depth=1'
docker exec -t $DOCKER_PROCESS bash -c 'source /opt/qt56/bin/qt56-env.sh && /chigraph/scripts/frameworks/build_frameworks.sh Debug -GNinja'
docker exec -t $DOCKER_PROCESS bash -c 'source /opt/qt56/bin/qt56-env.sh && /chigraph/scripts/frameworks/build_frameworks.sh Release -GNinja'
docker exec -t $DOCKER_PROCESS bash -c 'cd /chigraph/third_party && tar cJf kf5-debug-gcc-linux64.tar.xz kf5-debug'
docker exec -t $DOCKER_PROCESS bash -c 'cd /chigraph/third_party && tar cJf kf5-release-gcc-linux64.tar.xz kf5-release'

docker cp ${DOCKER_PROCESS}:/chigraph/third_party/kf5-debug-gcc-linux64.tar.xz ../
docker cp ${DOCKER_PROCESS}:/chigraph/third_party/kf5-release-gcc-linux64.tar.xz ../
