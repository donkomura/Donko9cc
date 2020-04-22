# 9cc

a small C compiler 

## docker

```bash
$ docker build -t compilerbook https://www.sigbus.info/compilerbook/Dockerfile
```

## Test on container

```bash
$ docker run --rm -v $(pwd):/9cc -w /9cc compilerbook make test
```


## Reference
https://www.sigbus.info/compilerbook#%E8%91%97%E8%80%85%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6)
