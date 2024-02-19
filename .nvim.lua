-- If using /, assume linux, if using \\, asume windows
local is_windows = package.config:sub(1, 1) == '\\'

if is_windows then
	vim.keymap.set('n', "<F5>", ":!./win32.ps1<CR>")
else
	vim.keymap.set('n', "<F5>", ":!./linux.sh<CR>")
end
