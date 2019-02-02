#!/bin/sh -Cue

squote() {
    while [ $# != 0 ]; do
        if [ -n "$1" ] && [ x"$(printf %s "$1" | tr -d -c \\055_/0-9A-Za-z)" = x"$1" ]; then
            printf %s${2+' '} "$1"
        else
            printf \'%s\'${2+' '} "$(printf %s "$1" | sed -r s/\'/\'\\\\\'\'/g)"
            fi
        shift
        done
    }

squote "$@"
printf \\n
