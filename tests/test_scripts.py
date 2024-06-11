from __future__ import annotations

import random

from .globals import (
    assert_match,
    assert_not_match,
    execute_command,
    runtime_error_test,
)


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
    for value in range(-20, 100):
        stdout, _ = execute_command(f"tests/prime\n{value}")
        if is_prime(value):
            assert_match(f"{value} is a prime", stdout)
        else:
            assert_match(f"{value} is not a prime", stdout)

        assert_not_match("@OFF", stdout)
        assert_not_match("@ON", stdout)


def test_script_reverse() -> None:
    for _ in range(20):
        arr = random.choices(range(-50, 100), k=40)
        stdout, _ = execute_command(f"tests/reverse\n{' '.join(map(str, arr))}")
        assert_match(" ".join(map(str, arr[::-1])), stdout)
        assert_not_match("@OFF", stdout)
        assert_not_match("@ON", stdout)


def test_script_sort() -> None:
    for _ in range(20):
        arr = random.choices(range(-50, 100), k=20)
        stdout, _ = execute_command(f"tests/sort\n{' '.join(map(str, arr))}")
        assert_match(" ".join(map(str, sorted(arr))), stdout)
        assert_not_match("@OFF", stdout)
        assert_not_match("@ON", stdout)


def test_script_sum() -> None:
    for _ in range(20):
        arr = random.choices(range(-50, 100), k=40)
        stdout, _ = execute_command(f"tests/sum\n{' '.join(map(str, arr))}")
        assert_match(str(sum(arr)), stdout)
        assert_not_match("@OFF", stdout)
        assert_not_match("@ON", stdout)


def test_script_1() -> None:
    stdout, _ = execute_command("tests/shell-script-1")
    assert_match("Starting test", stdout)
    assert_match("6969", stdout)
    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)


def test_script_2() -> None:
    stdout, _ = execute_command("tests/shell-script-2")
    assert_match("Starting test", stdout)

    for i in range(5, 10):
        assert_match(f"2 * {i} = {2 * i}", stdout)

    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)


def test_script_3() -> None:
    stdout, _ = execute_command("tests/shell-script-3")
    assert_match("Starting test", stdout)

    for i in range(1000, 2000):
        if i % 2 == 0:
            assert_match(f"{i} is even", stdout)
        else:
            assert_match(f"{i} is odd", stdout)

    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)


def test_script_4() -> None:
    runtime_error_test("tests/shell-script-4")


def test_script_5() -> None:
    stdout, _ = execute_command("tests/shell-script-5")

    lines = [f"{i} {j}" for i in range(50) for j in range(i, 50)]
    assert_match("\n".join(lines), stdout)

    lines.clear()
    for i in range(50):
        for j in range(i):
            assert_not_match(f"{i} {j}", stdout)

    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)


def test_script_6() -> None:
    stdout, _ = execute_command("tests/shell-script-6")

    assert_match("1234\n5\n6\n", stdout)
    assert_not_match("@OFF", stdout)
    assert_not_match("@ON", stdout)
