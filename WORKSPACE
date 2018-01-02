
http_archive (
    name = "com_google_absl",
    strip_prefix = "abseil-cpp-master",
    urls = ["https://github.com/abseil/abseil-cpp/archive/master.zip"]
)

new_http_archive (
    name = "nlohmann_json",
    strip_prefix = "json-master",
    urls = ["https://github.com/nlohmann/json/archive/master.zip"],
    build_file = "tools/json.bazel"
)

http_archive (
    name = "com_github_nelhage_boost",
    strip_prefix = "rules_boost-master",
    urls = ["https://github.com/nelhage/rules_boost/archive/master.zip"]
)

load("@com_github_nelhage_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

new_http_archive (
    name = "com_libgit2",
    strip_prefix = "libgit2-0.26.0/src",
    urls = ["https://github.com/chigraph/chigraph/releases/download/dependencies/libgit2-0.26.0-ascii.zip"],
    build_file = "tools/libgit2.bazel"
)

http_archive(
    name = "catch",
    url = "https://github.com/PlacidBox/catch-bazel/archive/v1.9.6.tar.gz",
    strip_prefix = "catch-bazel-1.9.6",
)
