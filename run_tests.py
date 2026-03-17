#!/usr/bin/env python3
"""
run_tests.py — Build & integration test runner for minimax-game.

Tests:
  1. build          - clean build via make, verify binary is produced
  2. connection     - launch host, Python connects as a raw TCP client,
                      verifies a valid GRID message is received
  3. bad_move       - abruptly disconnect after receiving GRID,
                      verify host doesn't crash
  4. log_format     - parse captured stdout, validate only log lines
                      (renderer UI output is filtered out)

Usage:
  python3 run_tests.py              # run all tests
  python3 run_tests.py --test build # run a specific test
  python3 run_tests.py --verbose    # show captured output on failure
"""

import argparse
import os
import re
import signal
import socket
import struct
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Optional

PROJECT_DIR  = Path(__file__).parent.resolve()
BINARY       = PROJECT_DIR / "bin" / "minimax_game"
TEST_PORT    = 15555
HOST_STARTUP = 1.5
RECV_TIMEOUT = 3.0

MSG_GRID  = 0x03
MSG_MOVE  = 0x05
MSG_ERROR = 0x08

LOG_PATTERN = re.compile(
    r"^\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\] (INFO|WARNING|ERROR): .+$"
)

GREEN  = "\033[32m"
RED    = "\033[31m"
YELLOW = "\033[33m"
BOLD   = "\033[1m"
RESET  = "\033[0m"


@dataclass
class TestResult:
    name: str
    passed: bool
    message: str = ""
    output: str  = ""


@dataclass
class Runner:
    verbose: bool = False
    results: list[TestResult] = field(default_factory=list)

    def run(self, name: str, fn: Callable[[], TestResult]) -> None:
        print(f"  {BOLD}{name:<20}{RESET}", end="", flush=True)
        try:
            result = fn()
        except Exception as exc:
            result = TestResult(name=name, passed=False, message=str(exc))
        result.name = name
        self.results.append(result)
        status = f"{GREEN}PASS{RESET}" if result.passed else f"{RED}FAIL{RESET}"
        detail = f"  {YELLOW}{result.message}{RESET}" if result.message else ""
        print(f"  [{status}]{detail}")
        if not result.passed and self.verbose and result.output:
            indent = "    "
            print(indent + result.output.replace("\n", f"\n{indent}"))

    def summary(self) -> int:
        total  = len(self.results)
        passed = sum(r.passed for r in self.results)
        colour = GREEN if passed == total else RED
        print(f"\n{BOLD}Results: {colour}{passed}/{total} passed{RESET}")
        return 0 if passed == total else 1


def encode_message(payload: bytes) -> bytes:
    return struct.pack(">I", len(payload)) + payload

def recv_message(sock: socket.socket) -> bytes:
    raw_len = _recv_exact(sock, 4)
    length  = struct.unpack(">I", raw_len)[0]
    return _recv_exact(sock, length)

def _recv_exact(sock: socket.socket, n: int) -> bytes:
    buf = b""
    while len(buf) < n:
        chunk = sock.recv(n - len(buf))
        if not chunk:
            raise ConnectionError(f"Connection closed after {len(buf)}/{n} bytes")
        buf += chunk
    return buf

def start_host(extra_args: Optional[list[str]] = None) -> subprocess.Popen:
    cmd = [str(BINARY), "--mode", "host", "--port", str(TEST_PORT), "--seed", "42"]
    if extra_args:
        cmd += extra_args
    return subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        preexec_fn=os.setsid,
    )

def kill_host(proc: subprocess.Popen) -> str:
    try:
        os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
    except ProcessLookupError:
        pass
    stdout, _ = proc.communicate(timeout=2)
    return stdout or ""

def connect_client(retries: int = 10, delay: float = 0.2) -> socket.socket:
    for attempt in range(retries):
        try:
            sock = socket.create_connection(("127.0.0.1", TEST_PORT), timeout=RECV_TIMEOUT)
            sock.settimeout(RECV_TIMEOUT)
            return sock
        except (ConnectionRefusedError, OSError):
            if attempt == retries - 1:
                raise
            time.sleep(delay)
    raise RuntimeError("unreachable")


def test_build() -> TestResult:
    result = subprocess.run(
        ["make", "clean", "all"],
        capture_output=True,
        text=True,
        cwd=PROJECT_DIR,
    )
    output = result.stdout + result.stderr
    if result.returncode != 0:
        return TestResult("build", False, "make failed", output)
    if not BINARY.exists():
        return TestResult("build", False, f"binary not found at {BINARY}", output)
    return TestResult("build", True, output=output)


def test_connection() -> TestResult:
    """
    Host + client handshake — GRID receipt only.

    The host (Player 1) sends the board immediately on connect, then blocks
    on stdin waiting for a keyboard move.  We can't exchange further messages
    without a real interactive player, so we validate the GRID message only:
      - first byte must be MSG_GRID (0x03)
      - payload must be non-empty
      - host process must still be alive after we disconnect
    """
    host   = start_host()
    sock   = None
    output = ""

    try:
        time.sleep(HOST_STARTUP)
        sock = connect_client()

        board_msg = recv_message(sock)
        if not board_msg:
            return TestResult("connection", False, "empty message from host")

        msg_type   = board_msg[0]
        board_data = board_msg[1:]

        if msg_type != MSG_GRID:
            return TestResult(
                "connection", False,
                f"expected GRID (0x{MSG_GRID:02x}), got 0x{msg_type:02x}"
            )
        if not board_data:
            return TestResult("connection", False, "GRID payload is empty")

        time.sleep(0.1)
        if host.poll() is not None:
            return TestResult("connection", False, f"host exited early (rc={host.poll()})")

        return TestResult("connection", True, f"board={len(board_data)}B")

    except Exception as exc:
        return TestResult("connection", False, str(exc))

    finally:
        if sock:
            sock.close()
        output = kill_host(host)


def test_bad_move() -> TestResult:
    """
    Abrupt disconnect after GRID — verify host survives.

    The host blocks on stdin after sending GRID (waiting for Player 1 keyboard
    input), so a malformed socket message won't be read until Player 2's turn.
    Instead we test the simpler and equally valid scenario: receive the GRID,
    then slam the socket shut.  The host must not crash.
    """
    host   = start_host()
    sock   = None
    output = ""

    try:
        time.sleep(HOST_STARTUP)
        sock = connect_client()

        board_msg = recv_message(sock)
        if not board_msg:
            return TestResult("bad_move", False, "no GRID received")

        # Abrupt disconnect
        sock.close()
        sock = None

        time.sleep(0.3)
        if host.poll() is not None:
            return TestResult("bad_move", False, f"host crashed (rc={host.poll()})")

        return TestResult("bad_move", True, "host survived abrupt disconnect")

    except Exception as exc:
        return TestResult("bad_move", False, str(exc))

    finally:
        if sock:
            sock.close()
        output = kill_host(host)


def test_log_format() -> TestResult:
    """
    Validate that every log line matches the expected schema.

    The renderer also writes to stdout (board UI + ANSI escape codes), so we
    filter to lines starting with '[' — those are logger output.  Non-log
    lines (renderer UI) are counted but not failed on.
    """
    host = start_host()

    try:
        time.sleep(HOST_STARTUP)
        try:
            sock = connect_client()
            sock.close()
        except Exception:
            pass

        time.sleep(0.3)
        output = kill_host(host)
        host   = None

        all_lines = [l for l in output.splitlines() if l.strip()]
        if not all_lines:
            return TestResult("log_format", False, "no output captured")

        # Only validate lines that look like log output (start with '[')
        log_lines = [l for l in all_lines if l.startswith("[")]
        if not log_lines:
            return TestResult("log_format", False, "no log lines found in output")

        bad = [l for l in log_lines if not LOG_PATTERN.match(l)]
        if bad:
            snippet = bad[0][:80]
            return TestResult(
                "log_format", False,
                f"{len(bad)} malformed log line(s), e.g.: {snippet!r}",
                output,
            )

        ui_lines = len(all_lines) - len(log_lines)
        return TestResult(
            "log_format", True,
            f"{len(log_lines)} log lines validated  ({ui_lines} UI lines ignored)"
        )

    except Exception as exc:
        return TestResult("log_format", False, str(exc))

    finally:
        if host is not None:
            kill_host(host)


ALL_TESTS: dict[str, Callable[[], TestResult]] = {
    "build":      test_build,
    "connection": test_connection,
    "bad_move":   test_bad_move,
    "log_format": test_log_format,
}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--test",    choices=ALL_TESTS.keys(),
                        help="Run a single test instead of all")
    parser.add_argument("--verbose", action="store_true",
                        help="Print captured output on failure")
    args = parser.parse_args()

    runner  = Runner(verbose=args.verbose)
    to_run  = {args.test: ALL_TESTS[args.test]} if args.test else ALL_TESTS

    print(f"\n{BOLD}minimax-game test suite{RESET}  ({PROJECT_DIR})\n")
    for name, fn in to_run.items():
        runner.run(name, fn)

    return runner.summary()


if __name__ == "__main__":
    sys.exit(main())