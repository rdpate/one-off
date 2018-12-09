exec >&2
fatal() { rc="$1"; shift; printf %s\\n "$*" >&2; exit "$rc"; }
case "$1" in
    all)
        redo-always
        exec redo-ifchange \
            exec-as \
            pipe-exec \
            ;
        ;;
esac
[ -e "$1".c ] || fatal 64 "unexpected target: $1"
redo-ifchange "$1".c
cc -O2 -o"$3" "$1".c
#strip "$3"
