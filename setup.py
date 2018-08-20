from setuptools import find_packages, setup, Command, Extension

PACKAGE = 'ble_serial'
BASE_VERSION = '0.1'
DEPENDENCIES = ['numpy']

dependencies = DEPENDENCIES

setup(name=PACKAGE,
      version=BASE_VERSION,
      packages=find_packages(exclude=('tests',)),
      ext_modules=[Extension(
          "ble_serial",
          ["ble_serial.c"],
          language='c++14')],
      )
