from conans import ConanFile, CMake
import os

version = os.getenv('CONAN_LIBEXECSTREAM_VERSION', '0.3.0')
channel = os.getenv('CONAN_LIBEXECSTREAM_CHANNEL', 'testing')
user = os.getenv('CONAN_LIBEXECSTREAM_USER', 'Manu343726')

class TestLibExecStream(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    requires = 'libexecstream/{}@{}/{}'.format(version, user, channel), 'cmake-utils/0.0.0@Manu343726/testing'
    generators = 'cmake'
    test_exec = 'example'

    def build(self):
        cmake = CMake(self.settings)
        self.run('cmake {} {}'.format(self.conanfile_directory, cmake.command_line))
        self.run('cmake --build . {}'.format(cmake.build_config))

    def test(self):
        self.run(os.path.join('.', 'bin', TestLibExecStream.test_exec))
