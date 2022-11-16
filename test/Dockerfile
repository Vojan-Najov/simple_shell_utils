FROM quay.io/school21/alpine:3.16

RUN apk --no-cache --upgrade add \
    git \
    git-lfs \
    curl \
    jq \
    bash \
    build-base \
    python3 \
    valgrind \
    cppcheck \
    alpine-sdk \
    pcre-dev \
    clang-extra-tools \
	coreutils \
	grep

RUN mkdir /test 

COPY . /project
#COPY materials/build/scripts /project/ci-scripts
#COPY materials/build/scripts /project/tests
#COPY materials/linters /project/tests/linters

WORKDIR "/project/test"

ENTRYPOINT ["/bin/bash", "start_test.sh"]
