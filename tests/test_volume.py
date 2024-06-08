from __future__ import annotations

from .globals import execute_command


def test_ps() -> None:
    execute_command("volume C:\\")
