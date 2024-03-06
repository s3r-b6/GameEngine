-- If using /, assume linux, if using \\, asume windows
local is_windows = package.config:sub(1, 1) == '\\'

if is_windows then
	vim.keymap.set('n', ",c", ":!./win32.ps1<CR>", { desc = "Compile" })
else
	vim.keymap.set('n', ",c", ":!./linux.sh<CR>", { desc = "Compile" })
end
