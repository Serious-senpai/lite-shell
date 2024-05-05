from __future__ import annotations

import os

from .globals import (
    assert_match,
    current_dir,
    execute_command,
)


def test_cd_1() -> None:
    stdout, _ = execute_command("echo We're currently at $cd")
    assert_match(f"We're currently at {current_dir}", stdout)


def test_cd_2() -> None:
    stdout, _ = execute_command("cd ..\necho We're currently at $cd")

    parent = os.path.normpath(current_dir / "..")
    assert_match(f"We're currently at {parent}", stdout)
