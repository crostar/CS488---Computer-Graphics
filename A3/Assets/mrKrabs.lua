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

  torso:add_child(lEye)
  torso:add_child(rEye)

  subroot:add_child(torso)

  torso:translate(0.0, -0.5, 0)
  rEye:translate(0.2, 0.8, 0)
  lEye:translate(-0.2, 0.8, 0)
  return subroot;
end

function createTorso()
  torso = gr.mesh('cube', 'torso')
  torso:scale(1.0, 1.0, 1.0)
  torso:rotate('y', 45.0)
  torso:set_material(red)
  return torso
end

function createEye(name)
  eyeJoint = gr.joint(name .. ' eye joint', {0, 10, 180}, {0, 90, 180})
  eye = gr.mesh('sphere', name .. ' eye')
  eye:scale(0.1, 0.5, 0.1)
  -- eye:scale(0.1, 1.0, 0.1)
  eye:rotate('x', 45.0)
  eye:set_material(gr.material({0.4, 0.8, 0.4}, {0.8, 0.8, 0.8}, 50.0))
  eyeJoint:add_child(eye)
  return eyeJoint
end

-- Add the cubeMesh GeometryNode to the child list of rootnode.
rootNode:add_child(createAll())

-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode
