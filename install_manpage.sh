#!/bin/sh

mkdir -p /usr/local/share/man/en_US/man1/
cp doc/fileperuser.man /usr/local/share/man/en_US/man1/fileperuser.1
mandb
