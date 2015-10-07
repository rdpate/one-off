import functools
import sys

import doublestar

@functools.lru_cache()
def compile(pattern):
    return doublestar.Pattern(pattern)
def match(pattern, path):
    return compile(pattern).match(path)

def test(expected, *args):
    actual = match(*args)
    if expected == actual:
        print(".", end="")
        test.need_newline = True
    else:
        test.failed += 1
        if test.need_newline:
            print()
            test.need_newline = False
        print("fail: match({!r}, {!r})".format(*args))
    test.count += 1
test.count = 0
test.failed = 0
def status():
    if test.need_newline:
        print()
    if test.failed:
        print("failed: {:,} of {:,}".format(test.failed, test.count))
        return 1

def main():
    test(True, "a", "a")
    test(True, "a", "a/")
    test(True, "a/", "a")
    test(True, "a/.", "a")
    test(True, "a", "a/.")

    test(True, "**", "a")
    test(True, "**", "a/.")
    test(True, "**", "a/b/c/d")
    test(True, "**/d", "a/b/c/d")
    test(True, "**/b/**/**/c/**", "a/b/c/d")
    test(True, "**/b/**/**/c/**", "a/b/c")
    test(True, "a/**", "a")
    test(True, "a/**", "a/b")
    test(True, "**", "a/b")
    test(False, "**/c", "a/b")
    test(True, "?b?", "abc")
    test(True, "*b*", "abc")
    test(True, "*b*", "b")
    test(False, "a?b?c", "a/b/c")
    test(True, "a?b?c", "axbxc")

    # TODO: trailing slash
    # test(False, "**/b/**/**/c/**", "a/b/c/")
    # test(False, "a/**", "a/")
    # test(False, "a/**/", "a")
    # test(False, "a/**/", "a/")

    return status()

if __name__ == "__main__":
    sys.exit(main())
