cc_library(
    name = "graph",
    hdrs = [
        "graph.h",
        "graphtpp.cpp",
    ],
    deps = [],
)

cc_binary(
    name = "client",
    srcs = ["client.cpp"],
    deps = [
        ":graph",
    ],
)

cc_test(
    name = "graph_test",
    srcs = ["graph_test.cpp"],
    deps = [
        ":graph",
        "//:catch",
    ],
)
