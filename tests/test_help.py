from __future__ import annotations

from .globals import (
    assert_match,
    execute_command,
    invalid_argument_test,
    too_many_positional_arguments_test,
    unrecognized_option_test,
)


def test_help_1() -> None:
    execute_command("help")
    execute_command("help help")

    _, stderr = invalid_argument_test("help halp")
    assert_match("Did you mean \"help\"?", stderr)


def test_help_2() -> None:
    unrecognized_option_test("help -a")


def test_help_3() -> None:
    unrecognized_option_test("help -a 2")


def test_help_4() -> None:
    too_many_positional_arguments_test("help foo bar")
