#!/usr/bin/env bash
# TODO: write or modify punctuation extension; "abc -- def" should become "abc&mdash;def"
#   may need to re-write a markdown parser

set -ue
fatal() {
    echo "$(basename "$0"): error: $2" >&2
    exit $1
}
title=
safe=
fragment=false
markdown_opts=()
handle_option() {
    case "$1" in
        help)
            cat <<'EOF'
markdown [OPTIONS] [FILE..]

Options:
 -t --title=TITLE   add title
    --safe=MODE     "replace", "remove", "escape" HTML tags in input;
                    defaults to "unsafe"
    --quiet         suppress all warnings
    --verbose       print all warnings
EOF
            exit 0;;
        t|title)
            [ $# = 2 ] || fatal 1 "missing value for $1 option"
            title="$2";;
        safe)
            case "$2" in
                replace|remove|escape)
                    safe="$2";;
                unsafe)
                    safe=;;
                *)
                    fatal 1 'unknown value for safe option';;
            esac;;
        quiet|verbose)
            [ $# = 2 ] && fatal 1 "unexpected value for $1 option"
            markdown_opts+=("--$1");;
        *)
            fatal 1 "unknown option: $1";;
    esac
}
while [ $# -gt 0 ]; do
    case "$1" in
        --)
            shift
            break;;
        --*=*)
            x="${1#--}"
            handle_option "${x%%=*}" "${1#*=}"
            shift;;
        --*)
            handle_option "${1#--}"
            shift;;
        -?*)
            if [ ${#1} = 2 ]; then
                handle_option "${1#-}"
            else
                handle_option "$(printf %s "$1" | cut -b2)" "${1#??}"
            fi
            shift;;
        *)
            break;;
    esac
done

if [ -n "$safe" ]; then
    markdown_opts+=(--safe="$safe")
fi
markdown_opts+=(
    -eutf-8
    -ohtml5
    -xmarkdown.extensions.{smarty,tables}
    --no_lazy_ol
    )
if [ -n "$title" ]; then
    printf %s '<title>'
    printf %s "$title" | sed 's/&/\&amp;/g; s/</\&lt;/g'
    printf %s\\n '</title>'
fi
cat -- "$@" | python3 -mmarkdown "${markdown_opts[@]}"
echo
