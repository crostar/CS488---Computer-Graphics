-- Mr. Krabs :
-- https://en.wikipedia.org/wiki/Mr._Krabs
-- red (1.0, 0.63, 0.4)
red = gr.material({1.0, 0.2, 0.1}, {0.7, 0.14, 0.07}, 50.0)
-- red = gr.material({1.0, 0.63, 0.4}, {0.07, 0.042, 0.028}, 500.0)
green = gr.material({0.4, 0.8, 0.4}, {0, 0, 0}, 50.0)
mouthBlack = gr.material({0.1, 0.1, 0.1}, {0, 0, 0}, 50.0)
beltBlack = gr.material({0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50.0)
lightBlue = gr.material({0.45, 0.65, 0.8}, {0.045, 0.065, 0.08}, 1.0)
darkBlue = gr.material({0.6, 0.65, 0.85}, {0.06, 0.065, 0.085}, 100.0)
gold = gr.material({1.0, 0.8, 0.0}, {0, 0, 0}, 2.0)
-- white = gr.material({0.8, 0.85, 0.6}, {0, 0, 0}, 1.0)
white = gr.material({0.9, 0.95, 0.9}, {0, 0, 0}, 1.0)

function Sun()
  sun = gr.mesh('sphere', 'sun')
  sun:set_material(gold)
  return sun
end

function Earth()
  earth = gr.mesh('sphere', 'earth')
  earth:set_material(white)
  return earth
end

function Moon()
  moon = gr.mesh('sphere', 'moon')
  moon:set_material(white)
  return moon
end

function Star()
  star = gr.mesh('sphere', 'star')
  star:set_material(white)
  return star
end

function Mirror()
  mirror = gr.node('mirror')
  mirror_instance = gr.mesh('cube', 'mirror_instance')
  mirror_instance:set_material(gold)

  mirror:add_child(mirror_instance)
  mirror:translate(0.0, -0.5, -1.0)
  return mirror
end


function buildScene()
  -- Create the top level root node named 'root'.
  rootNode = gr.node('root')

  -- rootNode:add_child(Crab())
  rootNode:add_child(Sun())
  rootNode:add_child(Earth())
  rootNode:add_child(Moon())
  rootNode:add_child(Star())
  -- rootNode:add_child(Box())

  -- Add the cubeMesh GeometryNode to the child list of rootnode.
  return rootNode
end

SceneNode = buildScene()

-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode
