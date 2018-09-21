load(
    "//tools/build_defs/oss/osquery:platforms.bzl",
    _FREEBSD = "FREEBSD",
    _LINUX = "LINUX",
    _MACOSX = "MACOSX",
    _WINDOWS = "WINDOWS",
)
load(
    ":defaults.bzl",
    _LABELS = "OSQUERY_LABELS",
)

_PREPROCESSOR_FLAGS = [
    "-DOSQUERY_VERSION=3.3.0",
    "-DOSQUERY_BUILD_VERSION=3.3.0",
    "-DOSQUERY_BUILD_SDK_VERSION=3.3.0",
]

_PLATFORM_PREPROCESSOR_FLAGS = [
    (
        _LINUX,
        [
            "-DLINUX=1",
            "-DPOSIX=1",
            "-DOSQUERY_LINUX=1",
            "-DOSQUERY_POSIX=1",
            "-DOSQUERY_BUILD_PLATFORM=linux",
            "-DOSQUERY_BUILD_DISTRO=centos7",
        ],
    ),
    (
        _MACOSX,
        [
            "-DAPPLE=1",
            "-DDARWIN=1",
            "-DBSD=1",
            "-DPOSIX=1",
            "-DOSQUERY_POSIX=1",
            "-DOSQUERY_BUILD_PLATFORM=darwin",
            "-DOSQUERY_BUILD_DISTRO=10.12",
        ],
    ),
    (
        _FREEBSD,
        [
            "-DFREEBSD=1",
            "-DBSD=1",
            "-DPOSIX=1",
            "-DOSQUERY_POSIX=1",
            "-DOSQUERY_FREEBSD=1",
            "-DOSQUERY_BUILD_PLATFORM=freebsd",
            "-DOSQUERY_BUILD_DISTRO=11.2",
        ],
    ),
    (
        _WINDOWS,
        [
            "-DWINDOWS=1",
            "-DOSQUERY_WINDOWS=1",
            "-DOSQUERY_BUILD_PLATFORM=windows",
            "-DOSQUERY_BUILD_DISTRO=10",
        ],
    ),
]

def _osquery_set_generic_kwargs(kwargs):
    kwargs.setdefault("labels", [])
    kwargs["labels"] += _LABELS

def _osquery_set_preprocessor_kwargs(kwargs):
    kwargs.setdefault("preprocessor_flags", [])
    kwargs["preprocessor_flags"] += _PREPROCESSOR_FLAGS

    kwargs.setdefault("platform_preprocessor_flags", [])
    kwargs["platform_preprocessor_flags"] += _PLATFORM_PREPROCESSOR_FLAGS

def osquery_cxx_library(external = False, **kwargs):
    _osquery_set_generic_kwargs(kwargs)
    if not external:
        _osquery_set_preprocessor_kwargs(kwargs)
    native.cxx_library(**kwargs)

def osquery_prebuilt_cxx_library(**kwargs):
    _osquery_set_generic_kwargs(kwargs)
    native.prebuilt_cxx_library(**kwargs)

def osquery_prebuilt_cxx_library_group(**kwargs):
    kwargs.setdefault("labels", [])
    kwargs["labels"] += _LABELS
    native.prebuilt_cxx_library_group(**kwargs)

def osquery_cxx_binary(external = False, **kwargs):
    _ignore = [external]
    _osquery_set_generic_kwargs(kwargs)
    _osquery_set_preprocessor_kwargs(kwargs)
    native.cxx_binary(**kwargs)

def osquery_cxx_test(external = False, **kwargs):
    _ignore = [external]
    _osquery_set_generic_kwargs(kwargs)
    _osquery_set_preprocessor_kwargs(kwargs)
    native.cxx_test(**kwargs)
