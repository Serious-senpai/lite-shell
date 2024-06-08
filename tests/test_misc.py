from __future__ import annotations

from .globals import (
    assert_match,
    command_not_found_test,
    execute_command,
)


def test_no_command() -> None:
    command_not_found_test("abcxyz")
    command_not_found_test("abcxyz/")


def test_escape() -> None:
    stdout, _ = execute_command("echoln \"$$Hello World$$\"")
    assert_match("$Hello World$", stdout)


def test_case_insensitive() -> None:
    stdout, _ = execute_command("EChO \"Hello World\"")
    assert_match("Hello World", stdout)

    stdout, _ = execute_command("eChO \"Hello World\"")
    assert_match("Hello World", stdout)
