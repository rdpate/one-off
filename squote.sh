squote() {
    if [ -n "$1" ] && [ x"$(printf %s "$1" | tr -d -c \\055_/0-9A-Za-z)" = x"$1" ]; then
        printf %s "$1"
    else
        printf \'%s\' "$(printf %s "$1" | sed -r s/\'/\'\\\\\'\'/g)"
        fi
    }
