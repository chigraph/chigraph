workspace(name = "org_chigraph_chigraph")

new_http_archive(
    name = "nlohmann_json",
    build_file = "third_party/json.bazel",
    strip_prefix = "json-master",
    urls = ["https://github.com/nlohmann/json/archive/master.zip"],
)

http_archive(
    name = "com_github_nelhage_boost",
    strip_prefix = "rules_boost-master",
    urls = ["https://github.com/nelhage/rules_boost/archive/master.zip"],
)

load("@com_github_nelhage_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

new_http_archive(
    name = "libgit2",
    build_file = "third_party/libgit2.bazel",
    strip_prefix = "libgit2-0.26.0",
    urls = ["https://github.com/chigraph/chigraph/releases/download/dependencies/libgit2-0.26.0-ascii.zip"],
)

http_archive(
    name = "catch",
    strip_prefix = "catch-bazel-1.9.6",
    url = "https://github.com/PlacidBox/catch-bazel/archive/v1.9.6.tar.gz",
)

new_http_archive(
    name = "llvm",
    build_file = "//:third_party/llvm/llvm.BUILD",
    strip_prefix = "llvm-release_50",
    urls = [
        "https://github.com/llvm-mirror/llvm/archive/release_50.tar.gz",
    ],
)

# new_local_repository(
#     name = "llvm",
#     build_file = "//third_party/llvm:llvm.BUILD",
#     path = "/home/russellg/projects/llvm-release_50",
# )

new_http_archive(
    name = "zlib_archive",
    build_file = "//:third_party/zlib.BUILD",
    sha256 = "36658cb768a54c1d4dec43c3116c27ed893e88b02ecfcb44f2166f9c0b7f2a0d",
    strip_prefix = "zlib-1.2.8",
    urls = [
        "http://zlib.net/fossils/zlib-1.2.8.tar.gz",
    ],
)
