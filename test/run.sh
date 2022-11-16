#!/bin/bash

cd ../
docker build . -t cat_grep_test:1.0 -f test/Dockerfile
docker run cat_grep_test:1.0
