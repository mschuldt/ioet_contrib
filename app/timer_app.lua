require("svcd")
require("cord")
sh = require("stormsh")

local SERVICE = 0x4C0F
local ATTR = 0x4C0F

function ring_alarm()
   print("***RING RING DING DING***")
end

SVCD.init("lame timer app",
	  function()
	     SVCD.add_service(SERVICE)
	     SVCD.add_attribute(SERVICE, 
				ATTR, 
				function (payload, source_ip, source_port)
				   cord.new (function ()
						local arr = storm.array.fromstr(payload)
						local time = bit.lshift(arr:get(1), 8) + arr:get(2)
						print("setting time for "..time.." seconds from now")
						storm.os.invokeLater(time*storm.os.SECOND, ring_alarm)
					     end)
				end)
	  end)

sh.start()

cord.enter_loop()
