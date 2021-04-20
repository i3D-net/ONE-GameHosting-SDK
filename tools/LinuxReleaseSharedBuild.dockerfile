FROM ubuntu:18.04

RUN apt update \
    && apt install build-essential -y \
    && apt-get install cmake -y

WORKDIR /one

COPY . ./

RUN find . -type f -exec sed -i 's/\r$//g' {} +

ARG OutputFolder=lib_build

RUN if [ -d ${OutputFolder} ]; then rm -Rf ${OutputFolder}; fi \
    && mkdir ${OutputFolder} \
    && mkdir ${OutputFolder}/x86 \
    && mkdir ${OutputFolder}/x64

WORKDIR /one/tools

RUN if [ -d ../build ]; then rm -Rf ../build; fi \
    && bash build_release_linux_so_32.sh \
    && cp ../build/one/arcus/libone_arcus.so ../${OutputFolder}/x86 \
    && rm -Rf ../build \
    && bash build_release_linux_so_64.sh \
    && cp ../build/one/arcus/libone_arcus.so ../${OutputFolder}/x64