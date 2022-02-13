local libuv = require("libuv")

--[[
** {======================================================
** Message Pack and Unpack
** =======================================================
--]]

local packetPBCode = { 0xA, 0x5F, 0xA, 0x12, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x2F, 0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x2E, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x12, 0xF, 0x5F, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x4D, 0x61, 0x6E, 0x61, 0x67, 0x65, 0x72, 0x5F, 0x22, 0x30, 0xA, 0x6, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x12, 0x12, 0xA, 0x4, 0x6E, 0x61, 0x6D, 0x65, 0x18, 0x1, 0x20, 0x1, 0x28, 0x9, 0x52, 0x4, 0x6E, 0x61, 0x6D, 0x65, 0x12, 0x12, 0xA, 0x4, 0x64, 0x61, 0x74, 0x61, 0x18, 0x2, 0x20, 0x1, 0x28, 0xC, 0x52, 0x4, 0x64, 0x61, 0x74, 0x61, 0x62, 0x6, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x33 }
local packetPB = string.char(function(idx) return packetPBCode[idx] end)

local replPBCode = { 0xA, 0xBD, 0x1, 0xA, 0x10, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x2F, 0x72, 0x65, 0x70, 0x6C, 0x2E, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x12, 0x6, 0x5F, 0x52, 0x45, 0x50, 0x4C, 0x5F, 0x22, 0x2C, 0xA, 0x4, 0x52, 0x65, 0x61, 0x64, 0x12, 0x12, 0xA, 0x4, 0x63, 0x6F, 0x64, 0x65, 0x18, 0x1, 0x20, 0x1, 0x28, 0x9, 0x52, 0x4, 0x63, 0x6F, 0x64, 0x65, 0x12, 0x10, 0xA, 0x3, 0x65, 0x6F, 0x66, 0x18, 0x2, 0x20, 0x1, 0x28, 0x8, 0x52, 0x3, 0x65, 0x6F, 0x66, 0x22, 0x6B, 0xA, 0x5, 0x50, 0x72, 0x69, 0x6E, 0x74, 0x12, 0x16, 0xA, 0x6, 0x70, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x18, 0x1, 0x20, 0x1, 0x28, 0x9, 0x52, 0x6, 0x70, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x12, 0x16, 0xA, 0x6, 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x18, 0x2, 0x20, 0x1, 0x28, 0x9, 0x52, 0x6, 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x12, 0x18, 0xA, 0x7, 0x68, 0x69, 0x73, 0x74, 0x6F, 0x72, 0x79, 0x18, 0x3, 0x20, 0x1, 0x28, 0x9, 0x52, 0x7, 0x68, 0x69, 0x73, 0x74, 0x6F, 0x72, 0x79, 0x12, 0x18, 0xA, 0x7, 0x72, 0x75, 0x6E, 0x6E, 0x69, 0x6E, 0x67, 0x18, 0x4, 0x20, 0x1, 0x28, 0x8, 0x52, 0x7, 0x72, 0x75, 0x6E, 0x6E, 0x69, 0x6E, 0x67, 0x62, 0x6, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x33 }
local replPB = string.char(function(idx) return replPBCode[idx] end)

local pbc = require("protobuf")
pbc.register(packetPB)
pbc.register(replPB)

---@class REPLPacket
local REPLPacket = {
	---@type PacketManager
	packetManager = nil
}
REPLPacket.__index = REPLPacket

function REPLPacket:packMessage(messageType, dataTable)
	local serializedData = pbc.encode(messageType, dataTable)
	local packet = pbc.encode("_PacketManager_.Packet", {
		name = messageType,
		data = serializedData
	})
	local packetSerial = self.packetManager:packPacket(packet)
	return packetSerial
end

function REPLPacket:packReadMessage(codeStr, eof)
	return self:packMessage("_REPL_.Read", {
		code = codeStr,
		eof = eof,
	})
end

function REPLPacket:packPrintMessage(running, prompt, history, output)
	return self:packMessage("_REPL_.Print", {
		running = running,
		prompt = prompt,
		history = history,
		output = output,
	})
end

function REPLPacket:addPackData(data)
	self.packetManager:addPackData(data)
end

function REPLPacket:getPacket()
	local status, str = self.packetManager:getPacket()
	if status ~= libuv.packet_status.OK then return end
	local packetTable = pbc.decode("_PacketManager_.Packet", str)
	return packetTable.name, pbc.decode(packetTable.name, packetTable.data)
end

---@return REPLPacket
local function REPLPacketHandler()
	local handler = {
		packetManager = libuv.PacketManager()
	}
	return setmetatable(handler, REPLPacket)
end

-- }======================================================

---@class repl:table
local repl = {}

---@overload fun():void
---@overload fun(callback:REPLEvalSignature):void
---@param callback REPLEvalSignature
function repl.startAsync(callback)
	libuv.replStartAsync(callback)
end

---@type REPLEvalSignature
function repl.evalDefault(codeStr, eof)
	return libuv.replDefault(codeStr, eof)
end

local firstLine = true
local codeCache
local function compileSourceCode(codeStr)
	local code = codeStr
	if firstLine then
		if codeStr:byte() == ("="):byte() then
			code = "return " .. codeStr:sub(2)
		else
			local loader = load("return " .. codeStr)
			if loader then
				return loader, codeStr
			end
		end
	else
		code = codeCache .. "\n" .. codeStr
	end
	local loader, msg = load(code)
	return loader or msg, code
end
local function inComplete(loader)
	return type(loader) == "string" and loader:sub(-5, -1) == "<eof>"
end
---@type REPLEvalSignature
function repl.evalDefaultLua(codeStr, eof)
	local running = true
	local history
	if eof then
		if firstLine then
			running = false
			print()
		else
			firstLine = true
			history = codeCache
			print()
		end
	else
		local loader, code = compileSourceCode(codeStr)
		if inComplete(loader) then
			firstLine = false
			codeCache = code
		else
			firstLine = true
			history = code
			local msg
			if type(loader) == "function" then
				local ret, em = pcall(function()
					print(loader())
				end)
				if not ret then
					msg = em
				end
			else
				msg = loader
			end
			if msg then
				print("lua: " .. msg)
			end
		end
	end
	local prompt = firstLine and "> " or ">> "
	return running, prompt, history
end

local tcp = libuv.tcp
local network = libuv.network
local err_code = libuv.err_code
local OK = err_code.OK
local EOF = err_code.EOF

local errName = libuv.errName
local strError = libuv.strError
local function printError(msg, status)
	printerr(msg, status, errName(status), strError(status))
end

---@alias REPLRemoteEvalSignature fun(codeStr:string | nil, eof:boolean):boolean, string, string | nil, string | nil @running, prompt, history, output

---@overload fun():void
---@overload fun(ip:string):void
---@overload fun(ip:string, port:integer):void
---@overload fun(ip:string, port:integer, callback:REPLEvalSignature):void
---@param ip string
---@param port integer
---@param callback REPLEvalSignature
function repl.serverStartAsync(ip, port, callback)
	callback = callback or repl.serverEvalDefault
	local tcpSocket = tcp.Tcp()
	local sockAddr = network.SockAddr()
	sockAddr:ip4Addr(ip or "0.0.0.0", port or 1999)
	tcpSocket:bind(sockAddr)
	tcpSocket:listenAsync(128, function(status)
		if status ~= OK then
			printError("TCP listen error:", status)
			return
		end
		local tcpConnection = tcp.Tcp()
		status = tcpSocket:accept(tcpConnection)
		if status ~= OK then
			printError("TCP accept error:", status)
			return
		end
		local fd = tcpConnection:fileno()
		print("New REPL connection start:", fd)
		local packetHandler = REPLPacketHandler()
		tcpConnection:readStartAsync(function(nread, str)
			if nread < 0 then
				if nread == EOF then
					print("REPL connection end:", fd)
				else
					printError("REPL connection read error:", nread)
				end
				tcpConnection:close()
				return
			end
			packetHandler:addPackData(str)
			while true do
				local type, readTable = packetHandler:getPacket()
				if not type then break end-- has no complete packet
				assert(type == "_REPL_.Read")
				local running, prompt, history, output = callback(readTable.code, readTable.eof)
				local msg = packetHandler:packPrintMessage(running, prompt, history, output)
				tcpConnection:writeAsync(msg, function(status)
					if status ~= OK then
						printError("REPL Server TCP Write error:", status)
					end
				end)
			end
		end)
	end)
end

---@param evalFunc REPLEvalSignature
---@return REPLRemoteEvalSignature
function repl.makeServerEval(evalFunc)
	return function(codeStr, eof)
		util.printBuffer()
		local running, prompt, history = evalFunc(codeStr, eof)
		local output = util.printFinish()
		return running, prompt, history, output
	end
end

---@type REPLRemoteEvalSignature
repl.serverEvalDefault = repl.makeServerEval(libuv.replDefault)

---@overload fun():void
---@overload fun(serverIP:string):void
---@overload fun(serverIP:string, serverPort:integer):void
---@param serverIP string
---@param serverPort integer
function repl.clientStart(serverIP, serverPort)
	local tcpClient = tcp.Tcp()
	local sockAddr = network.SockAddr()
	sockAddr:ip4Addr(serverIP or "0.0.0.0", serverPort or 1999)
	tcpClient:connectAsync(sockAddr, function(status)
		if status < 0 then
			printError("REPL TCP Connect error:", status)
			return
		end
		local packetHandler = REPLPacketHandler()
		local function readTerminalCodeAndSend(prompt)
			local codeStr = libuv.replRead(prompt or "> ")
			local msg = packetHandler:packReadMessage(codeStr, codeStr == nil)
			tcpClient:writeAsync(msg, function(status)
				if status ~= OK then
					printError("REPL TCP Write error:", status)
				end
			end)
		end
		tcpClient:readStartAsync(function(nread, str)
			if nread < 0 then
				printError("REPL TCP Read error:", nread)
			else
				packetHandler:addPackData(str)
				local prompt
				while true do
					local type, printTable = packetHandler:getPacket()
					if not type then break end-- has no complete packet
					assert(type == "_REPL_.Print")
					libuv.replHistory(printTable.history)
					prompt = printTable.prompt
					io.stdout:write(printTable.output)
					if not printTable.running then
						print("REPL Client End.")
						tcpClient:close()
						return
					end
				end
				readTerminalCodeAndSend(prompt)
			end
		end)
		print("REPL Client Start...")
		readTerminalCodeAndSend()
	end)
end

---@overload fun():void
---@overload fun(serverIP:string):void
---@overload fun(serverIP:string, serverPort:integer):void
---@param serverIP string
---@param serverPort integer
function repl.clientRun(serverIP, serverPort)
	libuv.setLoop()
	repl.clientStart(serverIP, serverPort)
	libuv.run()
end

return repl
