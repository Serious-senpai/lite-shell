from __future__ import annotations

import os
import time
from urllib import request

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
    stdout, _ = execute_command("tree src/ --ascii")
    for dirpath, dirnames, filenames in os.walk(current_dir / "src"):
        print(f"At {dirpath}:\n{dirnames}\n{filenames}")
        for dirname in dirnames:
            assert_match(dirname, stdout)
        for filename in filenames:
            assert_match(filename, stdout)


def test_download() -> None:
    execute_command("download https://example.com example.html")
    assert os.path.exists("example.html")

    response = request.urlopen("https://example.com")
    data = response.read()
    with open("example.html", "rb") as file:
        assert data == file.read()

    os.remove("example.html")
