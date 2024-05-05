from __future__ import annotations

from datetime import datetime, timezone

from .globals import (
    assert_match,
    execute_command,
    too_many_positional_arguments_test,
    unrecognized_option_test,
)


def test_date_1() -> None:
    utc_now = datetime.now(timezone.utc)
    utc_display = "/".join(map(str, [utc_now.day, utc_now.month, utc_now.year]))  # dd/mm/yyyy

    local_now = datetime.now()
    local_display = "/".join(map(str, [local_now.day, local_now.month, local_now.year]))  # dd/mm/yyyy

    stdout, _ = execute_command("date")
    assert_match(utc_display, stdout)
    assert_match(local_display, stdout)


def test_date_2() -> None:
    unrecognized_option_test("date -a")


def test_date_3() -> None:
    unrecognized_option_test("date -a 2")


def test_date_4() -> None:
    too_many_positional_arguments_test("date foo")


def test_date_5() -> None:
    too_many_positional_arguments_test("date foo bar")
