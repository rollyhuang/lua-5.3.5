local bcfx = require("bcfx")
local shader_type = bcfx.shader_type
local texture_format = bcfx.texture_format
local image = bcfx.image

local libuv = require("libuv")
local mbio = libuv.mbio

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
		printerr("Vertex shader reload completed!")
	end)
	watch.onFileChanged(fsPath, function()
		bcfx.destroy(fs)
		fs = bcfx.createShader(io.readfile(fsPath), shader_type.Fragment)
		bcfx.updateProgram(prog, vs, fs)
		printerr("Fragment shader reload completed!")
	end)
	return prog
end

function loader.LoadTexture(filename)
	local filePath = pathPrefix .. "Resource/Texture/" .. filename;
	local fileData = mbio.readFile(filePath)
	local parseMB, width, height = image.imageDecode(fileData, texture_format.RGBA8)
	return bcfx.createTexture2D(texture_format.RGBA8, parseMB, width, height)
end

function loader.loadMesh(fileName)
	local filePath = pathPrefix .. "Resource/Mesh/" .. fileName;
	local fileData = mbio.readFile(filePath)
	local meshs, materialLibraryNames = bcfx.mesh.meshParse(fileData)
	return meshs[1].vertex, meshs[1].index
end

local FaceNameList = {
	"right",  -- +x
	"left",   -- -x
	"bottom", -- +y
	"top",    -- -y
	"back",   -- +z
	"front",  -- -z
}
--[[
     top
left back right front
    bottom
OpenGL CubeMapSpace: +x to right, +y to bottom, +z to back, we look at -z, left handedness
the texture we save should be the version seen from the outside
When sample CubeMap, we should convert position from WorldSpace to CubeMapSpace, and it will sample inside the cube
]]
function loader.LoadTextureCubeMap(folderName)
	local filePrefix = (pathPrefix .. "Resource/Texture") / folderName
	local fileDataMB = {}
	local width
	for idx, faceName in ipairs(FaceNameList) do
		local filePath = filePrefix / (faceName .. ".jpg")
		local fileData, err, str = mbio.readFile(filePath)
		if not fileData then
			printerr("LoadTextureCubeMap error: ", err, str, filePath)
		end
		local parseMB, w, h = image.imageDecode(fileData, texture_format.RGB8)
		fileDataMB[idx] = parseMB
		assert(w == h)
		if not width then
			width = w
		else
			assert(width == w)
		end
	end
	return bcfx.createTextureCubeMap(texture_format.RGB8, fileDataMB, width, false)
end

function loader.LoadTexture2DArray(fileNameList)
	local fileDataMBList = {}
	local filePrefix = pathPrefix .. "Resource/Texture"
	local width, height
	for idx, fileName in ipairs(fileNameList) do
		local filePath = filePrefix / fileName
		local fileData, err, str = mbio.readFile(filePath)
		if not fileData then
			printerr("LoadTextureCubeMap error: ", err, str, filePath)
		else
			local parseMB, w, h = image.imageDecode(fileData, texture_format.RGB8)
			fileDataMBList[idx] = parseMB
			print(parseMB, idx)
			if not width and not height then
				width, height = w, h
			else
				assert(width == w and height == h)
			end	
		end
	end
	return bcfx.createTexture2DArray(texture_format.RGB8, fileDataMBList, width, height, false)
end

return loader
