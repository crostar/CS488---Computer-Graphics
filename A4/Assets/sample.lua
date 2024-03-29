-- Mr. Krabs :
-- https://en.wikipedia.org/wiki/Mr._Krabs
-- red (1.0, 0.63, 0.4)
red = gr.material({1.0, 0.2, 0.1}, {0.7, 0.14, 0.07}, 50.0)
-- red = gr.material({1.0, 0.63, 0.4}, {0.07, 0.042, 0.028}, 500.0)
green = gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0)
mouthBlack = gr.material({0.1, 0.1, 0.1}, {0, 0, 0}, 50.0)
beltBlack = gr.material({0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50.0)
lightBlue = gr.material({0.45, 0.65, 0.8}, {0.045, 0.065, 0.08}, 1.0)
darkBlue = gr.material({0.6, 0.65, 0.85}, {0.06, 0.065, 0.085}, 100.0)
gold = gr.material({1.0, 0.8, 0.0}, {0.8, 0.8, 0.8}, 50.0)
-- white = gr.material({0.8, 0.85, 0.6}, {0, 0, 0}, 1.0)
white = gr.material({0.9, 0.95, 0.9}, {0, 0, 0}, 1.0)

-- Create the top level root node named 'root'.
SceneRoot = gr.node('root')

function createAll()
  subroot = gr.node('subroot')

  t1, t2, t3 = createTorso();
  rEye = createEye('right');
  lEye = createEye('left');
  rArm = createArm('right')
  lArm = createArm('left')
  rLeg = createLeg('right')
  lLeg = createLeg('left')
  nose = createNose()
  mouth = createMouth()
  cl, cr = createCollars()
  belt = createBelt()

  subroot:add_child(t1)
  subroot:add_child(t2)
  subroot:add_child(t3)
  subroot:add_child(lEye)
  subroot:add_child(rEye)
  subroot:add_child(lArm)
  subroot:add_child(rArm)
  subroot:add_child(lLeg)
  subroot:add_child(rLeg)
  subroot:add_child(nose)
  subroot:add_child(mouth)
  subroot:add_child(cl)
  subroot:add_child(cr)
  subroot:add_child(belt)

  rEye:translate(0.3, 1.15, 0)
  lEye:translate(-0.3, 1.15, 0)

  rArm:rotate('z', -55)
  rArm:translate(0.65, 0.65, 0)
  lArm:rotate('z', 55)
  lArm:translate(-0.65, 0.65, 0)

  lLeg:translate(-0.4, -0.45, 0)
  rLeg:translate(0.4, -0.45, 0)

  nose:rotate('x', 30)
  nose:translate(0, 1.25, 0.20)

  mouth:rotate('x', 33)
  mouth:translate(0.033, 1.06, 0.28)

  return subroot;
end

function createBelt()
  belt = gr.sphere('belt')
  belt:scale(0.9, 0.3, 0.9)
  belt:translate(0, 0.2, 0)
  belt:set_material(beltBlack)

  button = gr.cube('button')
  button:scale(0.3, 0.9, 0.2)
  button:translate(0, 0.05, 0.9)
  button:set_material(gold)

  belt:add_child(button)
  return belt
end

function createCollars()
  cl = gr.cube('left collar')
  cr = gr.cube('right collar')
  cl:scale(0.2, 0.02, 0.2)
  cr:scale(0.2, 0.02, 0.2)
  cl:rotate('y', 25)
  cr:rotate('y', -25)
  cl:rotate('x', 25)
  cr:rotate('x', 25)

  cl:translate(-0.18, 0.7, 0.58)
  cr:translate(0.18, 0.7, 0.58)

  cl:set_material(lightBlue)
  cr:set_material(lightBlue)
  return cl, cr
end

function createTorso()
  t1 = gr.sphere('torso 1')
  t2 = gr.sphere('torso 2')
  t3 = gr.sphere('torso 3')
  t1:scale(0.95, 0.5, 0.95)
  t2:scale(0.83, 0.6, 0.83)
  t3:scale(0.65, 0.9, 0.65)
  -- t1:add_child(t2)
  -- t2:add_child(t3)
  t2:translate(0, 0.3, 0)
  t3:translate(0, 0.4, 0)
  t1:set_material(darkBlue)
  t2:set_material(lightBlue)
  t3:set_material(red)

  return t1, t2, t3
end

function createMouth()
  mouthJoint = gr.joint('mouth joint', {-30, 0, 30}, {0, 0, 0})
  mouth = gr.sphere('mouth')
  mouth:scale(0.37, 0.05, 0.32)
  mouth:set_material(mouthBlack)
  mouthJoint:add_child(mouth)

  return mouthJoint
end

function createNose()
  nosecolor = red

  noseJoint = gr.joint('nose joint', {-30, 0, 30}, {-30, 0, 30})
  lowernose = gr.sphere('lower nose')
  lowernose:scale(0.02, 0.08, 0.02)
  lowernose:translate(0, 0.08, 0)
  lowernose:set_material(nosecolor)

  lowerConnection = gr.sphere('middle connection')
  lowerConnection:scale(0.02, 0.02, 0.04)
  lowerConnection:rotate('x', 20)
  lowerConnection:translate(0, 0.14, 0.03)
  lowerConnection:set_material(nosecolor)

  middlenose = gr.sphere('middle nose')
  middlenose:scale(0.02, 0.08, 0.02)
  middlenose:translate(0, 0.2, 0.06)
  middlenose:set_material(nosecolor)

  upperConnection = gr.sphere('upper connection')
  upperConnection:scale(0.02, 0.02, 0.04)
  upperConnection:rotate('x', 20)
  upperConnection:translate(0, 0.26, 0.09)
  upperConnection:set_material(nosecolor)

  uppernose = gr.sphere('upper nose')
  uppernose:scale(0.02, 0.08, 0.02)
  uppernose:translate(0, 0.32, 0.12)
  uppernose:set_material(nosecolor)

  noseJoint:add_child(lowernose)
  noseJoint:add_child(lowerConnection)
  noseJoint:add_child(middlenose)
  noseJoint:add_child(upperConnection)
  noseJoint:add_child(uppernose)

  return noseJoint
end

function createEye(name)
  length = 0.9
  eyeJoint = gr.joint(name .. ' eye joint', {-30, 0, 30}, {-30, 0, 30})
  eye = gr.sphere(name .. ' eye')
  eye:scale(0.1, length, 0.1)
  eye:translate(0, length, 0)
  eye:set_material(white)
  eyeJoint:add_child(eye)

  eyeLid = createEyeLid(name)
  eyeLid:translate(0, 2 * length, 0)
  eyeJoint:add_child(eyeLid)

  eyeBall = createEyeball(name)
  eyeBall:translate(0, 0.8, 0.01)
  eyeJoint:add_child(eyeBall)

  return eyeJoint
end

function createEyeLid(name)
  eyeLidJoint = gr.joint(name .. ' eye lid', {0, 0, 0}, {0, 0, 0})
  eyeLid = gr.sphere(name .. ' eye lid')
  eyeLid:scale(0.12, 0.7, 0.12)
  eyeLid:translate(0, -0.5, 0)
  eyeLid:set_material(red)
  eyeLidJoint:add_child(eyeLid)

  eyeBrow = createEyebrow(name)
  eyeBrow:translate(0, 0.1, 0)
  eyeLidJoint:add_child(eyeBrow)

  lowerEyeLid = createLowerEyeLid(name)
  lowerEyeLid:translate(0, -0.9, 0)
  eyeLidJoint:add_child(lowerEyeLid)

  return eyeLidJoint
end

function createLowerEyeLid(name)
  lowerEyeLidJoint = gr.joint(name .. ' lower eye lid', {0, 0, 0}, {0, 0, 0})
  lowerEyeLid = gr.sphere(name .. ' lower eye lid')
  lowerEyeLid:scale(0.14, 0.02, 0.14)
  -- lowerEyeLid:translate(0, -0.5, 0)
  lowerEyeLid:set_material(red)
  lowerEyeLidJoint:add_child(lowerEyeLid)

  return lowerEyeLidJoint
end

function createEyebrow(name)
  eyebrowJoint = gr.joint(name .. ' eyebrow', {-30, 0, 30}, {-30, 0, 30})
  eyeBrowM = gr.sphere(name .. ' eyebrow M')
  eyeBrowL = gr.sphere(name .. ' eyebrow L')
  eyeBrowR = gr.sphere(name .. ' eyebrow R')

  for i, eyeBrow in ipairs({eyeBrowM, eyeBrowL, eyeBrowR}) do
    eyeBrow:scale(0.02, 0.1, 0.02)
    eyeBrow:translate(0, 0.1, 0)
    eyeBrow:set_material(mouthBlack)
    eyebrowJoint:add_child(eyeBrow)
  end
  eyeBrowL:rotate('z', 30)
  eyeBrowR:rotate('z', -30)
  return eyebrowJoint
end

function createEyeball(name)
  eyeballJoint = gr.joint(name .. ' eye ball', {-10, 0, 10}, {-10, 0, 10})
  eyeball = gr.sphere(name .. ' eye ball')
  eyeball:scale(0.065, 0.1, 0.065)
  eyeball:translate(0, 0, 0.08)
  eyeball:set_material(beltBlack)
  eyeballJoint:add_child(eyeball)

  return eyeballJoint
end

function createArm(name)
  length = 0.5
  upperArmJoint = gr.joint(name .. ' upper arm joint', {-10, 0, 10}, {-10, 0, 10})
  upperArm = gr.sphere(name .. ' upper arm')
  upperArm:scale(0.04, length, 0.04)
  upperArm:translate(0, length, 0)
  upperArm:set_material(red)
  upperArmJoint:add_child(upperArm)

  lowerArm = createLowerArm(name)
  if name == 'left' then
    lowerArm:rotate('x', -150)
  else
    lowerArm:rotate('x', 150)
  end
  lowerArm:translate(0, 2 * length, 0)
  upperArmJoint:add_child(lowerArm)

  sleeve = gr.sphere(name .. ' sleeve')
  sleeve:scale(0.08, 0.1, 0.08)
  sleeve:translate(0, 0.04, 0)
  sleeve:set_material(lightBlue)

  upperArmJoint:add_child(sleeve)

  upperArmJoint:rotate('y', 90)
  return upperArmJoint
end

function createLowerArm(name)
  lowerArmLength = 0.4
  lowerArmJoint = gr.joint(name .. ' lower arm joint', {-10, 0, 10}, {-10, 0, 10})
  lowerArm = gr.sphere(name .. ' lower arm')
  lowerArm:scale(0.04, lowerArmLength, 0.04)
  lowerArm:translate(0, lowerArmLength, 0)
  lowerArm:set_material(red)
  lowerArmJoint:add_child(lowerArm)

  hand = createHand(name)
  if name == 'left' then
    hand:rotate('z', 90)
  else
    hand:rotate('z', -90)
  end
  hand:translate(0, 2 * lowerArmLength, 0)
  lowerArmJoint:add_child(hand)

  lowerArmJoint:rotate('y', 90)
  return lowerArmJoint
end

function createHand(name)
  handJoint = gr.joint(name .. ' hand joint', {-10, 0, 10}, {-10, 0, 10})
  hand = gr.sphere(name .. ' hand')
  hand:scale(0.3, 0.2, 0.17)
  hand:rotate('y', -90)
  hand:set_material(red)
  handJoint:add_child(hand)

  thumb = createThumb(name)
  pinky = createPinky(name)
  if name == "left" then
    thumb:rotate('z', 10)
    pinky:rotate('z', -10)
    thumb:translate(-0.18, -0.02, 0)
    pinky:translate(0.18, -0.02, 0)
  else
    thumb:rotate('z', -10)
    pinky:rotate('z', 10)
    thumb:translate(0.18, 0.02, 0)
    pinky:translate(-0.18, -0.02, 0)
  end
  thumb:rotate('y', 90)
  handJoint:add_child(thumb)
  handJoint:add_child(pinky)

  handJoint:rotate('y', 90)
  pinky:rotate('y', 90)
  return handJoint
end

function createThumb(name)
  thumbJoint = gr.joint(name .. ' thumb joint', {-10, 0, 10}, {-10, 0, 10})
  thumb = gr.sphere(name .. ' thumb')
  thumb:scale(0.35, 0.6, 0.2)
  thumb:rotate('y', -90)
  thumb:translate(0, 0.5, 0)
  thumb:set_material(red)
  thumbJoint:add_child(thumb)

  thumbJoint:rotate('y', 90)
  return thumbJoint
end

function createPinky(name)
  pinkyJoint = gr.joint(name .. ' pinky joint', {-10, 0, 10}, {-10, 0, 10})
  pinky = gr.sphere(name .. ' pinky')
  pinky:scale(0.15, 0.3, 0.17)
  pinky:rotate('y', -90)
  pinky:translate(0, 0.24, 0)
  pinky:set_material(red)
  pinkyJoint:add_child(pinky)

  pinkyJoint:rotate('y', 90)
  return pinkyJoint
end

function createLeg(name)
  upperLegJoint = gr.joint(name .. ' upper leg joint', {-40, 0, 40}, {-40, 0, 40})
  upperLeg = gr.sphere(name .. ' upper leg')
  upperLeg:scale(0.1, 0.05, 0.1)
  -- upperLeg:translate(0, -0.1, 0)
  upperLeg:set_material(darkBlue)
  upperLegJoint:add_child(upperLeg)

  lowerLeg = gr.sphere(name .. ' lower leg')
  lowerLeg:scale(0.05, 0.15, 0.05)
  lowerLeg:translate(0, -0.12, 0)
  lowerLeg:set_material(red)
  upperLegJoint:add_child(lowerLeg)
  if name == 'left' then
    lowerLeg:rotate('z', -10)
  else
    lowerLeg:rotate('z', 10)
  end

  return upperLegJoint
end

crab = createAll()
crab:rotate('x', -10)
crab:rotate('y', 45)
crab:translate(1,2.7,4)

-- Add the cubeMesh GeometryNode to the child list of SceneRoot.
SceneRoot:add_child(crab)

sand = gr.material({0.8, 0.9, 0.7}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

plane = gr.mesh('plane', 'plane.obj' )
SceneRoot:add_child(plane)
plane:set_material(sand)
plane:scale(30, 30, 30)

-- #############################################
-- Read in the cow model from a separate file.
-- #############################################

cow_poly = gr.mesh('cow', 'cow.obj')
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate(0.0, 3.637, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, -1.0, 0.0)

-- Use the instanced cow model to place some actual cows in the scene.
-- For convenience, do this in a loop.

cow_number = 1

for _, pt in pairs({
		      {{1,1.3,4}, -45},
		      {{3,1.3,-3}, -40},
		      {{-2.5,1.3,-3}, -45},
		      {{-5.5,1.3,0}, -45}}) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   SceneRoot:add_child(cow_instance)
   cow_instance:add_child(cow_poly)
   cow_instance:scale(1.4, 1.4, 1.4)
   cow_instance:rotate('Y', pt[2])
   cow_instance:translate(table.unpack(pt[1]))

   cow_number = cow_number + 1
end

SceneRoot:rotate('X', -10)
SceneRoot:translate(0.0, -4.8, 0.0)

-- Return the root with all of it's childern.  The SceneNode A3::m_SceneRoot will be set
-- equal to the return value from this Lua script.
white_light_out = gr.light({20.0, 20.0, 20.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
white_light_in = gr.light({-40.0, 100.0, -150.0}, {0.3, 0.3, 0.3}, {1, 0, 0})
white_light_test = gr.light({0, 0, 20}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- magenta_light = gr.light({4.0, 10.0, 5.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(SceneRoot, 'sample.png', 512, 512,
	  {0, 5, 20}, {0, 0, -25}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light_in, white_light_out})
