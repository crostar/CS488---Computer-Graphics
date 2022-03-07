-- Mr. Krabs :
-- https://en.wikipedia.org/wiki/Mr._Krabs

red = gr.material({0.9, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50.0)

-- Create the top level root node named 'root'.
rootNode = gr.node('root')

function createAll()
  subroot = gr.node('subroot')

  torso = createTorso();
  rEye = createEye('right');
  lEye = createEye('left');
  rArm = createArm('right')
  lArm = createArm('left')
  rLeg = createLeg('right')
  lLeg = createLeg('left')

  subroot:add_child(torso)
  subroot:add_child(lEye)
  subroot:add_child(rEye)
  subroot:add_child(lArm)
  subroot:add_child(rArm)
  subroot:add_child(lLeg)
  subroot:add_child(rLeg)

  rEye:translate(0.3, 1.65, 0)
  lEye:translate(-0.3, 1.65, 0)

  rArm:rotate('z', -75)
  rArm:translate(0.7, 1.2, 0)
  lArm:rotate('z', 75)
  lArm:translate(-0.7, 1.2, 0)

  lLeg:translate(-0.4, -0.9, 0)
  rLeg:translate(0.4, -0.9, 0)
  return subroot;
end

function createTorso()
  t1 = gr.mesh('sphere', 'torso 1')
  t2 = gr.mesh('sphere', 'torso 2')
  t3 = gr.mesh('sphere', 'torso 3')
  t1:scale(1.2, 1.0, 1.2)
  t2:scale(0.8, 0.9, 0.8)
  t3:scale(0.7, 1.0, 0.7)
  t1:add_child(t2)
  t2:add_child(t3)
  t2:translate(0, 0.6, 0)
  t3:translate(0, 0.3, 0)
  t1:set_material(red)
  t2:set_material(red)
  t3:set_material(red)

  return t1
end

function createEye(name)
  eyeJoint = gr.joint(name .. ' eye joint', {-30, 0, 30}, {-30, 0, 30})
  eye = gr.mesh('sphere', name .. ' eye')
  eye:scale(0.1, 0.9, 0.1)
  eye:translate(0, 0.9, 0)
  eye:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))
  eyeJoint:add_child(eye)
  return eyeJoint
end

function createArm(name)
  upperArmJoint = gr.joint(name .. ' upper arm joint', {-30, 0, 30}, {-30, 0, 30})
  upperArm = gr.mesh('sphere', name .. ' upper arm')
  upperArm:scale(0.06, 0.8, 0.06)
  upperArm:translate(0, 0.8, 0)
  upperArm:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))
  upperArmJoint:add_child(upperArm)

  lowerArm = createLowerArm(name)
  if name == 'left' then
    lowerArm:rotate('z', 150)
  else
    lowerArm:rotate('z', -150)
  end
  lowerArm:translate(0, 1.6, 0)
  upperArmJoint:add_child(lowerArm)
  return upperArmJoint
end

function createLowerArm(name)
  lowerArmJoint = gr.joint(name .. ' lower arm joint', {-60, 0, 60}, {-60, 0, 60})
  lowerArm = gr.mesh('sphere', name .. ' lower arm')
  lowerArm:scale(0.04, 0.4, 0.04)
  lowerArm:translate(0, 0.4, 0)
  lowerArm:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))
  lowerArmJoint:add_child(lowerArm)

  return lowerArmJoint
end

function createLeg(name)
  upperLegJoint = gr.joint(name .. ' upper leg joint', {-30, 0, 30}, {-30, 0, 30})
  upperLeg = gr.mesh('sphere', name .. ' upper leg')
  upperLeg:scale(0.2, 0.15, 0.2)
  upperLeg:translate(0, -0.1, 0)
  upperLeg:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))
  upperLegJoint:add_child(upperLeg)

  lowerLeg = createLowerLeg(name)
  lowerLeg:translate(0, -0.2, 0)
  upperLegJoint:add_child(lowerLeg)
  return upperLegJoint
end

function createLowerLeg(name)
  lowerLegJoint = gr.joint(name .. ' lower leg joint', {-60, 0, 60}, {-60, 0, 60})
  lowerLeg = gr.mesh('sphere', name .. ' lower leg')
  lowerLeg:scale(0.1, 0.25, 0.1)
  lowerLeg:translate(0, -0.25, 0)
  lowerLeg:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))
  lowerLegJoint:add_child(lowerLeg)

  return lowerLegJoint
end


-- Add the cubeMesh GeometryNode to the child list of rootnode.
rootNode:add_child(createAll())

-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode
