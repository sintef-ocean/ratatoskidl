from conans import ConanFile, CMake, tools
from os import path


class RatatoskIDLConan(ConanFile):
    name = "RatatoskIDL"
    settings = None
    author = "SINTEF Ocean"
    exports = "version.txt"
    exports_sources = "*"
    description = "Ratatosk Interface Definition Language (IDL) files"
    license = "Apache-2.0"
    topics = ("DDS", "OpenSplice", "IDL")
    url = "https://github.com/sintef-ocean/ratatoskidl"
    generators = ("cmake_paths")
    options = {
      "with_doc": [True, False],
      "with_tests": [True, False],
      "with_CICD": [True, False]
    }

    default_options = (
      "with_doc=False",
      "with_tests=False",
      "with_CICD=False"
    )

    _cmake = None

    def build_requirements(self):

        if self.options.with_CICD:
            self.output.info("Using SfhBuildScripts")  # For SfhDocHelpers
            self.build_requires("sfhbuildscripts/[>=3.2.0]@sintef/stable")

        if self.options.with_tests:
            self.build_requires("opensplice-ce/[>=6.9]@sintef/stable")

    def _configure_cmake(self):
        if self._cmake is None:
            self._cmake = CMake(self)
            self._cmake.definitions["WITH_DOC"] = self.options.with_doc
            self._cmake.definitions["WITH_TESTS"] = self.options.with_tests
            self._cmake.configure()
        return self._cmake

    def set_version(self):
        self.version = \
            tools.load(path.join(self.recipe_folder, "version.txt")).strip()

    def build(self):

        if(self.options.with_doc):
            cmake = self._configure_cmake()
            cmake.build(target='doc')

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.name = "RatatoskIDL"

    def package_id(self):
        del self.info.options.with_CICD
        del self.info.options.with_tests
