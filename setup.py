import sys

try:
    from skbuild import setup
except ImportError:
    print('scikit-build is required to build from source.', file=sys.stderr)
    print('Please run:', file=sys.stderr)
    print('', file=sys.stderr)
    print('  python -m pip install scikit-build')
    sys.exit(1)

setup(
    name="pyblend2d",
    version="0.0.11",
    description="Blend2D bindings",
    author='Till Stensitzki',
    license="MIT",
    packages=['hello'],
   #cmake=['-DHELLO_BUILD_TESTING:BOOL=TRUE',]
)