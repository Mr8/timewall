#!/bin/bash

BINDIR=./bin

make test

ls $BINDIR | grep test_ | while read binfile
do
    $BINDIR/$binfile
done
