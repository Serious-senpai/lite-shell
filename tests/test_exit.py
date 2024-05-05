from __future__ import annotations

import random

from .globals import (
    execute_command,
    too_many_positional_arguments_test,
    unrecognized_option_test,
)


def test_exit_1() -> None:
    execute_command("exit", expected_exit_code=0)
    for exit_code in random.choices(range(100), k=5):
        execute_command(f"exit {exit_code}", expected_exit_code=exit_code)


def test_exit_2() -> None:
    unrecognized_option_test("exit -a")


def test_exit_3() -> None:
    unrecognized_option_test("exit -a 2")


def test_exit_4() -> None:
    too_many_positional_arguments_test("exit foo bar")
