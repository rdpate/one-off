#!/bin/sh -ue
# markdown [OPTIONS] [FILE..]
#
# Convert markdown from FILEs (or stdin) to html5.
#
# Options:
#     --doctype         add "<!doctype html>"
#     --title=TITLE     add title before all input
#     --html=MODE       "raw" (default), "remove", or "escape" HTML tags
#     --quiet           suppress all warnings
#     --verbose         print all warnings

# TODO: write or modify punctuation extension; "abc -- def" should become "abc&mdash;def", at least for me ("abc - def" would become "abc &ndash; def")
#   may need to re-write a markdown parser
# TODO: maybe add option for extensions (markdown module's -x option)

fatal() {
    echo "markdown: error: $2" >&2
    exit $1
}

doctype=false
title=
html=
noise=
handle_option() {
    case "$1" in
        doctype)
            [ $# = 1 ] || fatal 64 'unexpected --doctype value'
            doctype=true;;
        title)
            [ $# = 2 ] || fatal 64 'missing --title value'
            title="$2";;
        html)
            [ $# = 2 ] || fatal 64 'missing --html value'
            case "$2" in
                raw)
                    html=;;
                remove|escape)
                    html="$2";;
                *)
                    fatal 64 'unknown --html value';;
            esac;;
        quiet|verbose)
            [ $# = 2 ] && fatal 64 "unexpected --$1 value"
            noise="--$1";;
        *)
            fatal 64 "unknown option: $1";;
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

if $doctype; then
    printf %s\\n '<!doctype html>'
fi
if [ -n "$title" ]; then
    printf %s%s%s\\n '<title>' "$(printf %s "$title" | sed 's/&/\&amp;/g; s/</\&lt;/g')" '</title>'
fi
for x; do
    if ! [ -e "$x" ]; then
        fatal 66 "no such file: $x"
    elif [ -d "$x" ]; then
        fatal 66 "unexpected directory: $x"
    fi
done
cat -- "$@" | python3 -mmarkdown \
    $noise \
    ${html:+"--safe=$html"} \
    -eutf-8 -ohtml5 \
    -xmarkdown.extensions.smarty \
    -xmarkdown.extensions.tables \
    --no_lazy_ol
echo
