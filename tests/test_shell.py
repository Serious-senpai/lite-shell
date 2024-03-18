from __future__ import annotations

import random
import re
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Tuple


root_dir = Path(__file__).parent.parent
build_dir = root_dir / "build"


def execute_command(command: str, *, expected_exit_code: int = 0) -> Tuple[str, str]:
    process = subprocess.Popen(
        build_dir / "shell.exe",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    stdout, stderr = process.communicate(f"{command}\nexit\n".encode("utf-8"))
    assert process.returncode == expected_exit_code

    def decode(data: bytes) -> str:
        return data.decode("utf-8").replace("\r", "")

    return decode(stdout), decode(stderr)


def default_test(command: str) -> None:
    _, stderr = execute_command(command)
    assert stderr.strip() == ""


def assert_match(token: str, string: str) -> None:
    pattern = r"(?:[^\w]|^)" + re.escape(token) + r"(?:[^\w]|$)"
    print(pattern)
    print("-" * 20)
    print(repr(string))
    assert re.search(pattern, string) is not None


def test_args() -> None:
    stdout, stderr = execute_command("args hello world -abc test --b-c 1")
    assert_match("-a", stdout)
    assert_match("-b", stdout)
    assert_match("-c", stdout)
    assert_match("--b-c", stdout)
    assert stderr.strip() == ""


def test_cat() -> None:
    path = root_dir / "src" / "shell.cpp"
    with open(path, "r", encoding="utf-8") as file:
        data = file.read()

    stdout, stderr = execute_command(f"cat {path}")
    assert_match(data, stdout)
    assert stderr.strip() == ""


def test_date() -> None:
    utc_now = datetime.now(timezone.utc)
    utc_display = "/".join(map(str, [utc_now.day, utc_now.month, utc_now.year]))  # dd/mm/yyyy

    local_now = datetime.now()
    local_display = "/".join(map(str, [local_now.day, local_now.month, local_now.year]))  # dd/mm/yyyy

    stdout, stderr = execute_command("date")
    assert_match(utc_display, stdout)
    assert_match(local_display, stdout)

    assert stderr.strip() == ""


def test_echo() -> None:
    # Do not include white spaces at both ends
    test_string = "hello world    1 2 --3 4 -abc3 --4 \"in quotes\""
    stdout, stderr = execute_command(f"echo {test_string}")
    assert_match(test_string, stdout)
    assert stderr.strip() == ""


def test_exit() -> None:
    for exit_code in random.choices(range(100), k=5):
        _, stderr = execute_command(f"exit {exit_code}", expected_exit_code=exit_code)

    assert stderr.strip() == ""


def test_help() -> None:
    default_test("help")
    default_test("help help")

    _, stderr = execute_command("help halp", expected_exit_code=901)
    assert_match("Did you mean \"help\"?", stderr)
