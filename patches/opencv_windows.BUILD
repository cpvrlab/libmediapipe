# Description:
#   OpenCV libraries for video/image processing on Windows

licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

config_setting(
    name = "opt_build",
    values = {"compilation_mode": "opt"},
)

config_setting(
    name = "dbg_build",
    values = {"compilation_mode": "dbg"},
)

cc_library(
    name = "opencv",
    srcs = select({
        ":opt_build": glob([
            "x64/vc16/lib/*470.lib",
            "x64/vc16/bin/*470.dll",
        ]),
        ":dbg_build": glob([
            "x64/vc16/lib/*470d.lib",
            "x64/vc16/bin/*470d.dll",
        ]),
    }),
    hdrs = glob(["include/opencv2/**/*.h*"]),
    includes = ["include/"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
