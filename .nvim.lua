vim.lsp.config("esp-clangd", {
	cmd = {
		vim.fn.expand("~/.espressif/v6.1/tools/esp-clangd/bin/clangd"),
		"--background-index",
		"--compile-commands-dir=build",
		"--query-driver=/Users/lucasvanderwielen/.espressif/v6.1/tools/esp-clang/**",
		"--log=verbose",
	},
	filetypes = { "c", "cpp", "objc", "objcpp", "cuda", "proto" },
	root_markers = { ".git", "compile_commands.json", "compile_flags.txt", "CMakeLists.txt" },
})
vim.lsp.enable("esp-clangd")

vim.lsp.enable("clangd", false)
