include <settings.scad>
use <rail.scad>
use <clamp.scad>

include <polyScrewThread_r1.scad>


RailLength=25;

/* Example 01.
 * Just a 100mm long threaded rod.
 *
 * screw_thread(15,   // Outer diameter of the thread
 *               4,   // Step, traveling length per turn, also, tooth height, whatever...
 *              55,   // Degrees for the shape of the tooth 
 *                       (XY plane = 0, Z = 90, btw, 0 and 90 will/should not work...)
 *             100,   // Length (Z) of the tread
 *            PI/2,   // Resolution, one face each "PI/2" mm of the perimeter, 
 *               0);  // Countersink style:
 *                         -2 - Not even flat ends
 *                         -1 - Bottom (countersink'd and top flat)
 *                          0 - None (top and bottom flat)
 *                          1 - Top (bottom flat)
 *                          2 - Both (countersink'd)
 */

PI=3.141592; 
RailLength=25;
RailDiameter=30; 

module mask() {
    translate([-ClampThickness/2, 0, RailLength/2]) {
        rotate([0, 90, 0]) {
            cube([RailLength, ClampThickness, RingThickness], center= true);
        }
    }
}
intersection(){
    screw_thread(RailDiameter,10,55,RailLength,PI/4, 0);
    mask();
}
translate([RingThickness/2, 0, RailLength/2]) {
    rotate([0, 90, 0]) {
        cube([RailLength, ClampThickness, RingThickness], center= true);
    }
}

RailClampLength=50;

difference() {
    translate([0, -ClampThickness/2, RailLength /2 - ClampThickness / 2]) {
        cube([RailClampLength, ClampThickness, ClampThickness]);
    }
    connector();
}

module connector() {
    translate([RailClampLength-ClampZone, -ClampThickness/2, RailLength /2 - ClampThickness / 2 + ClampThickness/3]) {
        cube([ClampZone, ClampThickness, ClampThickness/3]);  
        translate([0, -ClampThickness/2, 4]){
            rotate([0, 90, 0]) {
                clamp_screw(); 
            }
        }   
    }
}    
//mask();