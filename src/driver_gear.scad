use <controller.scad>

include <rail.scad>
gear (circular_pitch=p,
              gear_thickness = RingThickness,
		      rim_thickness = RingThickness,
		      hub_thickness = RingThickness,
		      circles=0,
		      number_of_teeth = DriverNumberOfTeeth,
		      rim_width = 0);  