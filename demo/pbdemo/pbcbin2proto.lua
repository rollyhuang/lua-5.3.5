--- Output source to buffer Start ------
local oneIndent
local indentCnt = 0
local function IncreaseIndent() indentCnt = indentCnt + 1 end
local function DecreaseIndent() indentCnt = indentCnt - 1 end
local indentCache = { [0] = "" }
local function GetCurrentIndent()
	local indent = indentCache[indentCnt]
	if not indent then
		indent = string.rep(oneIndent, indentCnt)
		indentCache[indentCnt] = indent	
	end
	return indent
end

local outputTbl
local insert = table.insert
local function OutputLine(str)
	if str ~= "" then str = GetCurrentIndent() .. str end
	insert(outputTbl, str)
end
--- Output source to buffer End ------

-- no end of string, maybe '\0' in the end of string
local function NoEOS(str) return str:byte(-1) == 0 and str:sub(1, -2) or str end

local function MakeEnumArray(fieldTbl)
	local enums = {}
	local insert = table.insert
	local enumSize = fieldTbl.enum_size
	local enumString = fieldTbl.enum_string
	local enumID = fieldTbl.enum_id
	local idx = 0
	for index, name in ipairs(fieldTbl.enum_name) do
		local strs, ids = {}, {}
		for i = 1, enumSize[index], 1 do
			idx = idx + 1
			insert(strs, NoEOS(enumString[idx]))
			insert(ids, enumID[idx])
		end
		enums[NoEOS(name)] = {
			_strs_ = strs,
			_ids_ = ids,
		}
	end
	return enums
end

local function MakeMessageArray(fieldTbl)
	local messages = {}
	local insert = table.insert
	local messageSize = fieldTbl.message_size
	local messageField = fieldTbl.message_field
	local idx = 0
	for index, name in ipairs(fieldTbl.message_name) do
		local fields = {}
		for i = 1, messageSize[index], 1 do
			idx = idx + 1
			insert(fields, messageField[idx])
		end
		messages[NoEOS(name)] = {
			_fields_ = fields,
		}
	end
	return messages
end

local function ConstructMessageTree(messages, enums)
	local root = {} -- the root of the tree
	for name, msg in pairs(messages) do
		local current = root
		local prev = nil
		local key = nil
		for n in name:gmatch("%w*") do
			if not current[n] then current[n] = {} end
			prev = current
			key = n
			current = current[n]
		end
		assert(type(prev) == "table" and type(key) == "string")
		local value = prev[key]
		if value then
			for k, v in pairs(msg) do value[k] = v end
		else
			prev[key] = msg
		end
	end
	for name, enum in pairs(enums) do
		local current = root
		local prev = nil
		local key = nil
		for n in name:gmatch("%w*") do
			prev = current
			key = n
			current = current[n]
		end
		assert(type(prev) == "table")
		if prev._enums_ then
			assert(type(prev._enums_) == "table")
		else
			prev._enums_ = {}
		end
		prev._enums_[key] = enum
	end
	return root
end

local function FindPackageName(msgTree)
	local package = {}
	local newTree = nil
	local insert = table.insert
	local function OnlyOneField(tbl)
		local key, value = next(tbl)
		if key ~= nil and next(tbl, key) == nil then
			return key, value
		end
	end
	local function FindPackage(tree)
		local name, subTree = OnlyOneField(tree)
		if not name then return end
		insert(package, name)
		newTree = subTree
		return FindPackage(subTree)
	end
	FindPackage(msgTree)
	return table.concat(package, "."), newTree
end

local Label = {
	[0] = "optional",
	"required",
	"repeated"
}

local Type = {
	"double",
	"float",
	"int64",
	"uint64",
	"int32",
	"fixed64",
	"fixed32",
	"bool",
	"string",
	nil, -- TYPE_GROUP
	nil, -- TYPE_MESSAGE
	"bytes",
	"uint32",
	nil, -- TYPE_ENUM
	"sfixed32",
	"sfixed64",
	"sint32",
	"sint64",
}

local function OutputField(field)
	local typeName = Type[field.type] or NoEOS(field.type_name:match(".*%.(.*)$"))
	local label = Label[field.label or 0]
	assert(type(typeName) == "string" and type(label) == "string")
	local statement = label .. " " .. typeName .. " " .. NoEOS(field.name) .. " = " .. tostring(field.id) .. ";"
	OutputLine(statement)
end

local function OutputEnum(name, enum)
	OutputLine("enum " .. name .. " {")
	IncreaseIndent()
	for idx, id in ipairs(enum._ids_) do
		OutputLine(enum._strs_[idx] .. " = " .. tostring(id) .. ";")
	end
	DecreaseIndent()
	OutputLine("}")
end

local function OutputMessage(name, msg)
	OutputLine("message " .. name .. " {")
	IncreaseIndent()
	local fields = msg._fields_ or {}
	local enums = msg._enums_ or {}
	assert(type(fields) == "table" and type(enums) == "table")
	msg._fields_ = nil
	msg._enums_ = nil
	for eName, enum in pairs(enums) do OutputEnum(eName, enum) end
	for sName, subMsg in pairs(msg) do OutputMessage(sName, subMsg) end
	for _, field in ipairs(fields) do OutputField(field) end
	DecreaseIndent()
	OutputLine("}")
end

local function FieldTableToProtoSrc(fieldTbl, indent)
	outputTbl = {}
	oneIndent = indent or "  "
	local enums = MakeEnumArray(fieldTbl)
	local messages = MakeMessageArray(fieldTbl)
	local msgTree = ConstructMessageTree(messages, enums)	
	local pkgName, msgTree = FindPackageName(msgTree)
	OutputLine("syntax = \"proto2\";")
	OutputLine("package " .. pkgName .. ";")
	local enums = msgTree._enums_ or {}
	assert(type(enums) == "table")
	msgTree._enums_ = nil
	for name, enum in pairs(enums) do
		OutputLine("")
		OutputEnum(name, enum, "")
	end
	for name, msg in pairs(msgTree) do
		OutputLine("")
		OutputMessage(name, msg, "")
	end
	return table.concat(outputTbl, "\n")
end

--- Parse and Convert features ------

local varint = require("protobuf.varint")
local ParseVarint = varint.ParseVarint
local PBType = varint.PBType
local PBLabel = varint.PBLabel
local PBCFileConfig = require("protobuf.pbc_config")(PBLabel, PBType)

local fd = io.open(arg[1], "rb")
local msg = fd:read("a")
fd:close()
fd = nil
local fieldTbl = ParseVarint(msg, PBCFileConfig)
-- print(tostring(fieldTbl, 16))
local str = FieldTableToProtoSrc(fieldTbl)
print(str)

-- local tablesrc = require("common.tablesrc")
-- local src = tablesrc.tableToLuaSource(fieldTbl, "\t")
-- print("TableSrc:\n", src)
