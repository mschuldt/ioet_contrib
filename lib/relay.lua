require("storm")

local Relay = {}

function Relay:new(relaypin)
    assert(relaypin and storm.io[relaypin], "invalid pin spec")
    obj = {pin = relaypin}	-- initialize the new object
    setmetatable(obj, self)	-- associate class methods
    self.__index = self
    storm.io.set_mode(storm.io.OUTPUT, storm.io[relaypin])
    return obj
end

function Relay:pin()
   return self.pin
end

function Relay:on()
   storm.io.set(storm.io.HIGH, storm.io[self.pin])
end

function Relay:off()
   storm.io.set(storm.io.LOW, storm.io[self.pin])
end

return Relay
