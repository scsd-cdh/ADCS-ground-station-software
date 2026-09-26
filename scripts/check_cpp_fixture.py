#!/usr/bin/env python3
"""Fail if the MATLAB->C++ reference fixture is older than the MATLAB sources.

cpp/tests/data/demo_reference.txt carries an `inputs_sha256` stamp written by
matlab/tests/export_cpp_fixture.m. This recomputes the same hash over the files
listed in matlab/codegen/cpp_inputs.txt (see matlab/codegen/inputsHash.m) and
compares. No MATLAB needed, so it runs in the pre-commit hook and in CI.

    python scripts/check_cpp_fixture.py           check (exit 1 if stale)
    python scripts/check_cpp_fixture.py --print   print the current hash
"""
import glob
import hashlib
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INPUTS = os.path.join(ROOT, "matlab", "codegen", "cpp_inputs.txt")
FIXTURE = os.path.join(ROOT, "cpp", "tests", "data", "demo_reference.txt")


def inputs_hash():
    rel = set()
    with open(INPUTS, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            matches = glob.glob(os.path.join(ROOT, line))
            if not matches:
                sys.exit(f"cpp_inputs.txt entry matches no file: {line}")
            folder = os.path.dirname(line)
            rel.update(f"{folder}/{os.path.basename(m)}" for m in matches)
    h = hashlib.sha256()
    for r in sorted(rel):
        with open(os.path.join(ROOT, r), "rb") as f:
            data = f.read().replace(b"\r", b"")
        h.update(r.encode() + b"\0" + data + b"\0")
    return h.hexdigest()


def main():
    current = inputs_hash()
    if "--print" in sys.argv:
        print(current)
        return 0
    try:
        with open(FIXTURE, encoding="utf-8") as f:
            m = re.search(r"^inputs_sha256 ([0-9a-f]{64})$", f.read(), re.M)
    except FileNotFoundError:
        m = None
    if m and m.group(1) == current:
        return 0
    print(
        "C++ reference fixture is stale: files listed in matlab/codegen/cpp_inputs.txt\n"
        "(matlab/src/core, the demo scenario, ...) changed since it was generated.\n"
        "In MATLAB run:\n"
        "    run matlab/setup.m\n"
        "    build_cpp\n"
        "    export_cpp_fixture\n"
        "then run the C++ tests (cd cpp && ctest --test-dir build) and commit\n"
        "cpp/tests/data/demo_reference.txt with your change.",
        file=sys.stderr,
    )
    return 1


if __name__ == "__main__":
    sys.exit(main())
