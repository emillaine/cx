#!/usr/bin/env python3

import os
import platform
import subprocess
import sys
import argparse

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument("--cx", help="path to cx compiler executable", default="cx")
args, cx_args = arg_parser.parse_known_args()

os.chdir(os.path.dirname(__file__))

for file in os.listdir("."):
    if platform.system() == "Windows" and file in ["tree.cx", "asteroids", "opengl"]:
        continue

    if file.endswith(".cx"):
        output = os.path.splitext(file)[0] + (".exe" if platform.system() == "Windows" else "")
        exit_status = subprocess.call([args.cx, file, "-o", output, "-Werror"])
        os.remove(output)
    elif file != "inputs" and os.path.isdir(file):
        env = os.environ.copy()
        env["PATH"] = os.path.dirname(args.cx) + ":" + env["PATH"]
        exit_status = subprocess.call(["make", "-C", file, "CXFLAGS=" + " ".join(cx_args)], env=env)
    else:
        continue

    if exit_status != 0:
        sys.exit(1)

print("All examples built successfully.")
