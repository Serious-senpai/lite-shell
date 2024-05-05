from __future__ import annotations

from .globals import (
    assert_match,
    execute_command,
)


def test_suspend() -> None:
    stdout, _, pid = execute_command("sleep 1000 %\nsuspend $pid\nps\nresume $pid\nexit $pid\n", expected_exit_code=None, get_returncode=True)

    assert isinstance(pid, int)
    assert_match(str(pid), stdout)
    assert_match("STILL_ACTIVE", stdout)
    assert_match("Yes", stdout)
