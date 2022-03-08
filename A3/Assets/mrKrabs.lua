-- Mr. Krabs :
-- https://en.wikipedia.org/wiki/Mr._Krabs

red = gr.material({0.9, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50.0)
green = gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0)
black = gr.material({0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50.0)
lightBlue = gr.material({0.6, 0.8, 0.8}, {0, 0, 0}, 0.0)
darkBlue = gr.material({0, 0.5, 1.0}, {0, 0, 0}, 0.0)

-- Create the top level root node named 'root'.
rootNode = gr.node('root')

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

  mouth:rotate('x', 45)
  mouth:translate(0, 0.95, 0.38)
  return subroot;
end

function createTorso()
  t1 = gr.mesh('sphere', 'torso 1')
  t2 = gr.mesh('sphere', 'torso 2')
  t3 = gr.mesh('sphere', 'torso 3')
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
  mouthJoint = gr.joint('mouth joint', {0, 0, 0}, {0, 0, 0})
  mouth = gr.mesh('sphere', 'mouth')
  mouth:scale(0.4, 0.05, 0.3)
  mouth:set_material(black)
  mouthJoint:add_child(mouth)

  return mouthJoint
end

function createNose()
  noseJoint = gr.joint('nose joint', {-30, 0, 30}, {-30, 0, 30})
  lowernose = gr.mesh('sphere', 'lower nose')
  lowernose:scale(0.02, 0.08, 0.02)
  lowernose:translate(0, 0.08, 0)
  lowernose:set_material(black)

  lowerConnection = gr.mesh('sphere', 'middle connection')
  lowerConnection:scale(0.02, 0.02, 0.04)
  lowerConnection:rotate('x', 20)
  lowerConnection:translate(0, 0.14, 0.03)
  lowerConnection:set_material(black)

  middlenose = gr.mesh('sphere', 'middle nose')
  middlenose:scale(0.02, 0.08, 0.02)
  middlenose:translate(0, 0.2, 0.06)
  middlenose:set_material(black)

  upperConnection = gr.mesh('sphere', 'upper connection')
  upperConnection:scale(0.02, 0.02, 0.04)
  upperConnection:rotate('x', 20)
  upperConnection:translate(0, 0.26, 0.09)
  upperConnection:set_material(black)

  uppernose = gr.mesh('sphere', 'upper nose')
  uppernose:scale(0.02, 0.08, 0.02)
  uppernose:translate(0, 0.32, 0.12)
  uppernose:set_material(black)

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
  eye = gr.mesh('sphere', name .. ' eye')
  eye:scale(0.1, length, 0.1)
  eye:translate(0, length, 0)
  eye:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))
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
  eyeLid = gr.mesh('sphere', name .. ' eye lid')
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
  lowerEyeLid = gr.mesh('sphere', name .. ' lower eye lid')
  lowerEyeLid:scale(0.14, 0.02, 0.14)
  -- lowerEyeLid:translate(0, -0.5, 0)
  lowerEyeLid:set_material(red)
  lowerEyeLidJoint:add_child(lowerEyeLid)

  return lowerEyeLidJoint
end

function createEyebrow(name)
  eyebrowJoint = gr.joint(name .. ' eyebrow', {-30, 0, 30}, {-30, 0, 30})
  eyeBrowM = gr.mesh('sphere', name .. ' eyebrow M')
  eyeBrowL = gr.mesh('sphere', name .. ' eyebrow L')
  eyeBrowR = gr.mesh('sphere', name .. ' eyebrow R')

  for i, eyeBrow in ipairs({eyeBrowM, eyeBrowL, eyeBrowR}) do
    eyeBrow:scale(0.02, 0.1, 0.02)
    eyeBrow:translate(0, 0.1, 0)
    eyeBrow:set_material(black)
    eyebrowJoint:add_child(eyeBrow)
  end
  eyeBrowL:rotate('z', 30)
  eyeBrowR:rotate('z', -30)
  return eyebrowJoint
end

function createEyeball(name)
  eyeballJoint = gr.joint(name .. ' eye ball', {-10, 0, 10}, {-10, 0, 10})
  eyeball = gr.mesh('sphere', name .. ' eye ball')
  eyeball:scale(0.065, 0.1, 0.065)
  eyeball:translate(0, 0, 0.1)
  eyeball:set_material(black)
  eyeballJoint:add_child(eyeball)

  return eyeballJoint
end

function createArm(name)
  length = 0.5
  upperArmJoint = gr.joint(name .. ' upper arm joint', {-30, 0, 30}, {-30, 0, 30})
  upperArm = gr.mesh('sphere', name .. ' upper arm')
  upperArm:scale(0.04, length, 0.04)
  upperArm:translate(0, length, 0)
  upperArm:set_material(red)
  upperArmJoint:add_child(upperArm)

  lowerArm = createLowerArm(name)
  if name == 'left' then
    lowerArm:rotate('z', 150)
  else
    lowerArm:rotate('z', -150)
  end
  lowerArm:translate(0, 2 * length, 0)
  upperArmJoint:add_child(lowerArm)

  -- sleeve = createSleeve(name)
  -- upperArmJoint:add_child(sleeve)

  return upperArmJoint
end

function createSleeve(name)
  sleeveJoint = gr.joint(name .. ' sleeve', {-60, 0, 60}, {-60, 0, 60})
  sleeve = gr.mesh('sphere', name .. ' sleeve')
  sleeve:scale(0.1, 0.05, 0.1)
  sleeve:translate(0, 0.05, 0)
  sleeve:set_material(lightBlue)
  sleeveJoint:add_child(sleeve)

  return sleeveJoint
end

function createLowerArm(name)
  lowerArmLength = 0.4
  lowerArmJoint = gr.joint(name .. ' lower arm joint', {-60, 0, 60}, {-60, 0, 60})
  lowerArm = gr.mesh('sphere', name .. ' lower arm')
  lowerArm:scale(0.04, lowerArmLength, 0.04)
  lowerArm:translate(0, lowerArmLength, 0)
  lowerArm:set_material(red)
  lowerArmJoint:add_child(lowerArm)

  hand = createHand(name)
  if name == 'left' then
    hand:rotate('z', -90)
  else
    hand:rotate('z', 90)
  end
  hand:translate(0, 2 * lowerArmLength, 0)
  lowerArmJoint:add_child(hand)

  return lowerArmJoint
end

function createHand(name)
  handJoint = gr.joint(name .. ' hand joint', {-60, 0, 60}, {-60, 0, 60})
  hand = gr.mesh('sphere', name .. ' hand')
  hand:scale(0.3, 0.2, 0.17)
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
  handJoint:add_child(thumb)
  handJoint:add_child(pinky)

  return handJoint
end

function createThumb(name)
  thumbJoint = gr.joint(name .. ' thumb joint', {-60, 0, 60}, {-60, 0, 60})
  thumb = gr.mesh('sphere', name .. ' thumb')
  thumb:scale(0.35, 0.6, 0.2)
  thumb:translate(0, 0.5, 0)
  thumb:set_material(red)
  thumbJoint:add_child(thumb)

  return thumbJoint
end

function createPinky(name)
  pinkyJoint = gr.joint(name .. ' pinky joint', {-60, 0, 60}, {-60, 0, 60})
  pinky = gr.mesh('sphere', name .. ' pinky')
  pinky:scale(0.15, 0.3, 0.17)
  pinky:translate(0, 0.24, 0)
  pinky:set_material(red)
  pinkyJoint:add_child(pinky)

  return pinkyJoint
end

function createLeg(name)
  upperLegJoint = gr.joint(name .. ' upper leg joint', {-30, 0, 30}, {-30, 0, 30})
  upperLeg = gr.mesh('sphere', name .. ' upper leg')
  upperLeg:scale(0.1, 0.05, 0.1)
  -- upperLeg:translate(0, -0.1, 0)
  upperLeg:set_material(darkBlue)
  upperLegJoint:add_child(upperLeg)

  lowerLeg = createLowerLeg(name)
  if name == 'left' then
    lowerLeg:rotate('z', -10)
  else
    lowerLeg:rotate('z', 10)
  end
  -- lowerLeg:translate(0, -0.2, 0)
  upperLegJoint:add_child(lowerLeg)
  return upperLegJoint
end

function createLowerLeg(name)
  lowerLegJoint = gr.joint(name .. ' lower leg joint', {-60, 0, 60}, {-60, 0, 60})
  lowerLeg = gr.mesh('sphere', name .. ' lower leg')
  lowerLeg:scale(0.05, 0.15, 0.05)
  lowerLeg:translate(0, -0.15, 0)
  lowerLeg:set_material(red)
  lowerLegJoint:add_child(lowerLeg)

  return lowerLegJoint
end


-- Add the cubeMesh GeometryNode to the child list of rootnode.
rootNode:add_child(createAll())

-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode
