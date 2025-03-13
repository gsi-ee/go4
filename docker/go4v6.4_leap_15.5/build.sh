
name=$(basename $(pwd))

docker build -t $name . || exit


