from __future__ import annotations

from .globals import (
    assert_match,
    command_not_found_test,
    execute_command,
)


def test_no_command() -> None:
    command_not_found_test("random-bullshit-go")
    command_not_found_test("random-bullshit-go/")


def test_escape() -> None:
    stdout, _ = execute_command("echoln \"$$Hello World$$\"")
    assert_match("$Hello World$", stdout)
