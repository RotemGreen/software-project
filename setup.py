from setuptools import Extension, setup

module = Extension("symnmfmodule", sources=['symnmfmodule.c', 'symnmf_algo.c', 'symnmf_io.c', 'matrix_management.c'])
setup(name='symnmfmodule',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])
