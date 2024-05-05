from __future__ import annotations

import os
import time

from .globals import (
    assert_match,
    current_dir,
    execute_command,
)


def test_sleep() -> None:
    start = time.perf_counter()
    execute_command("sleep 1000")
    end = time.perf_counter()

    assert 1.0 < end - start < 1.1


def test_tree() -> None:
    stdout, _ = execute_command("tree . --ascii")
    for dirpath, dirnames, filenames in os.walk(current_dir):
        print(f"At {dirpath}:\n{dirnames}\n{filenames}")
        for dirname in dirnames:
            assert_match(dirname, stdout)
        for filename in filenames:
            assert_match(filename, stdout)
