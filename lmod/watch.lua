local libuv = require("libuv")
local fsevent = libuv.fsevent
local OK = libuv.err_code.OK
local CHANGE = fsevent.event_type.CHANGE
local RENAME = fsevent.event_type.RENAME
local RECURSIVE = fsevent.event_flag.RECURSIVE
local sys = libuv.sys

local reload = require("reload").reload

---@class watch:table
local watch = {}

local function error_msg(...)
	error(string.format(...))
end

local FilePath2Handle = {}
local function onFileChangedInternal(filePath, callback, onError)
	local lastTime = sys.hrTime()
	local function skip()
		local now = sys.hrTime()
		if now > lastTime and now - lastTime < 100000000 then
			return true
		end
		lastTime = now
		return false
	end
	local handle = fsevent.FsEvent()
	handle:startAsync(function(fileName, events, status)
		if status == OK then
			if events & CHANGE ~= 0 then
				if not skip() then
					callback(filePath)
				end
			elseif events & RENAME ~= 0 then
				error_msg("Error RENAME filePath: %s, fileName: %s", filePath, fileName)
			end
		else
			local handleStr = tostring(handle)
			onError(filePath)
			error_msg(
				"Watch filePath error: %s, fileName: %s, handle: %s, status: %s",
				filePath,
				fileName,
				handleStr,
				tostring(status)
			)
		end
	end, filePath, RECURSIVE)
	return handle
end

---@param filePath string
---@param callback fun(filePath:string):void
local function onFileChanged(filePath, callback)
	if FilePath2Handle[filePath] then return end
	FilePath2Handle[filePath] = onFileChangedInternal(filePath, callback, function(filePath)
		local handle = FilePath2Handle[filePath]
		if handle then
			handle:close()
			FilePath2Handle[filePath] = nil
		end
	end)
end

watch.onFileChanged = onFileChanged

---@param modName string
function watch.autoReload(modName)
	local filePath = package.searchpath(modName, package.path)
	if not filePath then
		error_msg("Error Could not find lua module: %s", modName)
	end
	onFileChanged(filePath, function(filePath)
		local ok, msg = reload(modName)
		if ok then
			printerr("reload succeed:", modName)
		else
			printerr("reload failed:", modName, msg)
		end
	end)
end

return watch
