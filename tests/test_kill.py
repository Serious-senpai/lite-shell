from __future__ import annotations

from .globals import (
    assert_match,
    assert_not_match,
    execute_command,
)


def test_kill() -> None:
    stdout, _, pid = execute_command("sleep 1000 %\nkill $pid 40000\nps\nexit $pid\n", expected_exit_code=None, get_returncode=True)

    assert isinstance(pid, int)
    assert_match(str(pid), stdout)
    assert_match("40000", stdout)
    assert_not_match("STILL_ACTIVE", stdout)
