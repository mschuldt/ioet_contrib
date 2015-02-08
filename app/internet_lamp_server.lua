require "cord"
LED = require("led")
brd = LED:new("GP0")

local pin = storm.io.D4

print("\ninternet lamp.\n")
brd:flash(4)

ipaddr = storm.os.getipaddr()
ipaddrs = string.format("%02x%02x:%02x%02x:%02x%02x:%02x%02x::%02x%02x:%02x%02x:%02x%02x:%02x%02x",
			ipaddr[0],
			ipaddr[1],ipaddr[2],ipaddr[3],ipaddr[4],
			ipaddr[5],ipaddr[6],ipaddr[7],ipaddr[8],
			ipaddr[9],ipaddr[10],ipaddr[11],ipaddr[12],
			ipaddr[13],ipaddr[14],ipaddr[15])

print("ip addr", ipaddrs)
print("node id", storm.os.nodeid())
cport = 49160--49152

storm.io.set_mode(storm.io.OUTPUT, pin)

--local a='0',b='0',c='0'
local lamp_on_p = 0
local strobe_on_p = 0
local delays = {1000, 500, 250, 100, 50, 25, 10}
delay_index = 1
local n = #delays

storm.io.set_mode(storm.io.OUTPUT, storm.io.D4)


function lamp_on ()
   storm.io.set(storm.io.HIGH, pin)
end

function lamp_off ()
   storm.io.set(storm.io.LOW, pin)
end

function change_delay_g(milliseconds)
   delay_g = storm.os.MILLISECOND*milliseconds
   print(storm.os.SECOND/delay_g, "times per second (global)")
end

change_delay = change_delay_g

local delay_g

change_delay(delays[1])

local strobe
local count = 0

function x(n)
   delay_index = n
end

function strobe_on ()
   local delay = delay_g
   function change_delay_l(milliseconds)
      delay = storm.os.MILLISECOND*milliseconds
      print(storm.os.SECOND/delay, "times per second (local)")
   end
   change_delay = change_delay_l
   strobe = cord.new(function ()
			local c = count
			count = count + 1
			while true do
			   delay = delays[delay_index]*storm.os.MILLISECOND

			   if not delay then
			      delay = 500*storm.os.MILLISECOND
			   end
			   storm.io.set(storm.io.HIGH, pin)
			   cord.await(storm.os.invokeLater, delay)
			   storm.io.set(storm.io.LOW, pin)
			   cord.await(storm.os.invokeLater, delay)
			end
		     end)
   print("strobe handle = ", strobe)
   return change
end

function strobe_off ()
   print("strobe_off()")
   print("strobe = ", strobe)
   if strobe then
      cord.cancel(strobe)
   end
   change_delay = change_delay_g
   print("end\n")
end

-- create echo server as handler
server = function()
   ssock = storm.net.udpsocket(7,
			       function(payload, from, port)
				  print("$$$")
				  brd:flash(1)
				  if payload == "l1" then
				     lamp_on_p = 1
				     lamp_on()
				     print("light on")
				  elseif payload == "l0" then
				     lamp_on_p = 0
				     lamp_off()
				     print("light off")
				  elseif payload == "s1" then
				     print("strobe on")
				     if strobe_on_p ~= 1 then
					strobe_on()
					strobe_on_p = 1
				     end
				  elseif payload == "s0" then
				     print("strobe off")
				     if strobe_on_p ~= 0 then
					strobe_off()
					strobe_on_p = 0
					lamp_off()
				     end
				  elseif payload == "f" then
				     delay_index = (delay_index % n) + 1
				     --change_delay(delays[delay_index])
				     print("delay index = ", delay_index)
				  else
				     print("Err: invalid command:", payload)
				  end

				  print(storm.net.sendto(ssock, payload, from, cport))
				  brd:flash(1)
			       end)
end

server()			-- every node runs the echo server


-- enable a shell
sh = require "stormsh"
sh.start()
cord.enter_loop() -- start event/sleep loop
