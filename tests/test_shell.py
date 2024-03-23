from __future__ import annotations

import random
import re
import subprocess
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Literal, Tuple, overload


root_dir = Path(__file__).parent.parent
build_dir = root_dir / "build"


@overload
def open_shell(*, text: Literal[True]) -> subprocess.Popen[str]: ...
@overload
def open_shell(*, text: Literal[False]) -> subprocess.Popen[bytes]: ...


def open_shell(*, text: bool) -> subprocess.Popen[Any]:
    return subprocess.Popen(
        build_dir / "shell.exe",
        bufsize=0,
        cwd=root_dir,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=text,
    )


def execute_command(command: str, *, expected_exit_code: int = 0) -> Tuple[str, str]:
    process = open_shell(text=False)
    stdout, stderr = process.communicate(f"{command}\nexit\n".encode("utf-8"))
    assert process.returncode == expected_exit_code

    def decode(data: bytes) -> str:
        return data.decode("utf-8").replace("\r", "")

    return decode(stdout), decode(stderr)


def default_test(command: str) -> None:
    _, stderr = execute_command(command)
    assert stderr.strip() == ""


def runtime_error_test(command: str) -> None:
    execute_command(command, expected_exit_code=900)


def invalid_argument_test(command: str) -> None:
    execute_command(command, expected_exit_code=901)


def assert_match(token: str, string: str) -> None:
    pattern = r"(?:[^\w]|^)" + re.escape(token) + r"(?:[^\w]|$)"
    print(f"Matching token:\n{token!r}\n" + "-" * 30 + f"\n{string!r}")
    assert re.search(pattern, string) is not None


def test_escape() -> None:
    stdout, stderr = execute_command("echo $$Hello World$$")
    assert_match("$Hello World$", stdout)
    assert stderr.strip() == ""


def test_args() -> None:
    stdout, stderr = execute_command("args hello world -abc test --b-c 1")
    assert_match("-a", stdout)
    assert_match("-b", stdout)
    assert_match("-c", stdout)
    assert_match("--b-c", stdout)
    assert stderr.strip() == ""


def test_cat() -> None:
    path = root_dir / "src" / "shell.cpp"
    with open(path, "r", encoding="utf-8") as file:
        data = file.read()

    stdout, stderr = execute_command(f"cat {path}")
    assert_match(data, stdout)
    assert stderr.strip() == ""


def test_cat_2() -> None:
    invalid_argument_test("cat")
    invalid_argument_test("cat -a")
    invalid_argument_test("cat -a 2")
    invalid_argument_test("cat foo bar")


def test_date() -> None:
    utc_now = datetime.now(timezone.utc)
    utc_display = "/".join(map(str, [utc_now.day, utc_now.month, utc_now.year]))  # dd/mm/yyyy

    local_now = datetime.now()
    local_display = "/".join(map(str, [local_now.day, local_now.month, local_now.year]))  # dd/mm/yyyy

    stdout, stderr = execute_command("date")
    assert_match(utc_display, stdout)
    assert_match(local_display, stdout)

    assert stderr.strip() == ""


def test_date_2() -> None:
    invalid_argument_test("date -a")
    invalid_argument_test("date -a 2")
    invalid_argument_test("date foo")
    invalid_argument_test("date foo bar")


def test_echo() -> None:
    # Do not include white spaces at both ends
    test_string = "hello world    1 2 --3 4 -abc3 --4 \"in quotes\""
    stdout, stderr = execute_command(f"echo {test_string}")
    assert_match(test_string, stdout)
    assert stderr.strip() == ""


def test_eval() -> None:
    invalid_argument_test("eval")


def test_eval_2() -> None:
    default_test("eval ++2 -m")


def test_eval_3() -> None:
    default_test("eval \"--2\" -m")


def test_eval_4() -> None:
    default_test("eval +-+-2 -m")


def test_eval_5() -> None:
    expression = "1949 + 18 * (38 - 91) / 6"
    stdout, stderr = execute_command(f"eval \"{expression}\" -m")
    assert_match(str(int(eval(expression))), stdout)
    assert stderr.strip() == ""


def test_eval_7() -> None:
    invalid_argument_test("eval -a")


def test_eval_8() -> None:
    runtime_error_test("eval a -m")


def test_eval_9() -> None:
    runtime_error_test("eval \"4 / 0\" -m")


def test_eval_10() -> None:
    runtime_error_test("eval \"((2 + 3) * 4\" -m")


def test_eval_11() -> None:
    runtime_error_test("eval \"5 / (1 - 2) )\" -m")


def test_eval_12() -> None:
    runtime_error_test("eval \"2 +\" -m")


def test_eval_13() -> None:
    runtime_error_test("eval \"* 2\" -m")


def test_eval_14() -> None:
    runtime_error_test("eval \"5 */ 3\" -m")


def test_eval_15() -> None:
    command = "eval Hello -s f\neval World! -s s\necho $f $s"
    stdout, stderr = execute_command(command)
    assert_match("Hello World!", stdout)
    assert stderr.strip() == ""


def test_eval_16() -> None:
    command = "eval \"8*(10-6)   /4  -3\" -ms FiRsT\neval \"((6-9) *  7)*  4*3\" -ms sEcOnD\neval \"$FiRsT + $sEcOnD\" -m"
    stdout, stderr = execute_command(command)
    assert_match("-247", stdout)
    assert stderr.strip() == ""


def test_eval_17() -> None:
    command = "eval \"input>\" -ps input\nrandom bullshit go\necho $input"
    stdout, stderr = execute_command(command)
    assert_match("random bullshit go", stdout)
    assert stderr.strip() == ""


def test_eval_18() -> None:
    command = "eval \"input>\" -mps input\n11669 - 4700\necho $input"
    stdout, stderr = execute_command(command)
    assert_match("6969", stdout)
    assert stderr.strip() == ""


def test_exit() -> None:
    execute_command("exit", expected_exit_code=0)
    for exit_code in random.choices(range(100), k=5):
        _, stderr = execute_command(f"exit {exit_code}", expected_exit_code=exit_code)

    assert stderr.strip() == ""


def test_exit_2() -> None:
    invalid_argument_test("exit -a")
    invalid_argument_test("exit -a 2")
    invalid_argument_test("exit foo bar")


def test_help() -> None:
    default_test("help")
    default_test("help help")

    _, stderr = execute_command("help halp", expected_exit_code=901)
    assert_match("Did you mean \"help\"?", stderr)


def test_help_2() -> None:
    invalid_argument_test("help -a")
    invalid_argument_test("help -a 2")
    invalid_argument_test("help foo bar")


def test_sleep() -> None:
    start = time.perf_counter()
    default_test("sleep 1000")
    end = time.perf_counter()

    assert end - start > 1.0


def test_script() -> None:
    stdout, stderr = execute_command("tests/shell-script-1")
    assert_match("Starting test", stdout)
    assert_match("6969", stdout)
    assert stderr.strip() == ""
