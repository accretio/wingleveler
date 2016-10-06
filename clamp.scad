
rotate(a=[0,0,0]) {
////BODY////
    
difference() {
    cube([40,60,60]);
    translate([0,0,15])
    cube([40,30,30]);

////Top HOLE 12.5 MM////

    translate([20,45,23])
    cylinder(r=6.25,h=37,$fn=200);

////NUT & BOLT HOLE////

    translate([20,15,0])
    cylinder(r=4,h=15,$fn=200);
  
    translate([20,15,9])
    cylinder(r=7.4,h=6.1,$fn=6);
        
        
}
}