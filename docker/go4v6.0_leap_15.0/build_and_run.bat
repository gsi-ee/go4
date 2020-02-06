
set name=go4v6.0_leap_15.0

docker build -t %name% . || exit

docker run -p 5903:5903 -v %CD%\conf:/conf -v %CD%\workdir:/workdir --rm -it  --name %name%  %name% /workdir/start.sh


