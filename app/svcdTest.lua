require "cord"
sh = require "stormsh"

-- start a coroutine that provides a REPL
sh.start()

require "svcd"
require "pprint"

storm.n.svcd_init("kavan", function() print("ready to go") end)
-- SVCD.add_service(12)
-- SVCD.add_attribute(12,1,function() print("someone wrote to attribute") end)

-- enter the main event loop. This puts the processor to sleep
-- in between events
cord.enter_loop()
