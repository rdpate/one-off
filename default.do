exec >&2
fatal() { rc="$1"; shift; printf %s\\n "$*" >&2; exit "$rc"; }
case "$1" in
    all)
        redo-always
        find . -name '*.c' | sed -r 's/\.c$//' | xargs redo-ifchange
        exit 0
        ;;
    esac
[ -e "$1.c" ] || fatal 64 "unexpected target: $1"
redo-ifchange "$1.c"
dep=".$1.dep"
cc -MD -MF "$dep" -O2 -o"$3" "$1.c"
sed -r 's/^[^ ]+: //; s/ \\$//; s/^ +| +$//g; s/ +/\n/g; /^$/d' | \
xargs redo-ifchange
rm "$dep"
#strip "$3"
