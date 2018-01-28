package(default_visibility = ["//visibility:public"])

load(
    "@org_chigraph_chigraph//:third_party/llvm/llvm.bzl",
    "gentbl",
    "template_rule",
    "expand_cmake_vars",
    "cmake_var_string",
)

cc_binary (
    name = "tablegen",
    srcs = glob([
        "utils/TableGen/*.h",
        "utils/TableGen/*.cpp",
    ]),
    deps = [
        "@llvm//:table_gen",
    ],
)

diagnosticKinds = ["Analysis", "AST", "Comment", "Common", "Driver", "Frontend", "Lex", "Parse", "Sema", "Serialization"]

# tablegen
tabelgen_targets = [
    {
        "name": "diagnostic",
        "td_file": "include/clang/Basic/Diagnostic.td",
        "srcs": ["include/clang/Basic/" + s for s in ["DiagnosticDocs.td", "DiagnosticCategories.td", "DiagnosticGroups.td", ] + 
            ["Diagnostic%sKinds.td" % kind for kind in diagnosticKinds]],
        "tbl_outs": [
            ("-gen-clang-diags-defs -clang-component=%s" % kind, "include/clang/Basic/Diagnostic%sKinds.inc" % kind)
            for kind in diagnosticKinds
        ] + [
            ("-gen-clang-diag-groups", "include/clang/Basic/DiagnosticGroups.inc"),
            ("-gen-clang-diags-index-name", "include/clang/Basic/DiagnosticIndexName.inc"),
        ]
    },
    {
        "name": "attr",
        "td_file": "include/clang/Basic/Attr.td",
        "srcs": ["include/clang/Basic/" + s for s in ["AttrDocs.td", "DeclNodes.td", "StmtNodes.td"]],
        "tbl_outs": [
            ("-gen-clang-attr-list", "include/clang/Basic/AttrList.inc"),
            ("-gen-clang-attr-subject-match-rule-list", "include/clang/Basic/AttrSubMatchRulesList.inc"),
            ("-gen-clang-attr-has-attribute-impl", "include/clang/Basic/AttrHasAttributeImpl.inc"),
            ("-gen-clang-attr-classes", "include/clang/AST/Attrs.inc"),
            ("-gen-clang-attr-impl", "include/clang/AST/AttrImpl.inc"),
            ("-gen-clang-attr-dump", "include/clang/AST/AttrDump.inc"),
            ("-gen-clang-attr-ast-visitor", "include/clang/AST/AttrVisitor.inc"),
            ("-gen-clang-attr-template-instantiate", "include/clang/Sema/AttrTemplateInstantiate.inc"),
            ("-gen-clang-attr-parsed-attr-list", "include/clang/Sema/AttrParsedAttrList.inc"),
            ("-gen-clang-attr-parsed-attr-kinds", "include/clang/Sema/AttrParsedAttrKinds.inc"),
            ("-gen-clang-attr-spelling-index", "include/clang/Sema/AttrSpellingListIndex.inc"),
            ("-gen-clang-attr-parsed-attr-impl", "include/clang/Sema/AttrParsedAttrImpl.inc"),
            ("-gen-clang-attr-parser-string-switches", "include/clang/Parse/AttrParserStringSwitches.inc"),
            ("-gen-clang-attr-subject-match-rules-parser-string-switches", "include/clang/Parse/AttrSubMatchRulesParserStringSwitches.inc"),
            ("-gen-clang-attr-pch-read", "include/clang/Serialization/AttrPCHRead.inc"),
            ("-gen-clang-attr-pch-write", "include/clang/Serialization/AttrPCHWrite.inc"),
        ]
    },
    {
        "name": "arm_neon",
        "td_file": "include/clang/Basic/arm_neon.td",
        "srcs": [],
        "tbl_outs": [
            ("-gen-arm-neon-sema", "include/clang/Basic/arm_neon.inc")
        ]
    },
    {
        "name": "statement_nodes",
        "td_file": "include/clang/Basic/StmtNodes.td",
        "srcs": [],
        "tbl_outs": [
            ("-gen-clang-stmt-nodes", "include/clang/AST/StmtNodes.inc"),
        ]
    },
    {
        "name": "decl_nodes",
        "td_file": "include/clang/Basic/DeclNodes.td",
        "srcs": [],
        "tbl_outs": [
            ("-gen-clang-decl-nodes", "include/clang/AST/DeclNodes.inc"),
        ]
    },
    {
        "name": "comment_nodes",
        "td_file": "include/clang/Basic/CommentNodes.td",
        "srcs": [],
        "tbl_outs": [
            ("-gen-clang-comment-nodes", "include/clang/AST/CommentNodes.inc"),
        ]
    },
    {
        "name": "comment_html_tags",
        "td_file": "include/clang/AST/CommentHTMLTags.td",
        "srcs": [],
        "tbl_outs": [
            ("-gen-clang-comment-html-tags", "include/clang/AST/CommentHTMLTags.inc"),
            ("-gen-clang-comment-html-tags-properties", "include/clang/AST/CommentHTMLTagsProperties.inc"),
        ]
    },
    {
        "name": "comment_html_named_character_references",
        "td_file": "include/clang/AST/CommentHTMLNamedCharacterReferences.td",
        "srcs": [],
        "tbl_outs": [
            ("-gen-clang-comment-html-named-character-references", "include/clang/AST/CommentHTMLNamedCharacterReferences.inc"),
        ]
    },
    {
        "name": "comment_commands",
        "td_file": "include/clang/AST/CommentCommands.td",
        "srcs": [],
        "tbl_outs": [
            ("-gen-clang-comment-command-info", "include/clang/AST/CommentCommandInfo.inc"),
            ("-gen-clang-comment-command-list", "include/clang/AST/CommentCommandList.inc"),
        ]
    },
    {
        "name": "static_analyzer_checkers",
        "td_file": "include/clang/StaticAnalyzer/Checkers/Checkers.td",
        "srcs": [
            "include/clang/StaticAnalyzer/Checkers/CheckerBase.td"
        ],
        "tbl_outs": [
            ("-gen-clang-sa-checkers", "include/clang/StaticAnalyzer/Checkers/Checkers.inc"),
        ]
    },

]

[
    gentbl (
        name = "tblgen_%s" % t["name"],
        tbl_outs = t["tbl_outs"],
        tblgen = ":tablegen",
        td_file = t["td_file"],
        td_srcs = t["srcs"]
    ) for t in tabelgen_targets
]

# Driver Options.inc
gentbl (
    name = "tblgen_driver_options_inc_gen",
    tbl_outs = [("-gen-opt-parser-defs", "include/clang/Driver/Options.inc")],
    tblgen = "@llvm//:llvm-tblgen",
    td_file = "include/clang/Driver/Options.td",
    td_srcs = [
        "@llvm//:include/llvm/Option/OptParser.td",
        "include/clang/Driver/CC1Options.td",
        "include/clang/Driver/CLCompatOptions.td",
    ],
)

# Generate version.inc

# Performs macro expansions on .def.in files
template_rule(
    name = "basic_version_inc_gen",
    src = "include/clang/Basic/Version.inc.in",
    out = "include/clang/Basic/Version.inc",
    substitutions = {
        "@CLANG_VERSION@": "5.0.0",
        "@CLANG_VERSION_MAJOR@": "5",
        "@CLANG_VERSION_MINOR@": "0",
        "@CLANG_VERSION_PATCHLEVEL@": "0",
    },
)

# Generate config.h

all_cmake_vars = {
    "BUG_REPORT_URL": "https://bugs.llvm.org",
    "CLANG_DEFAULT_OPENMP_RUNTIME": 'libomp',
    "CLANG_HAVE_LIBXML": "1",
    "CLANG_HAVE_RLIMITS": "1",
    "BACKEND_PACKAGE_STRING": "LLVM 5.0.0",
    "ENABLE_X86_RELAX_RELOCATIONS": "0",
}

cmake_vars = cmake_var_string(all_cmake_vars)

expand_cmake_vars(
    name = "config_h_gen",
    src = "include/clang/Config/config.h.cmake",
    cmake_vars = cmake_vars,
    dst = "include/clang/Config/config.h",
)

# Clang
cc_binary (
    name = "clang",
    srcs = glob([
        "tools/driver/*.cpp"
    ]),
    deps = [
        ":basic",
        ":code_gen",
        ":driver",
        ":frontend",
        ":frontend_tool",
        "@llvm//:analysis",
        "@llvm//:code_gen",
        "@llvm//:core",
        "@llvm//:ipo",
        "@llvm//:inst_combine",
        "@llvm//:instrumentation",
        "@llvm//:mc",
        "@llvm//:mc_parser",
        "@llvm//:objc_arc",
        "@llvm//:option",
        "@llvm//:scalar",
        "@llvm//:support",
        "@llvm//:transform_utils",
        "@llvm//:vectorize",

        "@llvm//:aarch64_code_gen",
        "@llvm//:arm_code_gen",
        "@llvm//:powerpc_code_gen",
        "@llvm//:x86_code_gen",

        "@llvm//:aarch64_asm_parser",
        "@llvm//:arm_asm_parser",
        "@llvm//:powerpc_asm_parser",
        "@llvm//:x86_asm_parser",
    ]
)

cc_library (
    name = "frontend",
    srcs = glob([
        "lib/Frontend/*.h",
        "lib/Frontend/*.cpp",
        "include/clang/StaticAnalyzer/Core/*.h",
    ]),
    hdrs = glob([
        "include/clang/Frontend/*.h",
        "include/clang/Frontend/*.def",
    ]) + [
        "include/clang/StaticAnalyzer/Core/Analyses.def",
    ],
    includes = ["include"],
    deps = [
        ":basic",
        ":serialization",
        ":parse",
        ":driver",
        "@llvm//:bit_reader",
        "@llvm//:option",
        "@llvm//:profile_data",
        "@llvm//:support",
    ]
)

cc_library (
    name = "rewrite_frontend",
    srcs = glob([
        "lib/Frontend/Rewrite/*.cpp",
        "lib/Frontend/Rewrite/*.h",
    ]),
    hdrs = glob([
        "include/Frontend/Rewrite/*.h",
    ]),
    deps = [
        ":ast",
        ":basic",
        ":edit",
        ":frontend",
        ":lex",
        ":rewrite",
        ":serialization",
    ]
)

cc_library (
    name = "rewrite",
    srcs = glob([
        "lib/Rewrite/*.h",
        "lib/Rewrite/*.cpp"
    ]),
    hdrs = glob([
        "include/clang/Rewrite/Core/*.h",
        "include/clang/Rewrite/Frontend/*.h",
    ]),
    deps = [
        ":lex",
        "@llvm//:support",
    ]
)

cc_library (
    name = "tooling_core",
    srcs = glob([
        "lib/Tooling/Core/*.h",
        "lib/Tooling/Core/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Tooling/Core/*.h"
    ]),
    deps = [
        ":ast",
        ":rewrite",
        "@llvm//:support",
    ]
)

cc_library (
    name = "edit",
    srcs = glob([
        "lib/Edit/*.h",
        "lib/Edit/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Edit/*.h",
    ]),
    deps = [
        ":ast",
        "@llvm//:support",
    ],
)

cc_library (
    name = "arc_migrate",
    srcs = glob([
        "lib/ARCMigrate/*.h",
        "lib/ARCMigrate/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/ARCMigrate/*.h",
    ]),
    deps = [
        ":static_analyzer_checkers",
        ":serialization",
        ":frontend",
        "@llvm//:support",
    ],
)

cc_library (
    name = "static_analyzer_checkers",
    srcs = glob([
        "lib/StaticAnalyzer/Checkers/*.h",
        "lib/StaticAnalyzer/Checkers/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/StaticAnalyzer/Checkers/*.h",
    ]),
    deps = [
        ":static_analyzer_core",
        ":ast_matchers",
        "@llvm//:support",
    ],
)

cc_library (
    name = "static_analyzer_frontend",
    srcs = glob([
        "lib/StaticAnalyzer/Frontend/*.h",
        "lib/StaticAnalyzer/Frontend/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/StaticAnalyzer/Frontend/*.h",
    ]),
    deps = [
        ":static_analyzer_checkers",
        ":frontend",
        "@llvm//:support",
    ],
)

cc_library (
    name = "static_analyzer_core",
    srcs = glob([
        "lib/StaticAnalyzer/Core/*.h",
        "lib/StaticAnalyzer/Core/*.cpp",
        
    ]) + [
        ":include/clang/StaticAnalyzer/Checkers/Checkers.inc",
    ],
    hdrs = glob([
        "include/clang/StaticAnalyzer/Core/*.h",
        "include/clang/StaticAnalyzer/Core/*.def",
        "include/clang/StaticAnalyzer/Core/PathSensitive/*.h",
        "include/clang/StaticAnalyzer/Core/PathSensitive/*.def",
        "include/clang/StaticAnalyzer/Core/BugReporter/*.h",
    ]),
    deps = [
        ":ast",
        ":rewrite",
        ":analysis",
        "@llvm//:support",
    ],
)

cc_library (
    name = "frontend_tool",
    srcs = glob([
        "lib/FrontendTool/*.h",
        "lib/FrontendTool/*.cpp",
    ]),
    hdrs = [
        "include/clang/FrontendTool/Utils.h",
    ],
    deps = [
        ":static_analyzer_frontend",
        ":arc_migrate",
        ":code_gen",
        ":rewrite_frontend",
        "@llvm//:support",
        "@llvm//:option",
    ],
    includes = ["include"]
)

cc_library (
    name = "index",
    srcs = glob([
        "lib/Index/*.h",
        "lib/Index/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Index/*.h",
    ]),
    deps = [
        ":serialization",
        ":toolingcore",
        ":format",
        "@llvm//:support",
        "@llvm//:core",
    ],
)

cc_library (
    name = "code_gen",
    srcs = glob([
        "lib/CodeGen/*.h",
        "lib/CodeGen/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/CodeGen/*.h",
    ]),
    deps = [
        ":ast",
        ":analysis",
        ":frontend",
        "@llvm//:analysis",
        "@llvm//:bit_reader",
        "@llvm//:bit_writer",
        "@llvm//:core",
        "@llvm//:coroutines",
        "@llvm//:coverage",
  		"@llvm//:ipo",
  		"@llvm//:ir_reader",
  		"@llvm//:inst_combine",
  		"@llvm//:instrumentation",
  		"@llvm//:lto",
  		"@llvm//:linker",
  		"@llvm//:mc",
  		"@llvm//:objc_arc",
  		"@llvm//:object",
  		"@llvm//:passes",
  		"@llvm//:profile_data",
  		"@llvm//:scalar",
  		"@llvm//:support",
  		"@llvm//:target",
  		"@llvm//:transform_utils",


    ],
)

cc_library (
    name = "serialization",
    srcs = glob([
        "lib/Serialization/*.h",
        "lib/Serialization/*.cpp",
        "include/clang/Frontend/*.h"
    ]),
    hdrs = glob([
        "include/clang/Serialization/*.h",
    ]) + [
        "include/clang/Serialization/AttrPCHRead.inc",
        "include/clang/Serialization/AttrPCHWrite.inc",
    ],
    deps = [
        ":sema",
        ":ast",
        ":lex",
        "@llvm//:bit_reader",
        "@llvm//:support",
    ],
)

cc_library (
    name = "parse",
    srcs = glob([
        "lib/Parse/*.h",
        "lib/Parse/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Parse/*.h",
    ]) + [
        "include/clang/Parse/AttrParserStringSwitches.inc",
        "include/clang/Parse/AttrSubMatchRulesParserStringSwitches.inc",
    ],
    deps = [
        ":sema",
        ":ast",
        "@llvm//:mc",
        "@llvm//:mc_parser",
        "@llvm//:support",
    ],
)

cc_library (
    name = "sema",
    srcs = glob([
        "lib/Sema/*.h",
        "lib/Sema/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Sema/*.h",
    ]) + [
        ":include/clang/Sema/AttrTemplateInstantiate.inc",
        ":include/clang/Sema/AttrParsedAttrList.inc",
        ":include/clang/Sema/AttrParsedAttrKinds.inc",
        ":include/clang/Sema/AttrSpellingListIndex.inc",
        ":include/clang/Sema/AttrParsedAttrImpl.inc",        
    ],
    deps = [
        ":ast",
        ":analysis",
        ":edit",
        "@llvm//:support",
    ],
)

cc_library (
    name = "format",
    srcs = glob([
        "lib/Format/*.h",
        "lib/Format/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Format/*.h",
    ]),
    deps = [
        ":tooling_core",
        "@llvm//:support",
    ],
)

cc_library (
    name = "ast_matchers",
    srcs = glob([
        "lib/ASTMatchers/*.h",
        "lib/ASTMatchers/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/ASTMatchers/*.h",
    ]),
    deps = [
        ":ast",
        "@llvm//:support",
    ],
)

cc_library (
    name = "dynamic_ast_matchers",
    srcs = glob([
        "lib/ASTMatchers/Dynamic/*.h",
        "lib/ASTMatchers/Dynamic/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/ASTMatchers/Dynamic/*.h",
    ]),
    deps = [
        ":ast_matchers",
        "@llvm//:support",
    ],
)

cc_library (
    name = "driver",
    srcs = glob([
        "lib/Driver/*.h",
        "lib/Driver/*.cpp",
        "lib/Driver/ToolChains/*.h",
        "lib/Driver/ToolChains/*.cpp",
        "lib/Driver/ToolChains/Arch/*.h",
        "lib/Driver/ToolChains/Arch/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Driver/*.h",
        "include/clang/Driver/*.def",
    ]) + [
        ":include/clang/Driver/Options.inc"
    ],
    deps = [
        ":ast_matchers",
        "@llvm//:binary_format",
        "@llvm//:option",
        "@llvm//:support",
    ],
    includes = [
        "lib/Driver"
    ]
)


cc_library (
    name = "analysis",
    srcs = glob([
        "lib/Analysis/*.h",
        "lib/Analysis/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/Analysis/*.h",
        "include/clang/Analysis/Analyses/*.h",
        "include/clang/Analysis/Analyses/*.def",
        "include/clang/Analysis/Support/*.h",
        "include/clang/Analysis/DomainSpecific/*.h",
        "include/clang/Analysis/FlowSensitive/*.h",
    ]),
    deps = [
        ":ast",
        "@llvm//:support",
    ],
)

cc_library (
    name = "ast",
    srcs = glob([
        "lib/AST/*.h",
        "lib/AST/*.cpp",
    ]),
    hdrs = glob([
        "include/clang/AST/*.h",
    ]) + [
        ":include/clang/AST/DeclNodes.inc",
        ":include/clang/AST/StmtNodes.inc",
        ":include/clang/AST/CommentCommandList.inc",
        ":include/clang/AST/CommentNodes.inc",
        ":include/clang/AST/Attrs.inc",
        ":include/clang/AST/AttrDump.inc",
        ":include/clang/AST/CommentHTMLTags.inc",
        ":include/clang/AST/AttrVisitor.inc",
        ":include/clang/AST/AttrImpl.inc",
        ":include/clang/AST/CommentHTMLNamedCharacterReferences.inc",
        ":include/clang/AST/CommentHTMLTagsProperties.inc",
        ":include/clang/AST/CommentCommandInfo.inc",
        ":include/clang/AST/TypeNodes.def",
        ":include/clang/AST/TypeLocNodes.def",
        ":include/clang/AST/BuiltinTypes.def",
        ":include/clang/AST/OperationKinds.def",
    ],
    includes = ["include"],
    deps = [
        ":basic",
        ":lex",
        "@llvm//:binary_format",
        "@llvm//:support",
    ]
)

cc_library (
    name = "basic",
    srcs = glob([
        "lib/Basic/*.cpp",
        "lib/Basic/*.h",
        "lib/Basic/*.inc",
    ]) + [
        "include/clang/Frontend/CodeGenOptions.h",
        ":include/clang/Config/config.h",
        "include/clang/AST/CommentDiagnostic.h",
    ] + ["include/clang/%s/%sDiagnostic.h" % (kind, kind) for kind in ["AST", "Driver", "Analysis", "Frontend", "Lex", "Parse", "Sema", "Serialization"]],
    hdrs = glob([
        "include/clang/Basic/*.h",
        "include/clang/Basic/*.def",
    ]) + [":include/clang/Basic/Diagnostic%sKinds.inc" % kind for kind in diagnosticKinds] + [
        ":include/clang/Basic/arm_neon.inc",
        ":include/clang/Basic/Version.inc",
        ":include/clang/Basic/AttrSubMatchRulesList.inc",
        ":include/clang/Basic/AttrHasAttributeImpl.inc",
        ":include/clang/Basic/DiagnosticGroups.inc",
        ":include/clang/Basic/AttrList.inc",
        ":include/clang/Frontend/CodeGenOptions.def", # This should be in srcs but it won't let us
    ],
    deps = [
        "@llvm//:support",
        "@llvm//:mc",
        "@llvm//:core",
    ],
    includes = ["include"],
)

cc_library (
    name = "lex",
    srcs = glob([
        "lib/Lex/*.cpp",
        "lib/Lex/*.h"
    ]),
    hdrs = glob([
        "include/clang/Lex/*.h",
    ]),
    deps = [
        "@llvm//:support",
        "@llvm//:target",
        ":basic",
    ],
    includes = ["include"],
)

