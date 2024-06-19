#! /bin/python3 -m

# To use this script, run `python3 -m <NAME_OF_FILE> ...` or mark this 
# script as an executable and run it.
# 
# To get all usable flags to this script, run it with --help or take a
# look at the function called `parse_args`.
#
# See end of file for license information.

import sys, os, argparse
import json, hashlib
import subprocess
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path
from collections.abc import Iterable
from collections import namedtuple

################################################
# Configuable variables
################################################

PROGRAM_NAME = "mylang"

LINK_LIBS: list = ["-lc"]

SOURCE_DIR = "src"
CACHE_DIR = "buildcache"

HOOK_FILES: tuple | None = None

DEFAULT_SOURCE_DIR = Path(__file__).parent / Path(SOURCE_DIR)
DEFAULT_CACHE_DIR = Path(__file__).parent / Path(CACHE_DIR)

C_COMPILER: str = "cc"
CFLAGS: tuple | None = ("-Wall", "-Wextra", "-Wpedantic", "-Wvla")
COPT: str = "-O2 -g3" # I never recommend using -O0

CPPFLAGS: tuple | None = None # The standard cpp hook uses the c flags as well
CPPOPT: str = "-O1"

LINK_WITH: str = "cc"
LINK_OPTS: str = "-O2 -g3"

################################################
# End of configuable arguments
################################################

VERSION = "0.1-r2"

def parse_args():
    args = argparse.ArgumentParser(prog="build")

    args.add_argument("--version",
            action="version",
            version=f"Buildscript version: %(prog)s {VERSION}",
            )
    
    args.add_argument("-B", "--always-make",
            action="store_true",
            help="unconditionally make all targets and regenerate new cache (does not delete cache)",
            dest="new_cache"
            )

    args.add_argument("-d", "--dir", 
            type=Path,
            metavar="DIR",
            default=Path(DEFAULT_CACHE_DIR),
            help=f"cache directory, default: {DEFAULT_CACHE_DIR}",
            dest="cache_dir"
            )

    return args.parse_args()


def get_sources(path: Path):
    sources = []
    for p in path.iterdir():
        if p.is_dir(): 
            sources.extend(get_sources(p))
        if p.is_file():
            sources.append(p)

    return sources


class BuildHooks:
    def __init__(self):
        self.pre_hooks = list()
        self.file_handlers = dict()
        self.file_specific_hooks = dict()
        self.post_hooks = list()
        self.linker_func = None

    def pre_hook(self, func):
        """
        Pre hook functions. Runs before compiling.
        The registered funtions will all recieve a copy of a list of strings
        with paths pointing to the all the source files that is passed to the 
        compiling step.
        """
        self.pre_hooks.append(func)
        return func

    def post_hook(self, func):
        """
        Post hook functions. Runs before linking step.
        The registered funtions will all recieve a copy of a list of strings
        with paths pointing to the all the object files that is passed to the 
        linker step.
        """
        self.post_hooks.append(func)
        return func

    def file_handler(self, suffix: str):
        """
        Function parameters: (src: str, obj: str) -> Iterable | None
        src is the path to the source file as a string
        obj is the path to the object file to be created as a string

        Returning None implies that the file ought not to be processed.
        Otherwise, it should return an iterable object, excluding str.
        """
        def decorator(func):
            self.file_handlers.update({suffix: func})
            return func
        return decorator

    def file_specific_hook(self, filenames: str | tuple[str]):
        """
        File specific hook.
        Function parameters: (src: str, obj: str) -> Iterable | None
        src is the path to the source file as a string
        obj is the path to the object file to be created as a string

        Returning None implies that the file ought not to be processed.
        Otherwise, it should return an iterable object, excluding str.
        """
        def decorator(func):
            if isinstance(filenames, str):
                self.file_specific_hooks.update({filenames : func})
            elif isinstance(filenames, tuple):
                self.file_specific_hooks.update({k:func for k in filenames})
            else:
                raise AttributeError("string or tuple")
            return func
        return decorator
    
    def link_hook(self, func):
        """
        Function parameters: (objs: list[str], exe: str) -> Iterable | None
        The function gets a list of paths to objects in string format to link together
        and a path in string format link the final executable into
        
        Returning None implies that it should not do any linking.
        Otherwise, it should reutrn an interable object, exlcuding str.
        """
        self.linker_func = func
        return func


build_hooks = BuildHooks()


class Cache:
    def __init__(self, filename: Path, new_cache: bool):
        self.filename = filename
        self.cached_files = dict()
        self.new_cache = new_cache
        self.hash_func = None

    def __enter__(self):
        json_data = dict()
        if not self.new_cache and self.filename.exists():
            with self.filename.open("r") as cache:
                # If it fails for some reason, create a new one
                try:
                    json_data = json.load(cache)
                except:
                    print("Invalid json: {str(sys.exception())}", 
                          "Making a new cache", 
                          "This might take time depending on project size", 
                          sep='\n', file=sys.stderr)
            
        self.hash_func = json_data.get("hash", "sha256")
        self.cached_files = json_data.get("files", dict())
        return self

    def __exit__(self, typ, val, trb):
        if not self.filename.exists():
            print(f"Creating {self.filename}")
        with self.filename.open("w") as cache:
            json.dump({"hash": self.hash_func, "files": self.cached_files}, cache, indent=3)

    def calculate_hash(self, file: Path):
        """Calculates the file hash and returns the hash of the file"""
        hash_func = hashlib.new(self.hash_func)
        try:
            file_io_blocks = file.stat().st_blksize 
        except AttributeError:
            file_io_blocks = 4096

        with file.open("rb") as f:
            while chunk := f.read(file_io_blocks):
                hash_func.update(chunk)
         
        return hash_func.hexdigest()
    
    def get_hash(self, file: Path, value=None):
        return self.cached_files.get(str(file), value)
    
    def update_hash(self, file: Path, hsh):
        self.cached_files.update({str(file): hsh})


@build_hooks.file_handler(".c")
def cc(src, obj):
    if CFLAGS is None:
        return (C_COMPILER, COPT, "-c", src, "-o", obj)

    return (C_COMPILER, *CFLAGS, COPT, "-c", src, "-o", obj)

@build_hooks.file_handler(".cpp")
def cxx(src, obj):
    cmd = ["g++"]

    if CFLAGS is not None:
        cmd.extend(CFLAGS)
    if CPPFLAGS is not None:
        cmd.extend(CPPFLAGS)

    cmd.extend(("-c", src, "-o", obj))
    return cmd

@build_hooks.file_handler(".h")
def h_files(src, obj):
    return None # do nothing

@build_hooks.file_handler(".hpp")
def hpp_files(src, obj):
    return None # do nothing

@build_hooks.file_handler(".swp")
def swp_files(src, obj):
    return None # do nothing

@build_hooks.link_hook
def linker_func(objs: list[str], exe):
    if LINK_LIBS is None:
        return (LINK_WITH, LINK_OPTS, "-o", exe, *objs)

    return (LINK_WITH, LINK_OPTS, "-o", exe, *objs, *LINK_LIBS)


def check_and_make_cmd(cmd):
    if not isinstance(cmd, Iterable):
        raise TypeError(f"function {file_handler.__name__} did not return an iterable")

    if isinstance(cmd, str):
        raise TypeError(f"function {file_handler.__name__} returned a string")

    cmd_len = len(cmd)
    if cmd_len == 0:
        raise TypeError(f"funciton {file_handler.__name__} returned an empty iterable")
    else:
        return ' '.join(cmd)


RunHandlerRet = namedtuple("RunHandlerRet", ["process", "src", "obj", "cached"])
def run_handler(handler, src, obj, cached):

    cmd = handler(str(src), obj)

    if cmd is None:
        return RunHandlerRet(None, src, obj, cached)

    if cached[0]:
        return RunHandlerRet((), src, obj, cached)

    runnable_cmd = check_and_make_cmd(cmd)

    print(*cmd)

    return RunHandlerRet(
            subprocess.run(runnable_cmd,
                shell=True,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT
                ),
            src,
            obj,
            cached
            )

def main():
    global build_hooks
    args = parse_args()
    cache_file = args.cache_dir / Path("file_cache.json")

    if HOOK_FILES is not None:
        for hook_file in HOOK_FILES:
            with open(hook_file, "r") as f:
                exec(compile(f.read(), hook_file, "exec"), {"build_hooks": build_hooks})

    if not args.cache_dir.exists():
        print(f"Making directory: {args.cache_dir}")
        args.cache_dir.mkdir()

    cwd = Path.cwd()
    sources = list()

    for path in args.src_dirs:
        if not path.exists():
            raise FileNotFoundError(f"{str(path)} does not exsist")
        
        sources.extend(
            map(lambda s: s.relative_to(cwd), get_sources(path))
            )

    for func in build_hooks.pre_hooks:
        func(list(sources))

    link_objs = list()
    source_not_changed = True 

    with Cache(cache_file, args.new_cache) as cache:
        executor = ProcessPoolExecutor()

        file_futures = list()

        for src, obj in zip(sources, map(lambda s: str(args.cache_dir / s) + ".o", sources)):
            handler = build_hooks.file_specific_hooks.get(str(src))
            
            if handler is None:
                handler = build_hooks.file_handlers.get(src.suffix)
            
            if handler is None:
                executor.shutdown(cancel_futures=True)
                print(f"No handler for {src.suffix!r} files")
                return 0

            Path(obj).parent.mkdir(exist_ok=True)

            hsh = cache.calculate_hash(src)
            cached = (cache.get_hash(src) == hsh, hsh)

            file_futures.append(executor.submit(run_handler, handler, src, obj, cached))

        for future in as_completed(file_futures):
            result = future.result()

            if result.process is None:
                continue

            link_objs.append(result.obj)

            if result.cached[0]:
                continue

            source_not_changed &= result.cached[0]

            print(result.process.stdout, end='')

            if result.process.returncode != 0:
                executor.shutdown(cancel_futures=True)
                return result.process.returncode

            cache.update_hash(result.src, result.cached[1])

        executor.shutdown()

    if source_not_changed:
        print("Nothing to compile")
        return 0
    
    for func in build_hooks.post_hooks:
        func(list(link_objs))
    
    if source_not_changed:
        return 0

    linker_cmd = build_hooks.linker_func(link_objs, PROGRAM_NAME)

    if linker_cmd is None:
        return 0
    
    print(*linker_cmd)

    linker = subprocess.run(check_and_make_cmd(linker_cmd),
            shell=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT
            )

    print(linker.stdout, end='')

    return linker.returncode

if __name__ == "__main__": exit(main())

# Copyright (c) 2024 lindfors.freja@gmail.com
# 
# Permission is hereby granted, free of charge, to any person obtaining a 
# copy of this software and associated documentation files (the "Software"), 
# to deal in the Software without restriction, including without limitation 
# the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the 
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
# DEALINGS IN THE SOFTWARE.
