#!/usr/bin/env bash
#
# Copyright (C) 2023 Ethan Uppal. All rights reserved.
#
# File: install/unix.sh
# Installs libconfig on Unix-like systems. Pass -lconfig to use.

git clone https://github.com/ethanuppal/config
cd config
make install && \
    cd ..; rm -rf config
