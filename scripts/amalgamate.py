#!/usr/bin/env python3

#
# This script generates an amalgamated header and writes it to stdout
#

import io
import os
import re
import sys

ROOT_PATH = os.path.dirname(os.path.dirname(__file__))
INCLUDE_PATH = os.path.join(ROOT_PATH, "include")
RE_INCLUDE = re.compile(
    r'^\s*#include\s+(?P<path>("[^"]+"|\<[^>]+\>))\s*$', re.MULTILINE
)
RE_UNDEF = re.compile(r"^\s*#undef\s+(linux|unix)\s*$", re.MULTILINE)
RE_PRAGMA_ONCE = re.compile(r"^\s*#pragma\s+once\s*$", re.MULTILINE)


class HeaderInclude:
    def __init__(self, path, is_angle, is_lib, is_cheader):
        self.path = path
        self.is_angle = is_angle
        self.is_lib = is_lib
        self.is_cheader = is_cheader

    def __repr__(self):
        return f"<HeaderInclude '{self.path}'>"

    def __eq__(self, other):
        return self.path == other.path

    def __ne__(self, other):
        return self.path != other.path

    def __hash__(self):
        return hash(self.path)


class HeaderFile:
    def __init__(self, basedir, filename):
        self.basedir = basedir
        self.filename = filename
        self.contents = ""
        self.includes = []
        self.depends = []
        self._load()

    def _load(self):
        with open(self.fullpath) as f:
            self.contents = f.read()
        self.includes = self._find_includes()
        for include in self.includes:
            if not include.is_lib:
                continue
            header = HeaderFile(self.basedir, include.path)
            self.depends.append(header)
        self.contents = RE_INCLUDE.sub("", self.contents)
        self.contents = RE_UNDEF.sub("", self.contents)
        self.contents = RE_PRAGMA_ONCE.sub("", self.contents)
        if self.contents.strip() == "#pragma once":
            self.contents = ""

    def _find_includes(self):
        includes = []
        for match in RE_INCLUDE.finditer(self.contents):
            path = match.group("path")
            is_angle = False
            if path.startswith("<") and path.endswith(">"):
                path = path[1 : len(path) - 1]
                is_angle = True
            elif path.startswith('"') and path.endswith('"'):
                path = path[1 : len(path) - 1]
            else:
                raise Exception("header include missing angle or double quotes")
            is_lib = path.startswith("turbine/")
            is_cheader = path.endswith(".h")
            include = HeaderInclude(path, is_angle, is_lib, is_cheader)
            includes.append(include)
        return sorted(includes, key=lambda x: x.path)

    @property
    def fullpath(self):
        return os.path.join(self.basedir, self.filename)

    def __repr__(self):
        return f"<HeaderFile '{self.filename}'>"

    def __eq__(self, other):
        return self.fullpath == other.fullpath

    def __ne__(self, other):
        return self.fullpath != other.fullpath

    def __hash__(self):
        return hash(self.fullpath)


def find_headers(dir_path=INCLUDE_PATH):
    files_list = []
    for root, dirs, files in os.walk(dir_path):
        for file in files:
            if file.endswith(".hpp"):
                path = os.path.join(root, file)
                if path.startswith(INCLUDE_PATH):
                    path = path[len(INCLUDE_PATH) + 1 :]
                header = HeaderFile(dir_path, path)
                files_list.append(header)
    return sorted(files_list, key=lambda x: x.filename)


def split_includes(header_list):
    c_includes = set()
    cxx_includes = set()
    lib_includes = set()
    for header in header_list:
        for include in header.includes:
            if include.is_angle and not include.is_lib and not include.is_cheader:
                cxx_includes.add(include)
            elif include.is_angle and not include.is_lib and include.is_cheader:
                c_includes.add(include)
            elif include.is_lib:
                lib_includes.add(include)
    c_includes = sorted(list(c_includes), key=lambda x: x.path)
    cxx_includes = sorted(list(cxx_includes), key=lambda x: x.path)
    lib_includes = sorted(list(lib_includes), key=lambda x: x.path)
    return (c_includes, cxx_includes, lib_includes)


def list_headers_ordered(header_list):
    headers_set = set()
    headers_ordered = []

    def list_header(header):
        for dep_header in header.depends:
            list_header(dep_header)
        if header not in headers_set:
            headers_ordered.append(header)
            headers_set.add(header)

    for header in header_list:
        list_header(header)
    return headers_ordered


def generate_code(headers_ordered, c_includes, cxx_includes):
    output = io.StringIO()

    def put(x):
        output.write(x)

    put("// This file is auto-generated, do not edit.\n")
    put("#pragma once\n")
    put("\n")

    def put_include(include):
        put("#include ")
        if include.is_angle:
            put("<")
        else:
            put('"')
        put(include.path)
        if include.is_angle:
            put(">")
        else:
            put('"')
        put("\n")

    for include in c_includes:
        put_include(include)

    put("\n")

    for include in cxx_includes:
        put_include(include)

    put("\n")

    put("// Undefine these macros since they conflict with namespaces\n")
    put("#undef linux\n")
    put("#undef unix\n")

    put("\n")

    for header in headers_ordered:
        contents = header.contents.strip()
        if contents:
            put(f"// {header.filename}\n")
            put(contents)
            put("\n\n")

    return output.getvalue().rstrip() + "\n"


def main(args):
    all_headers = find_headers()
    headers_ordered = list_headers_ordered(all_headers)
    c_includes, cxx_includes, lib_includes = split_includes(all_headers)
    output_code = generate_code(headers_ordered, c_includes, cxx_includes)
    sys.stdout.write(output_code)


if __name__ == "__main__":
    sys.exit(main(sys.argv))
