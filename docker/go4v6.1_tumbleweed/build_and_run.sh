
name=$(basename $(pwd))

docker build -t $name . || exit

docker run --net host -v $(pwd)/conf:/conf -v $(pwd)/workdir:/workdir --rm -it \
 --name $name  \
 $name /workdir/start.sh

# docker run --net host -v $(pwd)/conf:/conf -v $(pwd)/workdir:/workdir --rm -it \
# --name $name --user $(id -u):$(id -g) \
# $name /workdir/start.sh

