import collections
import fnmatch
import os
import pathlib
import re
import sys


def _path_type(path):
    if isinstance(path, pathlib.PurePath):
        return path
    elif isinstance(path, str):
        if not path:
            raise ValueError("empty path")
        return pathlib.PurePosixPath(path)
    elif isinstance(path, Pattern):
        return path._path
    else:
        raise TypeError("unknown path type")

def _match_any(name):
    return True


class Pattern:
    """glob matching of **, *, ?, [seq], and [!seq]

    Deals with relative, platform-neutral paths:
    - paths must be either strings or PurePath instances
        - including PurePath subclasses
    - string paths are converted to PurePosixPath and must use "/" component separators
        - special treatment for trailing slashes must be handled elsewhere, as pathlib ignores them
    - path.anchor must be empty

    Globs of "**" match zero or more path components.  Any component containing "**" which is not exactly "**" is an error.  For example, "**/xyz" and "a/**/c" are valid, while "a/**.c" is an error.

    Globs of "*" match zero or more characters in a path component.  Globs of "?" match one character in a path component.  Because both of these match characters in a path component, neither matches "/" component separators.

    Globs of "[seq]" and "[!seq]" match any character in "seq".  Ranges ("X-Y"), named classes ("[:name:]"), collating symbols ("[.ch.]"), and equivalence classes ("[=a=]") are not supported; additionally, "seq" may not contain "-" or "[" to allow future expansion.  To include "]" in "seq", it must be the first character.

    There is no way to quote meta-characters; however, "*", "?", and "[" are literal in a character class: "[*]", "[?]", "[[]".

    TODO: anchored paths
    TODO: trailing slashes
    TODO: better sequence handling
    """
    def __init__(self, path):
        path = _path_type(path)
        if path.anchor:
            raise ValueError("relative path required")
        if ".." in path.parts:
            raise ValueError("Patterns with .. unsupported")
        self._path = path
        # storing _match now abuses _translate to check syntax
        self._match = self._translate(path.as_posix())

    @staticmethod
    def _translate(path):
        parts = []
        end = "$"
        path = path.split("/")
        while path[-1] == "**":
            end = "(?:/|$)"
            path.pop()
            if not path:
                return _match_any
        prev = None
        for comp in path:
            if comp == "**":
                if prev != "**":
                    parts.append("(?:[^/]+/)*")
            elif "**" in comp:
                raise ValueError("invalid **")
            else:
                parts.append(Pattern._translate_comp(comp))
                parts.append("/")
            prev = comp
        parts.pop()
        return re.compile("".join(parts) + end).match

    @staticmethod
    def _translate_comp(comp):
        cre = []
        n = 0
        end = len(comp)
        while n < end:
            c = comp[n]
            n += 1
            if c == "*":
                cre.append("[^/]*")
            elif c == "?":
                cre.append("[^/]")
            elif c == "[":
                c = comp[n:n + 1]
                if c == "!":
                    cre.append("[^")
                    n += 1
                elif c == "]":
                    cre.append(r"[]")
                    n += 1
                else:
                    cre.append("[")
                e = comp.find("]", n)
                if e == -1:
                    raise ValueError("invalid [seq]")
                seq = comp[n:e]
                n = e + 1
                if "-" in seq or "[" in seq:
                    raise ValueError("invalid [seq]")
                seq = seq.replace("\\", r"\\")
                if seq.startswith("^"):
                    seq = "\\" + seq
                cre.append(seq + "]")
            else:
                cre.append(re.escape(c))
        return "".join(cre)

    def as_posix(self):
        return self._path.as_posix()

    def match(self, path):
        path = _path_type(path)
        return bool(self._match(path.as_posix()))

    def __truediv__(self, other):
        if isinstance(other, Pattern):
            other = other._path
        return Pattern(self._path / other)

    def __rtruediv__(self, other):
        return Pattern(other) / self

    def __repr__(self):
        path = self._path
        if isinstance(path, pathlib.PurePosixPath):
            # intentionally include pathlib.PosixPath as subclass of PurePosixPath
            path = str(path)
        return "Pattern({!r})".format(self._path)


def glob(pattern):
    if not isinstance(pattern, Pattern):
        pattern = Pattern(pattern)
    # TODO
    raise NotImplementedError()
