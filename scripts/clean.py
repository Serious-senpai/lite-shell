from __future__ import annotations

import os
import re
import subprocess
from pathlib import Path
from typing import Iterable

from colorama import Fore, Style, init


def explore(directory: str, *, recursive: bool) -> Iterable[str]:
    if recursive:
        for root, _, files in os.walk(directory):
            for file in files:
                yield os.path.join(root, file)

    else:
        for file in os.listdir(directory):
            yield os.path.join(directory, file)


init(convert=True)
root = Path(__file__).parent.parent
pattern = re.compile(r"^#include [\"<][\w\.]+[\">]$", flags=re.MULTILINE)
removable = 0
for file in explore(str(root), recursive=True):
    if file.endswith(".cpp") or file.endswith(".hpp"):
        with open(file, "r", encoding="utf-8") as f:
            data = f.read()

        includes = tuple(pattern.finditer(data))
        if len(includes) == 1:
            # Skip files with only one include
            continue

        for include in includes:
            line = include.group()
            print(f"Removing {Fore.BLUE}{line}{Style.RESET_ALL} from {file}")

            d = data.replace("\n" + line, "")

            with open(file, "w", encoding="utf-8") as f:
                f.write(d)

            # Recompile
            process = subprocess.Popen([root / "scripts" / "build.bat"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            code = process.wait()

            if code == 0:
                print("-> Removable")
                data = d
                removable += 1

            else:
                print("-> Not removable")
                with open(file, "w", encoding="utf-8") as f:
                    f.write(data)


print(f"Found {removable} removable " + ("include" if removable == 1 else "includes"))
exit(removable)
