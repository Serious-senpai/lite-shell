from __future__ import annotations

import random
import re
import subprocess
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, List, Literal, Optional, Tuple, overload

import psutil


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


def bad_alloc_test(command: str) -> None:
    execute_command(command, expected_exit_code=902)


def subprocess_creation_error_test(command: str) -> None:
    execute_command(command, expected_exit_code=903)


def environment_resolve_error_test(command: str) -> None:
    execute_command(command, expected_exit_code=904)


def command_not_found_test(command: str) -> None:
    execute_command(command, expected_exit_code=905)


def argument_missing_test(command: str) -> None:
    execute_command(command, expected_exit_code=906)


def unrecognized_option_test(command: str) -> None:
    execute_command(command, expected_exit_code=907)


def too_many_positional_arguments_test(command: str) -> None:
    execute_command(command, expected_exit_code=908)


def match(token: str, string: str) -> Optional[re.Match[str]]:
    pattern = r"(?:[^\w]|^)" + re.escape(token) + r"(?:[^\w]|$)"
    print(f"Matching token:\n{token!r}\n" + "-" * 30 + f"\n{string!r}")
    return re.search(pattern, string, re.MULTILINE)


def assert_match(token: str, string: str) -> None:
    assert match(token, string) is not None


def assert_not_match(token: str, string: str) -> None:
    assert match(token, string) is None


def test_no_command() -> None:
    execute_command("random-bullshit-go", expected_exit_code=905)
    execute_command("random-bullshit-go/", expected_exit_code=905)


def test_escape() -> None:
    stdout, stderr = execute_command("echoln \"$$Hello World$$\"")
    assert_match("$Hello World$", stdout)
    assert stderr.strip() == ""


def test_cat_1() -> None:
    path = root_dir / "src" / "shell.cpp"
    with open(path, "r", encoding="utf-8") as file:
        data = file.read()

    stdout, stderr = execute_command(f"cat {path}")
    assert_match(data, stdout)
    assert stderr.strip() == ""


def test_cat_2() -> None:
    argument_missing_test("cat")


def test_cat_3() -> None:
    unrecognized_option_test("cat -a")


def test_cat_4() -> None:
    unrecognized_option_test("cat -a 2")


def test_cat_5() -> None:
    too_many_positional_arguments_test("cat foo bar")


def test_date_1() -> None:
    utc_now = datetime.now(timezone.utc)
    utc_display = "/".join(map(str, [utc_now.day, utc_now.month, utc_now.year]))  # dd/mm/yyyy

    local_now = datetime.now()
    local_display = "/".join(map(str, [local_now.day, local_now.month, local_now.year]))  # dd/mm/yyyy

    stdout, stderr = execute_command("date")
    assert_match(utc_display, stdout)
    assert_match(local_display, stdout)

    assert stderr.strip() == ""


def test_date_2() -> None:
    unrecognized_option_test("date -a")


def test_date_3() -> None:
    unrecognized_option_test("date -a 2")


def test_date_4() -> None:
    too_many_positional_arguments_test("date foo")


def test_date_5() -> None:
    too_many_positional_arguments_test("date foo bar")


def test_echo() -> None:
    # Do not include white spaces at both ends
    test_string = r"hello world    1 2 --3 4 -abc3 --4 \"in quotes\""
    stdout, stderr = execute_command(f"echoln \"{test_string}\"")

    test_string = test_string.replace(r"\"", "\"")
    assert_match(test_string, stdout)
    assert stderr.strip() == ""


def test_eval_1() -> None:
    argument_missing_test("eval")


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
    unrecognized_option_test("eval -a")


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
    command = "eval Hello -s f\neval World! -s s\necholn \"$f $s\""
    stdout, stderr = execute_command(command)
    assert_match("Hello World!", stdout)
    assert stderr.strip() == ""


def test_eval_16() -> None:
    command = "eval \"8*(10-6)   /4  -3\" -ms FiRsT\neval \"((6-9) *  7)*  4*3\" -ms sEcOnD\neval \"$FiRsT + $sEcOnD\" -m"
    stdout, stderr = execute_command(command)
    assert_match("-247", stdout)
    assert stderr.strip() == ""


def test_eval_17() -> None:
    command = "eval \"input>\" -ps input\nrandom bullshit go\necholn \"$input\""
    stdout, stderr = execute_command(command)
    assert_match("random bullshit go", stdout)
    assert stderr.strip() == ""


def test_eval_18() -> None:
    command = "eval \"input>\" -mps input\n11669 - 4700\necholn \"$input\""
    stdout, stderr = execute_command(command)
    assert_match("6969", stdout)
    assert stderr.strip() == ""


def test_eval_19() -> None:
    argument_missing_test("eval 2 -s")


def test_exit_1() -> None:
    execute_command("exit", expected_exit_code=0)
    for exit_code in random.choices(range(100), k=5):
        _, stderr = execute_command(f"exit {exit_code}", expected_exit_code=exit_code)

    assert stderr.strip() == ""


def test_exit_2() -> None:
    unrecognized_option_test("exit -a")


def test_exit_3() -> None:
    unrecognized_option_test("exit -a 2")


def test_exit_4() -> None:
    too_many_positional_arguments_test("exit foo bar")


def test_help_1() -> None:
    default_test("help")
    default_test("help help")

    _, stderr = execute_command("help halp", expected_exit_code=901)
    assert_match("Did you mean \"help\"?", stderr)


def test_help_2() -> None:
    unrecognized_option_test("help -a")


def test_help_3() -> None:
    unrecognized_option_test("help -a 2")


def test_help_4() -> None:
    too_many_positional_arguments_test("help foo bar")


def test_ps() -> None:
    process = open_shell(text=True)

    assert process.stdin is not None
    process.stdin.write("sleep 1000 %\nps\n")
    process.stdin.flush()

    time.sleep(0.5)

    # https://stackoverflow.com/a/31214525
    subprocesses = psutil.Process().children(recursive=True)
    print(subprocesses)

    pid = -1
    for p in subprocesses:
        if "sleep" in p.name():
            pid = p.pid
            break

    assert pid != -1

    stdout, stderr = process.communicate("exit\n")
    assert_match(str(pid), stdout)
    assert_match("STILL_ACTIVE", stdout)
    assert process.returncode == 0
    assert stderr.strip() == ""


def test_kill() -> None:
    process = open_shell(text=True)

    assert process.stdin is not None
    process.stdin.write("sleep 1000 %\n")
    process.stdin.flush()

    time.sleep(0.5)

    # https://stackoverflow.com/a/31214525
    subprocesses = psutil.Process().children(recursive=True)
    print(subprocesses)

    pid = -1
    for p in subprocesses:
        if "sleep" in p.name():
            pid = p.pid
            break

    assert pid != -1

    stdout, stderr = process.communicate(f"kill {pid} 40000\nps\nexit\n")
    assert_match(str(pid), stdout)
    assert_match("40000", stdout)
    assert_not_match("STILL_ACTIVE", stdout)
    assert process.returncode == 0
    assert stderr.strip() == ""


def test_sleep() -> None:
    start = time.perf_counter()
    default_test("sleep 1000")
    end = time.perf_counter()

    assert 1.0 < end - start < 1.2


def is_prime(value: int, /) -> bool:
    if value < 2:
        return False

    div = 2
    while div * div <= value:
        if value % div == 0:
            return False

        div += 1

    return True


def test_script_prime() -> None:
    for value in range(20):
        stdout, stderr = execute_command(f"tests/prime\n{value}")
        if is_prime(value):
            assert_match(f"{value} is a prime", stdout)
        else:
            assert_match(f"{value} is not a prime", stdout)

        assert_not_match("@OFF", stdout)
        assert_not_match("@ON", stdout)
        assert stderr.strip() == ""


def test_script_reverse() -> None:
    for _ in range(20):
        arr = random.choices(range(-50, 100), k=40)
        stdout, stderr = execute_command(f"tests/reverse\n{' '.join(map(str, arr))}")
        assert_match(" ".join(map(str, arr[::-1])), stdout)
        assert_not_match("@OFF", stdout)
        assert_not_match("@ON", stdout)
        assert stderr.strip() == ""


def test_script_sum() -> None:
    for _ in range(20):
        arr = random.choices(range(-50, 100), k=40)
        stdout, stderr = execute_command(f"tests/sum\n{' '.join(map(str, arr))}")
        assert_match(str(sum(arr)), stdout)
        assert_not_match("@OFF", stdout)
        assert_not_match("@ON", stdout)
        assert stderr.strip() == ""


def test_script_1() -> None:
    stdout, stderr = execute_command("tests/shell-script-1")
    assert_match("Starting test", stdout)
    assert_match("6969", stdout)
    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)
    assert stderr.strip() == ""


def test_script_2() -> None:
    stdout, stderr = execute_command("tests/shell-script-2")
    assert_match("Starting test", stdout)

    for i in range(5, 10):
        assert_match(f"2 * {i} = {2 * i}", stdout)

    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)
    assert stderr.strip() == ""


def test_script_3() -> None:
    stdout, stderr = execute_command("tests/shell-script-3")
    assert_match("Starting test", stdout)

    for i in range(1000, 2000):
        if i % 2 == 0:
            assert_match(f"{i} is even", stdout)
        else:
            assert_match(f"{i} is odd", stdout)

    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)
    assert stderr.strip() == ""


def test_script_4() -> None:
    runtime_error_test("tests/shell-script-4")


def test_script_5() -> None:
    stdout, stderr = execute_command("tests/shell-script-5")

    lines: List[str] = []
    for i in range(50):
        for j in range(i, 50):
            lines.append(f"{i} {j}")

    assert_match("\n".join(lines), stdout)

    lines.clear()
    for i in range(50):
        for j in range(i):
            assert_not_match(f"{i} {j}", stdout)

    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)
    assert stderr.strip() == ""


def test_script_6() -> None:
    stdout, stderr = execute_command("tests/shell-script-6")

    assert_match("1234\n5\n6\n", stdout)
    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)
    assert stderr.strip() == ""


def test_script_7() -> None:
    stdout, stderr = execute_command("tests/shell-script-7")

    assert_match("lmao this dumb string", stdout)
    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)
    assert stderr.strip() == ""
