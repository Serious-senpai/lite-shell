from __future__ import annotations

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


def test_echo() -> None:
    # Do not include white spaces at both ends
    test_string = "hello world    1 2 --3 4 -abc3 --4"
    stdout, stderr = execute_command(f"echo {test_string}")
    assert test_string in stdout.strip()
    assert stderr.strip() == ""
