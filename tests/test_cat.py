from __future__ import annotations

from .globals import (
    argument_missing_test,
    assert_match,
    execute_command,
    root_dir,
    too_many_positional_arguments_test,
    unrecognized_option_test,
)


def test_cat_1() -> None:
    path = root_dir / "src" / "shell.cpp"
    with open(path, "r", encoding="utf-8") as file:
        data = file.read()

    stdout, _ = execute_command(f"cat {path}")
    assert_match(data, stdout)


def test_cat_2() -> None:
    argument_missing_test("cat")


def test_cat_3() -> None:
    unrecognized_option_test("cat -a")


def test_cat_4() -> None:
    unrecognized_option_test("cat -a 2")


def test_cat_5() -> None:
    too_many_positional_arguments_test("cat foo bar")
