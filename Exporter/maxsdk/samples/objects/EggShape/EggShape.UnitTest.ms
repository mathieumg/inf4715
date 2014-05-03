fileIn "MxsUnit/MxsUnit.ms"

struct UnitTest_EggShape
(
public
	function setup =
	(
	),

	function teardown =
	(
	),
	
	function test_egg_param =
	(
		local myEgg = Egg egg_length:100 egg_thickness:5 pos:[ 0, 0, 0] egg_outline:false
		myEgg.egg_angle = 90	
		myEgg.egg_length = 300
		assert_equal 200 myEgg.egg_width
		myEgg.egg_width = 30
		assert_equal 45 myEgg.egg_length
		myEgg.egg_outline = true
		myEgg.egg_thickness = 10
	),
	
	function test_egg_copy = 
	(
		local myEgg = Egg egg_length:100 egg_thickness:5 egg_width:300 pos:[ 0, 0, 0] egg_outline:true
		local myEggCopy = copy myEgg
		
		assert_equal myEgg.egg_angle myEggCopy.egg_angle
		assert_equal myEgg.egg_length myEggCopy.egg_length
		assert_equal myEgg.egg_thickness myEggCopy.egg_thickness
		assert_equal myEgg.egg_width myEggCopy.egg_width
		assert_equal myEgg.egg_outline myEggCopy.egg_outline
	),
	
	function test_common_param =
	(
		local myEgg = Egg pos:[ 0, 0, 0]
		myEgg.render_renderable = false
		myEgg.render_thickness = 2.0
		myEgg.render_mapcoords = false		
	
		myEgg.egg_thickness = 0
		local inner = curveLength myEgg 1
		local outer = curveLength myEgg 2
		
		assert_equal inner outer
	),
	
	function test_edit_spline =
	(
		local myEgg2 = Egg egg_length:100 legg_thickness:5 pos:[-20,-20, 0]
		max modify mode
		convertToSplineShape myEgg2
		open myEgg2 1
		deleteSpline myEgg2 1
		
		local newspline1 = addNewSpline myEgg2
		local newspline2 = addNewSpline myEgg2
		local newspline3 = addNewSpline myEgg2
		addKnot  myEgg2 newspline1 #smooth #curve [60,60,0]
		addKnot  myEgg2 newspline1 #smooth #curve [40,40,0]
		addKnot  myEgg2 newspline2 #smooth #curve [60,60,0]
		addKnot  myEgg2 newspline2 #smooth #curve [40,40,0]
		addKnot  myEgg2 newspline3 #smooth #curve [60,60,0]
		addKnot  myEgg2 newspline3 #smooth #curve [40,40,0]
		bindKnot myEgg2 false newspline1 1 1
		bindKnot myEgg2 true newspline1 3 1
		bindKnot myEgg2 false newspline2 1 1
		bindKnot myEgg2 true newspline2 2 1
		bindKnot myEgg2 false newspline3 3 1
		bindKnot myEgg2 true newspline3 2 1
		updateshape myEgg2	
	),

	Tests = 
	#(
		test_egg_param,
		test_egg_copy,
		test_common_param,
		test_edit_spline
	)
)

run_test_fixture UnitTest_EggShape script:(getThisScriptFilename())
