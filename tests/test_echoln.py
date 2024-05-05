from __future__ import annotations

from .globals import (
    assert_match,
    execute_command,
)


def test_echoln() -> None:
    test_string = r"hello world    1 2 --3 4 -abc3 --4 \"in quotes\"  "
    stdout, _ = execute_command(f"echoln \"{test_string}\"")

    test_string = test_string.replace(r"\"", "\"")
    assert_match(test_string, stdout)
