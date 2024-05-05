from __future__ import annotations

from .globals import (
    assert_match,
    execute_command,
)


def test_ps() -> None:
    stdout, stderr, pid = execute_command("sleep 1000 %\nps\nexit $pid\n", expected_exit_code=None, get_returncode=True)
    print(stderr)

    assert isinstance(pid, int)
    assert_match(str(pid), stdout)
    assert_match("STILL_ACTIVE", stdout)
