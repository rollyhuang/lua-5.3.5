local bcfx = require("bcfx")
local shader_type = bcfx.shader_type
local texture_format = bcfx.texture_format

local watch = require("watch")

local loader = {}

local pathPrefix = ""
function loader.SetPathPrefix(prefix)
	pathPrefix = prefix
end

function loader.LoadProgram(name)
	local vsPath = pathPrefix .. "Resource/Shader/" .. name .. ".vert.glsl"
	local fsPath = pathPrefix .. "Resource/Shader/" .. name .. ".frag.glsl"
	local vs = bcfx.createShader(io.readfile(vsPath), shader_type.Vertex)
	local fs = bcfx.createShader(io.readfile(fsPath), shader_type.Fragment)
	local prog = bcfx.createProgram(vs, fs)
	watch.onFileChanged(vsPath, function()
		bcfx.destroy(vs)
		vs = bcfx.createShader(io.readfile(vsPath), shader_type.Vertex)
		bcfx.updateProgram(prog, vs, fs)
		print_err("Vertex shader reload completed!")
	end)
	watch.onFileChanged(fsPath, function()
		bcfx.destroy(fs)
		fs = bcfx.createShader(io.readfile(fsPath), shader_type.Fragment)
		bcfx.updateProgram(prog, vs, fs)
		print_err("Fragment shader reload completed!")
	end)
	return prog
end

function loader.LoadTexture(filename)
	local filePath = pathPrefix .. "Resource/Texture/" .. filename;
	local fileData = bcfx.utils.ReadFile(filePath)
	local parseMB = bcfx.utils.ImageParse(fileData)
	return bcfx.createTexture(parseMB, texture_format.RGBA8)
end

return loader
