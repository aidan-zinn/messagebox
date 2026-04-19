// Messagebox enclosure — prototype
// Tight box sized around the 72 x 43 OLED with ~3 mm margin on each side.
// Electronics sit loose on the floor (no internal standoffs or screw mounts).
// Servo is fastened to the front wall (torque reaction requires it).
// Render: F5 preview, F6 full render, F7 export STL.
// Two parts: set PART = "body" or "lid" and export separately.

PART = "body"; // "body" | "lid" | "both"

// ---------- Global dimensions ----------
wall        = 2.0;
ext_w       = 82;    // external width  (X, front face) — OLED 72 + 2x3 margin + 2x2 wall
ext_d       = 53;    // external depth  (Y)             — OLED 43 + 2x3 margin + 2x2 wall
ext_h       = 50;    // external height (Z)             — servo body + clearances
corner_r    = 4;     // vertical corner radius (outside)
$fn         = 48;

// Derived interior
int_w = ext_w - 2*wall;   // 78
int_d = ext_d - 2*wall;   // 49
int_h = ext_h - wall;     // 48 (open top, lid closes it)
int_r = max(0.5, corner_r - wall);  // interior radius — keeps wall thickness uniform

// ---------- Lid ----------
lid_h       = 8;                // total lid height
lid_lip_h   = 4;                // lip that drops into body recess
lid_clear   = 0.4;              // slop between lid lip and body opening (per side)

// ---------- OLED module ----------
oled_w      = 72;
oled_d      = 43;
oled_slop   = 1.0;              // per side, so opening = oled + 2*slop

// Top lip that the OLED rests on
lip_w_in    = 2;                // how far lip projects inward from each wall
lip_thk     = 2;                // lip thickness (Z)
lip_top_z   = ext_h - 8;        // top surface of lip (8 mm below top of box)

// ---------- Servo (SG90-class) ----------
servo_screw_pitch = 28;
servo_screw_d     = 2.2;
servo_shaft_d     = 6;
servo_z           = 22;

// ---------- Back wall USB-C cutout ----------
usb_w = 10;
usb_h = 5;
usb_z = 3;

// ============================================================
// Rounded rectangular prism: hull of 4 vertical corner cylinders.
// (w, d, h, r) -> part occupies [0..w] x [0..d] x [0..h] with rounded
// vertical corners of radius r.
module rounded_box(w, d, h, r) {
    hull() {
        for (x = [r, w - r], y = [r, d - r])
            translate([x, y, 0]) cylinder(r = r, h = h);
    }
}

// ============================================================
module body() {
    difference() {
        // Outer shell (rounded)
        rounded_box(ext_w, ext_d, ext_h, corner_r);

        // Interior cavity (rounded, smaller radius to preserve wall)
        translate([wall, wall, wall])
            rounded_box(int_w, int_d, ext_h, int_r);

        // Lid recess — rim steps down by lid_lip_h
        translate([wall - lid_clear, wall - lid_clear, ext_h - lid_lip_h])
            rounded_box(int_w + 2*lid_clear, int_d + 2*lid_clear,
                        lid_lip_h + 0.01, int_r);

        // USB-C cutout in back wall
        translate([(ext_w - usb_w)/2, ext_d - wall - 0.01, wall + usb_z])
            cube([usb_w, wall + 0.02, usb_h]);

        // Servo shaft through front wall
        translate([ext_w/2, -0.01, servo_z])
            rotate([-90, 0, 0])
                cylinder(d = servo_shaft_d, h = wall + 0.02, $fn = 48);

        // Servo screw holes through front wall
        for (dx = [-servo_screw_pitch/2, servo_screw_pitch/2])
            translate([ext_w/2 + dx, -0.01, servo_z])
                rotate([-90, 0, 0])
                    cylinder(d = servo_screw_d, h = wall + 0.02, $fn = 24);
    }

    // OLED rest lip — rounded outer (matches interior), rectangular OLED opening.
    opening_w = oled_w + 2*oled_slop;   // 74
    opening_d = oled_d + 2*oled_slop;   // 45
    translate([0, 0, lip_top_z - lip_thk])
        difference() {
            translate([wall, wall, 0])
                rounded_box(int_w, int_d, lip_thk, int_r);
            translate([(ext_w - opening_w)/2, (ext_d - opening_d)/2, -0.01])
                cube([opening_w, opening_d, lip_thk + 0.02]);
        }
}

// ============================================================
module lid() {
    top_w = ext_w;
    top_d = ext_d;
    lip_w = int_w + 2*lid_clear - 0.4;   // slight slop into recess
    lip_d = int_d + 2*lid_clear - 0.4;
    lip_r = max(0.5, int_r - 0.2);

    // Top plate (matches exterior footprint & radius)
    rounded_box(top_w, top_d, lid_h - lid_lip_h, corner_r);

    // Lip hanging below plate, centered
    translate([(top_w - lip_w)/2, (top_d - lip_d)/2, -(lid_lip_h)])
        rounded_box(lip_w, lip_d, lid_lip_h, lip_r);
}

// ============================================================
if (PART == "body")      body();
else if (PART == "lid")  translate([0, 0, ext_h + 10]) lid();
else { body(); translate([0, 0, ext_h + 10]) lid(); }
