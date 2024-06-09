from __future__ import annotations

import shutil
from .globals import (
    assert_match,
    command_not_found_test,
    execute_command,
    root_dir,
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


def test_mutable_path() -> None:
    shutil.move(root_dir / "build" / "hello.exe", root_dir / "hello123.exe")
    try:
        stdout, _ = execute_command(f"eval -s PATH \"$PATH;{root_dir}\"\nhello123")
        assert_match("Hello world!", stdout)
    finally:
        shutil.move(root_dir / "hello123.exe", root_dir / "build" / "hello.exe")
