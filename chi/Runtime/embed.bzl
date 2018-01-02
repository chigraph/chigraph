def _impl(ctx):
    input = ctx.file.file
    output = ctx.outputs.out

    ctx.actions.run_shell(
        inputs = [input],
        outputs = [output],
        command = "(echo \\#include \\<string\\>; echo std::string {}\\(\\) {{ return {{ $(hexdump -ve '1/1 \"(char)0x%.2x,\"' {}) }}\\;}}) > {}".format(ctx.attr.function_name, input.path, output.path)
    )

embed_file_in_cpp = rule (
    implementation = _impl,
    attrs = {
        "file": attr.label(mandatory=True, allow_files=True, single_file=True),
        "function_name": attr.string(mandatory=True),
    },
    outputs = {
        "out": "%{name}.cpp"
    }
)