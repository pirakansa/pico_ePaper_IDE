FROM alpine:3.17

RUN apk update && \
    apk upgrade && \
    apk add git \
            cmake \
            python3 \
            build-base \
            bsd-compat-headers \
            newlib-arm-none-eabi \
            gcc-arm-none-eabi \
            su-exec && \
    chmod +s /sbin/su-exec && \
    adduser builder -u 1000 -D

ARG SDK_PATH=/usr/share/pico_sdk
RUN git clone --depth 1 --branch 1.5.0 https://github.com/raspberrypi/pico-sdk $SDK_PATH && \
    cd $SDK_PATH && \
    git submodule update --init

ENV PICO_SDK_PATH=$SDK_PATH

USER builder

