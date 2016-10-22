


// this is the stepper motor (https://github.com/jcrocholl/kossel)

// NEMA17 stepper motors.
motor_shaft_diameter = 5;
motor_shaft_radius = motor_shaft_diameter/2 + extra_radius;

// Placement for the NEMA17 stepper motors.
motor_offset = 44;
motor_length = 47;

module nema17() {
  // NEMA 17 stepper motor.
  difference() {
    union() {
      translate([0, 0, -motor_length/2]) intersection() {
        cube([42.2, 42.2, motor_length], center=true);
        cylinder(r=25.1, h=motor_length+1, center=true, $fn=60);
      }
      cylinder(r=11, h=4, center=true, $fn=32);
      cylinder(r=2.5, h=48, center=true, $fn=24);
    }
    for (a = [0:90:359]) {
      rotate([0, 0, a]) translate([15.5, 15.5, 0])
        cylinder(r=m3_radius, h=10, center=true, $fn=12);
    }
  }
}


nema17();