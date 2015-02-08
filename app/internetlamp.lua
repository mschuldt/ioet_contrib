
require "cord" -- scheduler / fiber library

print("Internet Lamp Client")

ipaddr = storm.os.getipaddr()
ipaddrs = string.format("%02x%02x:%02x%02x:%02x%02x:%02x%02x::%02x%02x:%02x%02x:%02x%02x:%02x%02x",
			ipaddr[0],
			ipaddr[1],ipaddr[2],ipaddr[3],ipaddr[4],
			ipaddr[5],ipaddr[6],ipaddr[7],ipaddr[8],	
			ipaddr[9],ipaddr[10],ipaddr[11],ipaddr[12],
			ipaddr[13],ipaddr[14],ipaddr[15])

print("ip addr", ipaddrs)
print("node id", storm.os.nodeid())
cport = 49160

-- client side
Button = require("button")
btn1 = Button:new("D11")		-- button 1 on starter shield
btn2 = Button:new("D10")	-- button 2 on starter shield
btn3 = Button:new("D9")	-- button 3 on starter shield
LED = require("led")
green = LED:new("D2")

local lamp_status = "l0"
local strobing = "s0"

-- create client socket
csock = storm.net.udpsocket(cport, 
			    function(payload, from, port)
			       print ("received: ", payload)
				green:flash(1)
				if payload == "l0" or payload == "l1" then
				    lamp_status = payload
				elseif payload == "s0" or payload == "s1" then
				    strobing = payload
				end		
				print("lamp_status: ",lamp_status)
			    end)


-- send echo on each button press

toggleLamp = function()
   local msg = "l0"
   if lamp_status == "l0"
   then msg = "l1"
   else msg = "l0"
   end
   print("send:", msg)
   -- send upd echo to link local all nodes multicast
   storm.net.sendto(csock, msg, "ff02::1", 7) 
end

toggleStrobing = function()
   local msg = "s0"
   if strobing == "s0"
   then msg = "s1"
   else msg = "s0"
   end
   print("send:", msg)
   -- send upd echo to link local all nodes multicast
   storm.net.sendto(csock, msg, "ff02::1", 7) 
end

-- button press runs client
btn1:whenever("RISING",function() 
		print("Toggling lamp")
		toggleLamp() 
		      end)

btn2:whenever("RISING",function() 
		print("Toggling strobing")
		toggleStrobing() 
		      end)

btn3:whenever("RISING",function() 
		print("Rotating freq")
		storm.net.sendto(csock, "f", "ff02::1", 7) 
		      end)






-- enable a shell
cord.enter_loop() -- start event/sleep loop
