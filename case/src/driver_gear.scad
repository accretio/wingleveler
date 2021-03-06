use <controller.scad>

include <rail.scad>

tol = 0.2;

module shaft() {
    translate([3 - tol, 0, RingThickness/2]) {
        cube([2, 5, RingThickness], center=true);
    };
difference() {gear (circular_pitch=p,
              gear_thickness = RingThickness,
		      rim_thickness = RingThickness,
		      hub_thickness = RingThickness,
		      circles=0,
		      number_of_teeth = DriverNumberOfTeeth,
		      rim_width = 0);  
    cylinder(RingThickness, 5/2 + tol , 5/2 + tol, $fn=fn);
}
}

shaft();