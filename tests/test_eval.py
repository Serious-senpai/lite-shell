from __future__ import annotations

from .globals import (
    argument_missing_test,
    assert_match,
    execute_command,
    runtime_error_test,
    unrecognized_option_test,
)


def test_eval_1() -> None:
    argument_missing_test("eval")


def test_eval_2() -> None:
    execute_command("eval ++2 -m")


def test_eval_3() -> None:
    execute_command("eval \"--2\" -m")


def test_eval_4() -> None:
    execute_command("eval +-+-2 -m")


def test_eval_5() -> None:
    expression = "1949 + 18 * (38 - 91) / 6"
    stdout, _ = execute_command(f"eval \"{expression}\" -m")
    assert_match(str(int(eval(expression))), stdout)


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
    stdout, _ = execute_command(command)
    assert_match("Hello World!", stdout)


def test_eval_16() -> None:
    command = "eval \"8*(10-6)   /4  -3\" -ms FiRsT\neval \"((6-9) *  7)*  4*3\" -ms sEcOnD\neval \"$FiRsT + $sEcOnD\" -m"
    stdout, _ = execute_command(command)
    assert_match("-247", stdout)


def test_eval_17() -> None:
    command = "eval \"input>\" -ps input\nrandom bullshit go\necholn \"$input\""
    stdout, _ = execute_command(command)
    assert_match("random bullshit go", stdout)


def test_eval_18() -> None:
    command = "eval \"input>\" -mps input\n11669 - 4700\necholn \"$input\""
    stdout, _ = execute_command(command)
    assert_match("6969", stdout)


def test_eval_19() -> None:
    argument_missing_test("eval 2 -s")
