#!/usr/bin/env python3
import json
import sys

def load(path):
    with open(path) as handle:
        return json.load(handle)

def main():
    if len(sys.argv) != 3:
        print("usage: compare_baseline.py pcnet.json baseline.json", file=sys.stderr)
        return 2
    pcnet, baseline = map(load, sys.argv[1:])
    print(json.dumps({"pcnet_seconds": pcnet.get("seconds"), "baseline_seconds": baseline.get("seconds"), "pcnet_points": pcnet.get("control_points"), "baseline_points": baseline.get("control_points")}, indent=2))
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
