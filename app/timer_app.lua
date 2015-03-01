require("svcd")
require("cord")
d = require("ss")
d:init()
d:clear()
sh = require("stormsh")

local SERVICE = 0x4C0F
local ATTR = 0x4C0F

function ring_alarm()
   print("RING RING")
   for i=0,10 do
      d:num(8888)
      d:clear()
   end
   --+ buzzer + flash LEDs
end

local time = -1

storm.os.invokePeriodically(1*storm.os.SECOND,
			    function ()
			       if time > 0 then
				  d:num(time)
				  time = time - 1
			       elseif time == 0 then
				  ring_alarm()
				  time = -1
			       end
			    end)

SVCD.init("lame timer app",
	  function()
	     SVCD.add_service(SERVICE)
	     SVCD.add_attribute(SERVICE,
				ATTR,
				function (payload, source_ip, source_port)
				   cord.new(function ()
					       local arr = storm.array.fromstr(payload)
					       time = bit.lshift(arr:get(1), 8) + arr:get(2)
					       print("alarm set for "..time.." seconds from now")
					    end)
				end)
	  end)

sh.start()

cord.enter_loop()

print("time app ready to please")
