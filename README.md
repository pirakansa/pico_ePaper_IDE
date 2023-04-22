# pico_ePaper_IDE

## build to uf2

After setting up the environment in .devcontainer, follow these steps:

```sh
$ git clone https://github.com/waveshare/Pico_ePaper_Code.git epaper
$ mkdir -p build && cmake -S . -B ./build
$ make -C ./build
```


## mono bitmap to ImageData

Display the data described in ImageData using the following steps:

```sh
$ sudo docker run --rm -it -v `pwd`:/work ubuntu bash
$ apt update && \
    DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends python3 python3-pip libopencv-dev && \
    pip install opencv-python
$ ./bmp2Monopic.py ./Lenna.bmp | sed s/\'//g | sed s/]/\\n}\;/g | sed s/\\[/{\\n/g
```

