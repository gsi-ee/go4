
name=$(basename $(pwd))

docker build -t $name . || exit

docker run --net host -v $(pwd)/conf:/conf -v $(pwd)/workdir:/workdir --rm -it \
--name $name  \
$name /bin/bash

