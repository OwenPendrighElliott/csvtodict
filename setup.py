from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11

class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': [],
    }

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append('-std=c++11')

        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)

# Pybind11 extension
ext_modules = [
    Extension(
        'csvtodict',
        ['src/csvtodict.cpp'],
        include_dirs=[
            pybind11.get_include(user=True),  # Pybind11 include dir
            pybind11.get_include()            # System Pybind11 include dir
        ],
        language='c++'
    ),
]

setup(
    name='csvtodict',
    version='0.1',
    author='Owen Elliott',
    author_email='owenelliott@duck.com',
    description='A fast CSV file reader',
    long_description='',
    ext_modules=ext_modules,
    install_requires=['pybind11>=2.5.0'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)
