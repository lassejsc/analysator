from setuptools import setup, Extension
import numpy
setup(
    name="demo",
    version="1.0",
    description="This is a demo package",
    ext_modules=[Extension("cpphelpers", sources=["cpphelpers.cpp"])],
    include_dirs=[numpy.get_include()]
)
