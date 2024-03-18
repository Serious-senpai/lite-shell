from __future__ import annotations

import re
import subprocess
from pathlib import Path
from typing import Tuple


build_dir = Path(__file__).parent.parent / "build"


def execute_command(command: str, *, expected_exit_code: int = 0) -> Tuple[str, str]:
    process = subprocess.Popen(
        build_dir / "shell.exe",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    stdout, stderr = process.communicate(f"{command}\nexit 0\n")
    assert process.returncode == expected_exit_code
    return stdout, stderr


def assert_match(token: str, string: str) -> None:
    pattern = r"(?:[^\w]|^)" + re.escape(token) + r"(?:[^\w]|$)"
    assert re.search(pattern, string) is not None


def test_args() -> None:
    stdout, stderr = execute_command("args hello world -abc test --b-c 1")
    assert_match("-a", stdout)
    assert_match("-b", stdout)
    assert_match("-c", stdout)
    assert_match("--b-c", stdout)
    assert stderr.strip() == ""


def test_echo() -> None:
    # Do not include white spaces at both ends
    test_string = "hello world    1 2 --3 4 -abc3 --4 \"in quotes\""
    stdout, stderr = execute_command(f"echo {test_string}")
    assert_match(test_string, stdout)
    assert stderr.strip() == ""
