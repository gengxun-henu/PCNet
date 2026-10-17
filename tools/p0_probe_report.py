#!/usr/bin/env python3
"""Validate and summarize a PCNet P0 probe report."""
import json
import sys
from pathlib import Path

REQUIRED = {"environment_path", "library_path", "controlnet", "timing", "failures"}


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: p0_probe_report.py <report.json>", file=sys.stderr)
        return 2
    data = json.loads(Path(sys.argv[1]).read_text())
    missing = sorted(REQUIRED.difference(data))
    if missing:
        print(f"missing fields: {', '.join(missing)}", file=sys.stderr)
        return 1
    print(f"environment: {data['environment_path']}")
    print(f"library: {data['library_path']}")
    print(f"controlnet: {data['controlnet']}")
    print(f"timing: {data['timing']}")
    print(f"failures: {len(data['failures'])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
