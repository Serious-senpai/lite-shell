from __future__ import annotations

import os
import re
import subprocess
from pathlib import Path
from typing import Literal, Optional, Tuple, Union, overload


__all__ = (
    "root_dir",
    "build_dir",
    "current_dir",
    "execute_command",
    "runtime_error_test",
    "invalid_argument_test",
    "bad_alloc_test",
    "subprocess_creation_error_test",
    "environment_resolve_error_test",
    "command_not_found_test",
    "argument_missing_test",
    "unrecognized_option_test",
    "too_many_positional_arguments_test",
    "assert_match",
    "assert_not_match",
)


root_dir = Path(__file__).parent.parent
build_dir = root_dir / "build"
current_dir = Path(os.getcwd())


@overload
def execute_command(
    command: str,
    *,
    expected_exit_code: Optional[int] = 0,
    get_returncode: Literal[False] = False,
    no_stderr: bool = True,
) -> Tuple[str, str]: ...


@overload
def execute_command(
    command: str,
    *,
    expected_exit_code: Optional[int] = 0,
    get_returncode: Literal[True],
    no_stderr: bool = True,
) -> Tuple[str, str, int]: ...


def execute_command(
    command: str,
    *,
    expected_exit_code: Optional[int] = 0,
    get_returncode: bool = False,
    no_stderr: bool = True,
) -> Union[Tuple[str, str], Tuple[str, str, int]]:
    process = subprocess.Popen(
        build_dir / "shell.exe",
        bufsize=0,
        cwd=root_dir,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=False,
    )
    stdout, stderr = process.communicate(f"{command}\nexit\n".encode("utf-8"))
    assert process.returncode == expected_exit_code or expected_exit_code is None

    if no_stderr:
        assert stderr == b""

    def decode(data: bytes) -> str:
        return data.decode("utf-8").replace("\r", "")

    result = decode(stdout), decode(stderr)
    return (*result, process.returncode) if get_returncode else result


def runtime_error_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=900, no_stderr=False)


def invalid_argument_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=901, no_stderr=False)


def bad_alloc_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=902, no_stderr=False)


def subprocess_creation_error_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=903, no_stderr=False)


def environment_resolve_error_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=904, no_stderr=False)


def command_not_found_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=905, no_stderr=False)


def argument_missing_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=906, no_stderr=False)


def unrecognized_option_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=907, no_stderr=False)


def too_many_positional_arguments_test(command: str) -> Tuple[str, str]:
    return execute_command(command, expected_exit_code=908, no_stderr=False)


def __match(token: str, string: str) -> Tuple[Optional[re.Match[str]], str]:
    pattern = r"(?:[^\w]|^)" + re.escape(token) + r"(?:[^\w]|$)"
    return (re.search(pattern, string, re.MULTILINE), f"Matching token:\n{token!r}\n" + "-" * 48 + "\n" + string)


def assert_match(token: str, string: str) -> None:
    matched, message = __match(token, string)
    assert matched is not None, message


def assert_not_match(token: str, string: str) -> None:
    matched, message = __match(token, string)
    assert matched is None, message
