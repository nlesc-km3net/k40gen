from distutils.version import LooseVersion
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test as test_ext
import sys
import os
import re
import platform
import subprocess
import sysconfig

__version__ = '0.0.1'


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)',
                                                   out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
                cfg.upper(),
                extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j1']

        if 'CONDA_EXE' in os.environ:
            # Building in a conda environment, use the special
            # toolchain file, ROOT doesn't seem to work
            cmake_args += ['-DENABLE_ROOT=OFF',
                           '-DCMAKE_INSTALL_PREFIX={}'.format(os.environ['CONDA_PREFIX']),
                           '-DCMAKE_TOOLCHAIN_FILE={}/anaconda-toolchain.cmake'.format(ext.sourcedir)]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)
        print()  # Add an empty line for cleaner output


class CatchTestCommand(test_ext):
    """
    A custom test runner to execute both Python unittest tests and C++ Catch-
    lib tests.
    """

    def distutils_dir_name(self, dname):
        """Returns the name of a distutils build directory"""
        dir_name = "{dirname}.{platform}-{version[0]}.{version[1]}"
        return dir_name.format(dirname=dname,
                               platform=sysconfig.get_platform(),
                               version=sys.version_info)

    def run(self):
        # Run CMake tests
        subprocess.call(['ctest -V'],
                        cwd=os.path.join('build',
                                         self.distutils_dir_name('temp')),
                        shell=True)


ext_modules = [
    CMakeExtension('k40gen')
]


setup(
    name='k40gen',
    version=__version__,
    author='Roel Aaij',
    author_email='roelaaij@nikhef.nl',
    url='https://gitlab.nikhef.nl/roelaaij/k40gen',
    description='standalone background generator for KM3NeT',
    long_description='',
    ext_modules=ext_modules,
    setup_requires=['pytest-runner', 'numpy', 'cmake'],
    install_requires=['numpy'],
    tests_require=["pytest"],
    cmdclass=dict(build_ext=CMakeBuild, test=CatchTestCommand),
    zip_safe=False,
)
