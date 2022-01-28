load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_binary(
	name = "main",
	deps = [
	     ":core_tag",
	],
	srcs = ["main.cc"],
)

cc_library(
	name = "core_tag",
	srcs = ["core_tag.cc"],
	hdrs = ["core_tag.h"],
	deps = [
	     "@com_google_absl//absl/status:status",
	     "@com_google_absl//absl/status:statusor",
	],
)