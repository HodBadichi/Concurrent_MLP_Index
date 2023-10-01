#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_DIR="$(realpath "$SCRIPT_DIR/../..")"

function Build() {
    LOGFILE="$PROJECT_DIR/Code/Utils/userspace-rcu-lib/log.txt"
    echo "Cloning urcu library.."
    rm -fr ${PROJECT_DIR}/Code/Utils/userspace-rcu
    eval "git clone git://git.liburcu.org/userspace-rcu.git ${PROJECT_DIR}/Code/Utils/userspace-rcu"
    if [ $? -ne 0 ]; then
         echo "Cloning userspace-rcu-lib failed."
         exit 1
    fi
    echo "Going to install user space urcu library..."
    rm -fr ${PROJECT_DIR}/Code/Utils/userspace-rcu-lib
    eval "mkdir -p $PROJECT_DIR/Code/Utils/userspace-rcu-lib"
    eval "cd $PROJECT_DIR/Code/Utils/userspace-rcu" >> $LOGFILE 2>&1
    make clean >> $LOGFILE 2>&1
    ./bootstrap >> $LOGFILE 2>&1
    echo "Configuring urcu library...(this might take some time)"
    ./configure --prefix ${PROJECT_DIR}/Code/Utils/userspace-rcu-lib >> $LOGFILE 2>&1
    make >> $LOGFILE 2>&1
    make install >> $LOGFILE 2>&1
    if [ $? -ne 0 ]; then
         echo "Building userspace-rcu-lib failed. please check ${LOGFILE} for details"
         exit 1
    fi
    echo "urcu library installed sucessfuly !"

    echo "Going to start build..."
    cd $PROJECT_DIR
    mkdir build
    cd build
    echo "Running CMake..."
    cmake ..
    if [ $? -ne 0 ]; then
         echo "Running CMake failed. Please see ${LOGFILE} for details."
         exit 1
    fi
    echo "Done executing: CMake"
    echo "Running Make..."
    make
    if [ $? -ne 0 ]; then
         echo "Running Make failed"
         exit 1
    fi
    echo "Done executing: 'make'"
    echo "Completed building the project!"
}

Build