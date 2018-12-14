underline() {
    printf %s "$*" | sed -r 's/./&\x08_/g' | ul
    }
