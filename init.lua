require 'torch'
require 'dok'
require 'image'
require 'xlua'

local help_desc = [[
      Ardrone...
]]

ardrone = {}

-- load C lib
require 'libardrone'

function ardrone.new(pipe)
   local ar = libardrone.initArdrone(pipe)
   return ar
end

function ardrone.release(ar)
   libardrone.releaseArdrone(ar)
end

function ardrone.command(ar, fb, lr, rot, alt)
   libardrone.commandArdrone(ar, fb, lr, rot, alt)
end

function ardrone.takeoff(ar, takeoff)
   libardrone.takeoffArdrone(ar, takeoff)
end

function ardrone.emergency(ar, on)
   libardrone.emergencyArdrone(ar, on)
end