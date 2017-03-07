 #!/bin/bash

set -e

SCRIPTSDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTSDIR/frameworks

docker build -t chigraph/frameworks-build .

DOCKER_PROCESS=$(docker run -d chigraph/frameworks-build bash -c 'while true; do sleep 1000; done')

docker exec -t $DOCKER_PROCESS bash -c 'cd / && git clone https://github.com/chigraph/chigraph --depth=1'
docker exec -t $DOCKER_PROCESS /chigraph/scripts/frameworks/build_frameworks.sh

