#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools, RunEnvironment


class RatatoskIDLTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = ("cmake_paths", "cmake", "virtualenv")
    requires = ("opensplice-ce/[>=6.9]@sintef/stable")

    def build(self):
        if not tools.cross_building(self.settings):
            env_build = RunEnvironment(self)
            with tools.environment_append(env_build.vars):
                cmake = CMake(self)
                cmake.configure()
                cmake.build()
                cmake.test()

    def imports(self):
        pass

    def test(self):
        if not tools.cross_building(self.settings):
            print("SUCCESS")
        else:
            print("NOT_RUN (cross-building)")
