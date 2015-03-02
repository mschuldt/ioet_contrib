require("svcd") 
require("cord")
sh = require("stormsh")


SVCD.init("boss", function()



SVCD.add_service(0x4C0F)
SVCD.add_attribute(0x4C0F,0x4C0F, function (payload, source_ip, source_port) 

				  cord.new (function ()
					       print("payload = " .. payload)
					       --local args = storm.array.fromstr(storm.mp.unpack(payload))
					       --print("storm.array.fromstr(payload) " .. storm.array.fromstr(payload))				
	                                        

					       local j = storm.array.fromstr(payload)
					       print("j:get(1) = " .. j:get(1))	
						print("get_as = " .. j:get_as(storm.array.UINT16, 0))
			                        print("-->" .. bit.lshift(j:get(1), 8) + j:get(2))
					       --for k,v in pairs(j) do print(k,v) end 
					       --print("args" .. args)
					       end) end) 
end) 
--[[
SVCD.add_service("service2")
SVCD.add_attribute("service2","temp", function() print("available attribute") end) 

SVCD.add_service("service3")
SVCD.add_attribute("service3","temp", function() print("available attribute") end) 
 
SVCD.add_service("service4")
SVCD.add_attribute("service4","temp", function() print("available attribute") end) 
 
SVCD.add_service("service5")
SVCD.add_attribute("service5","temp", function() print("available attribute") end) 
--]]  
 
--[[
storm.os.invokePeriodically(
  5*storm.os.SECOND, 
  function() print("subscribing") SVCD.subscribe("fe80::212:6d02:0:3039", 12, 1, function() print("notified") end) end)
]]--
sh.start()

cord.enter_loop()
