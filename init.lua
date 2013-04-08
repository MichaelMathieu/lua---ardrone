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
   return libardrone.initArdrone(pipe)
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

function ardrone.initvideo()
   libardrone.initVideoArdrone()
end

function ardrone.getframe(frame)
   if frame == nil then
      frame = torch.FloatTensor()
   end
   frame:resize(1, 180, 320)
   libardrone.getFrameArdrone(frame)
   return frame
end
