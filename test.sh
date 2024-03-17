#!/bin/bash
# chmod u+x test.sh

sudo make uninstall \
    && sudo make install \
    && cd test \
    && make build_test \
    && DYLD_LIBRARY_PATH=/usr/local/lib ./main "$@"
