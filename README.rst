


Introduction
------------

This project provides Interface Definition Language files as import targets. The
IDL-files are located in ``src`` and the convention is that each folder is a target
``RatatoskIDL::<folder_name>``, which can be used in other projects. Note that this
project uses an *Exact versioning* scheme, so the requested version must match exactly,
*major.minor.patch*. The following snippet shows how a consumer can use this project.

.. code-block:: cmake

    find_package(OpenSplice 6.9 REQUIRED)
    find_package(RatatoskIDL 0.2.0 CONFIG REQUIRED)

    # pingpong is the target with IDL sources
    ratatosk_add_idl_library(ping-dds pingpong)
    add_executable(test-ping ping.cpp)
    target_link_libraries(test-ping ping-dds)

``RatatoskIDL`` defines pseudo-targets with a property ``IDL_SOURCES``, which is a list of
IDL files for that target. A full example is provided in ``examples``. To list targets
that ``RatatoskIDL`` provides, see the file hierarchy in the documentation or browse the
source directory ``<INSTALL_PREFIX>/share/RatatoskIDL``, e.g with
``<INSTALL_PREFIX>=/usr/local/`` on Linux.

The CMake function ``ratatosk_add_idl_library()`` creates an *OBJECT* library, which
provides the generated interface header files during building, that can be linked to the
library or executable in need of the interface. So e.g. for ``pingpong``:

.. code-block:: cpp

    // <target_name>/<idl_filename>_DCPS.hpp
    #include "pingpong/ping_DCPS.hpp"

Building
~~~~~~~~

The IDL files are architecture-independent and require no compiler. Only ``CMake >= 3.10`` is needed. Building and installing will make it available to other projects with
``find_package`` using the ``CONFIG`` flag. There are also ``.deb`` and ``.tar.gz`` archive
packages.

.. code-block:: bash

    mkdir build && cd build
    cmake .. -DWITH_DOC=OFF
    cmake --build . --target install
    cmake --build . --target package  # Build .deb and/or .tar.gz

Build options and conan
^^^^^^^^^^^^^^^^^^^^^^^

``RatatoskIDL`` is available as a conan package. ``RatatoskIDL`` itself does not depend on
OpenSplice unless the CMake function ``ratatosk_add_idl_library()`` is used. The
``examples`` directory provides an example on how to use it as a consumer. The conan
package is available by adding the following conan remote:

.. code-block:: bash

    conan remote add sintef https://artifactory.smd.sintef.no/artifactory/api/conan/conan-local
    conan config set general.revisions_enabled=1

First, you can add the following to your `conanfile.txt <https://docs.conan.io/en/latest/reference/conanfile_txt.html>`_:

.. code-block:: ini

    [requires]
    RatatoskIDL/=0.2.0@sintef/stable
    opensplice-ce/[>=6.9]@sintef/stable

    [generators]
    cmake_paths
    cmake
    virtualenv

    [options]
    RatatoskIDL:with_doc=False

Then, add this to CMakeLists.txt before the CMake snippet above.

.. code-block:: cmake

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)


.. table::

    +-----------------------------+-------------+--------------------------------------------------+
    | Option (conan/CMake)        | Default     | Comment                                          |
    +=============================+=============+==================================================+
    | ``with_doc / WITH_DOC``     | False / OFF | Build documentation                              |
    +-----------------------------+-------------+--------------------------------------------------+
    | ``with_tests / WITH_TESTS`` | False / OFF | Build IDL targets to check syntax                |
    +-----------------------------+-------------+--------------------------------------------------+
    | ``with_CICD`` / N/A         | False       | Internal CI/CD build script as build requirement |
    +-----------------------------+-------------+--------------------------------------------------+
    | \                           | \           | \                                                |
    +-----------------------------+-------------+--------------------------------------------------+

The table above shows available configuration options for the package.

``WITH_TESTS=ON``
    Build the libraries for C++ (isocpp2), as a way of confirming
    that the IDL syntax is correct. This test requires OpenSplice.

``WITH_DOC=ON``
    Build documentation using ``doxygen`` and ``sphinx``, see requirements
    below.

``with_CICD=True``
    A conan-only requirement to add internal ``SfhBuildScripts``.


**Documentation requirements *(optional)***

The documentation is built with the help of ``doxygen`` and ``sphinx``. There are also
additional python packages listed in ``doc/requirements.txt``. The requirements can be
installed with:

**Debian linux**:

.. code-block:: bash

    apt-get install doxygen
    python -m pip install -r doc/requirements.txt --upgrade

**Windows**, assuming you have the `chocolatey <https://chocolatey.org/>`_ package manager:

.. code-block:: bash

    choco install -y doxygen.install
    python -m pip install -r doc/requirements.txt --upgrade

Adding new idl libraries to this repository
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a new library is to be added, the developer needs to manually edit
``CMakeLists.txt``. Suppose you have created IDL files ``hugin.idl`` and ``munin.idl``. You
want them to be part of a component named ``Midgard``.

1. Place ``{hugin.idl, munin.idl}`` in ``src/Midgard/``

2. Add to ``CMakeLists.txt``:  ``ratatosk_idl_lib(Midgard "hugin.idl munin.idl")``

3. Increase the version of the project.

4. Update version in ``conanfile.txt`` snippet in this readme and in ``examples/conanfile.txt``.

Example folder
~~~~~~~~~~~~~~

To build the example, please make sure that ``RatatoskIDL`` and ``openoplice-ce`` are
properly installed. This is easily achived using the example ``conanfile.txt``.

.. code-block:: bash

    conan remote add sintef https://artifactory.smd.sintef.no/artifactory/api/conan/conan-local
    cd examples && mkdir build && cd build
    conan install ..
    cmake ..
    cmake --build .
    . activate.sh  # sets OpenSplice environment variables
    bin/test-ping
    # And in another terminal:
    . activate.sh
    bin/test-pong
