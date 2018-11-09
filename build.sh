#!/bin/bash

docker build -t plusminus/cmake:latest cmake/Dockerfile
docker build -t plusminus/gcc:latest gcc/Dockerfile
docker build -t plusminus/qt:latest qt/Dockerfile
