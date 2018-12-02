exec >&2
redo-ifchange exec-as.c
cc -o"$3" exec-as.c
