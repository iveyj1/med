#!/usr/bin/env python3

import argparse
import re
import subprocess
import sys
from pathlib import Path


FRAME_RE = re.compile(
    r"^(?P<prefix>.*?)"
    r"(?P<binary>\S+)"
    r"\((?P<symbol>[^()+]*)\+(?P<offset>0x[0-9a-fA-F]+)\)"
    r"\s*\[(?P<runtime_addr>0x[0-9a-fA-F]+)\]"
    r"(?P<suffix>.*)$"
)

NM_RE = re.compile(r"^([0-9a-fA-F]+)\s+[A-Za-z?]\s+(.+)$")


class SymbolCache:
    def __init__(self):
        self._cache = {}

    def get(self, binary: str) -> dict[str, int]:
        if binary not in self._cache:
            self._cache[binary] = self._load(binary)
        return self._cache[binary]

    @staticmethod
    def _load(binary: str) -> dict[str, int]:
        symbols: dict[str, int] = {}
        for cmd in (["nm", "-an", binary], ["nm", "-D", "-an", binary]):
            try:
                proc = subprocess.run(
                    cmd,
                    check=False,
                    text=True,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.DEVNULL,
                )
            except FileNotFoundError:
                return symbols

            if proc.returncode != 0:
                continue

            for line in proc.stdout.splitlines():
                m = NM_RE.match(line)
                if not m:
                    continue

                addr_hex, raw_name = m.groups()
                try:
                    addr = int(addr_hex, 16)
                except ValueError:
                    continue

                name = raw_name.strip()
                symbols.setdefault(name, addr)

                if "@@" in name:
                    base_name = name.split("@@", 1)[0]
                    symbols.setdefault(base_name, addr)

        return symbols


def run_addr2line(binary: str, address: int) -> str | None:
    cmd = ["addr2line", "-e", binary, "-f", "-C", "-p", hex(address)]
    try:
        proc = subprocess.run(
            cmd,
            check=False,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
    except FileNotFoundError:
        return None

    if proc.returncode != 0:
        return None

    out = proc.stdout.strip()
    if not out:
        return None
    return out


def resolve_frame(line: str, symcache: SymbolCache) -> str:
    match = FRAME_RE.match(line.rstrip("\n"))
    if not match:
        return line.rstrip("\n")

    binary = match.group("binary")
    symbol = match.group("symbol")
    offset_hex = match.group("offset")

    try:
        offset = int(offset_hex, 16)
    except ValueError:
        return line.rstrip("\n") + "  [unresolved: bad offset]"

    addr = None
    if symbol:
        symbols = symcache.get(binary)
        base = symbols.get(symbol)
        if base is not None:
            addr = base + offset
    else:
        addr = offset

    if addr is None:
        return line.rstrip("\n") + f"  [unresolved: no symbol '{symbol}']"

    resolved = run_addr2line(binary, addr)
    if resolved is None:
        return line.rstrip("\n") + "  [unresolved: addr2line failed]"

    return line.rstrip("\n") + f"  => {resolved}"


def input_lines(path: str | None):
    if path is None or path == "-":
        for line in sys.stdin:
            yield line
        return

    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            yield line


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Resolve backtrace lines like './med(func+0x12)[0x...]' to file:line"
    )
    parser.add_argument(
        "input",
        nargs="?",
        help="Input file (default: stdin). Use '-' for stdin.",
    )
    args = parser.parse_args()

    if not shutil_which("addr2line"):
        print("symtrace: addr2line not found in PATH", file=sys.stderr)
        return 2
    if not shutil_which("nm"):
        print("symtrace: nm not found in PATH", file=sys.stderr)
        return 2

    symcache = SymbolCache()
    for line in input_lines(args.input):
        print(resolve_frame(line, symcache))

    return 0


def shutil_which(cmd: str) -> str | None:
    from shutil import which

    return which(cmd)


if __name__ == "__main__":
    raise SystemExit(main())
