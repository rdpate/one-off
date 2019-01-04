set -Cue
exec >&2
fatal() { rc="$1"; shift; printf %s\\n "${0##*/} error: $*" >&2 || true; exit "$rc"; }

case "$1" in
all)
    redo-always
    find . -name '*.link' | sed -r 's/\.link$//' | xargs redo-ifchange
    exit 0
    ;;
esac

target="$1"
output="$3"
link="$target.link"
reldir="$(dirname "$link")"
[ -e "$link" ] || fatal 66 "unknown target $1"
redo-ifchange "$link"
exec <"$link"
set -- "$target.o"
while read -r line; do
    case "$line" in
        '#'*|'') continue ;;
        [-/]*) ;;
        *) line="$reldir/$line" ;;
        esac
    set -- "$@" "$line"
    done
for x; do
    case "$x" in -*) continue ;; esac
    printf %s\\0 "$x"
    done | xargs -0 redo-ifchange
cc -o"$output" "$@"
