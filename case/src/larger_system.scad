// On 17/11 we did a flight test in a C172 - the torque was just not enough.

include <settings.scad>
include <utils.scad>

include <nutsnbolts/cyl_head_bolt.scad>


Final=true;
ClampRailTolerance=0.1; // we want a *lot* of friction here. later we'll add safety pins
GrooveDepth=5;

// the clamps work great

module clamp() {
     // translate([0,-100, 0]) rotate([-90, 0, 0]) big_screw();

    
     translate([-ClampThickness/2, -85, 0]) {
          rotate([0, 90, 0]) {
               difference(){
                    cylinder(ClampThickness, ClampDiameter/2+ClampThickness/2, ClampDiameter/2+ClampThickness/2, $fn=fn);
                    cylinder(ClampThickness, ClampDiameter/2, ClampDiameter/2, $fn=fn);
                    translate([0, -ClampDiameter/2, 0]) {
                         cube([ClampDiameter, ClampDiameter / 1.3, ClampThickness]);
                    }
                    if (Final) {
                         translate([0, -55, ClampThickness/2]) {
                              rotate([-90, 0, 0]) big_screw();
                         }
                    }
               } ;
          }   
     };

    
     // this is for the rail
     translate([0, -35, 0]) {
          difference() {
               cube([ClampThickness, ClampThickness * 3, ClampThickness * 2], center=true);
  
          }
     }
   
}

module clamp_left() {
     translate([100, 0, 0]) {
          difference() {
               clamp();
               common_rail(ClampRailTolerance);
          }
     }
}

module clamp_right() {
     translate([-100, 0, 0]) {
          difference() {
               clamp();
               common_rail(ClampRailTolerance);
          }
     }
}


//clamp_left();

// we need at least 20 between the clamps
// the max we can reasonably do is something like 40cm.

RailWidth=400;
RailHeight=40;
RailDepth=20;


module clamp_groove(tolerance) {
     translate([0, -RailHeight/4 , RailDepth/2 - (GrooveDepth/2) + tolerance]) {
          rail_groove(tolerance);
     }
     translate([0, -RailHeight/4, -RailDepth/2 + (GrooveDepth)/2 - tolerance]) {
          rail_groove(tolerance);
     } 
}



module weight_groove(tolerance) {
     translate([0, -RailHeight/4 + RailHeight/2 , RailDepth/2 - (GrooveDepth/2) + tolerance]) {
          rail_groove(tolerance);
     }
     translate([0, -RailHeight/4 + RailHeight/2, -RailDepth/2 + (GrooveDepth)/2 - tolerance]) {
          rail_groove(tolerance);
     } 
}

module common_rail(tolerance) {
     difference() {
          cube([RailWidth, RailHeight+tolerance, RailDepth+tolerance], center=true);
          clamp_groove(tolerance);
          weight_groove(tolerance);
     }
}



// we need to split the common rail for printing, and put bolts to secure it.
// the play is exclusively going to be vertical.
// and the print is going to be horizontal, with the grooves facing up or down


RailSplitLip=40; // that'll make it 24cm, pretty close to the limit. 


module right_lip() {
     translate([RailSplitLip/2, 0, RailDepth/4]) {
          cube([RailSplitLip, RailHeight/4, RailDepth/2], center=true);
     };
     difference() {
          translate([-RailWidth/2, 0, 0]) {
               cube([RailWidth, RailHeight, RailDepth], center=true);
          };
          
          translate([-RailSplitLip/2, 0, - RailDepth /4 ]) {
               cube([RailSplitLip, RailHeight/4, RailDepth/2], center=true);
          };
     }
}

module left_lip() {
     translate([-RailSplitLip/2, 0,- RailDepth/4]) {
          cube([RailSplitLip, RailHeight/4, RailDepth/2], center=true);
     };
     difference() {
          translate([RailWidth/2, 0, 0]) {
               cube([RailWidth, RailHeight, RailDepth], center=true);
          };
          
          translate([RailSplitLip/2, 0, RailDepth /4 ]) {
               cube([RailSplitLip, RailHeight/4, RailDepth/2], center=true);
          };
     }
}

module left_common_rail() {
     difference() {
          common_rail(0);
          right_lip();
          rail_screws();
     }
     difference() {
          rod();
          translate([-RailWidth/2, 0, 0]) {
               cube([RailWidth, RingDiameter, RingDiameter], center=true);
          }
     }
}

module right_common_rail() {
     difference() {
          common_rail(0);
          left_lip();
          rail_screws();
     }
     difference() {
          rod();
          translate([RailWidth/2, 0, 0]) {
               cube([RailWidth, RingDiameter, RingDiameter], center=true);
          }
     } 
}


PI=3.141592; 

RailDiameter=60; // don't change that if you don't want to have to reprint the weight .. 
RodTol=0.2;

module rod() {
     translate([0, 20, 0]) {
          rotate([0, 90, 0]) {
               rotate([0, 0, -90]) {
                    difference() {
                         translate([-RailDepth, -RailDepth/2, -RailWidth/2]) {
                              cube([RailDepth, RailDepth, RailWidth ]);   
                         }
                         translate([-RailDiameter/2 - RodTol/2, 0, -RailWidth/2]) {
                              screw_thread(RailDiameter+RodTol,10,55,RailWidth,PI/4, 0);
                         }
                    }
               }
          }
     }
}

left_common_rail();

     
module rail_screws() {
     
     module m3_screw_assembly() {
          translate([0, 0, 5]) {
              // screw("M3x16", thread="modeled");
               translate([0, 0, 10]) {
                    hole_through(name="M3", l=16, cl=0.1, h=10, hcl=0.4);
               }
               translate([0, 0, -16]) {
                    rotate([180,0,0]) nutcatch_parallel("M3", l=5, clk= 0.1);
               }
               
          }
     }
     translate([RailSplitLip/2, 0, 0]) {
          m3_screw_assembly();
     }
     translate([-RailSplitLip/2, 0, 0]) {
          m3_screw_assembly();
     }
     
}
/*
translate([50, 0, 0]) {
     left_common_rail();
}

right_common_rail();
*/


module rail_groove(tolerance) {
     cube([RailWidth, GrooveDepth-tolerance, GrooveDepth-tolerance], center=true);
}


use <flat_rail.scad>

/*
translate([0, 20, 0]) {
     rotate([0, 90, 0])
          rotate([0, 0, -90])
     {
          left_part();
      //    driver_gear();
     }
}
*/
