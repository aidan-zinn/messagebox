// Messagebox heart — 8-bit pixel-art heart, driven by an FS90R servo.
//
// Print orientation: back face DOWN (z=0 on bed), pixels extruding UP in +Z.
// No supports needed. The horn pocket is a cavity that opens toward the bed.

// ======== Parameters ========
pixel_size            = 4;     // was 8; fits 82x50 box front face at 44x40mm
heart_depth           = 10;
horn_pocket_length    = 21;    // along X (horizontal, across heart width)
horn_pocket_width     = 5;     // along Y
horn_pocket_depth     = 2.5;
horn_clearance        = 0.2;
$fn = 64;

// ======== Pixel bitmap ========
// 1 = filled, 0 = empty. Row 0 is the TOP.
bitmap = [
    [0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0],  // row 0 — two top bumps, central notch
    [0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0],  // row 1
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],  // row 2 — widest
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],  // row 3
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],  // row 4
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0],  // row 5
    [0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0],  // row 6
    [0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0],  // row 7
    [0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0],  // row 8
    [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0]   // row 9 — single-pixel point
];

rows = len(bitmap);        // 10
cols = len(bitmap[0]);     // 11

// ======== Heart ========
module pixel_heart() {
    difference() {
        // Pixels — centered on origin, extruding in +Z from z=0.
        union() {
            for (r = [0 : rows - 1]) {
                for (c = [0 : cols - 1]) {
                    if (bitmap[r][c] == 1) {
                        x = (c - (cols - 1) / 2) * pixel_size;
                        y = ((rows - 1) / 2 - r) * pixel_size;
                        translate([x - pixel_size/2, y - pixel_size/2, 0])
                            cube([pixel_size, pixel_size, heart_depth]);
                    }
                }
            }
        }

        // Horn pocket cut into the back face, opening toward -Z.
        // Length along X so a dual-arm horn runs left/right across the heart.
        pocket_l = horn_pocket_length + 2 * horn_clearance;
        pocket_w = horn_pocket_width  + 2 * horn_clearance;
        translate([-pocket_l/2, -pocket_w/2, -0.01])
            cube([pocket_l, pocket_w, horn_pocket_depth + 0.02]);
    }
}

// ======== Render ========
pixel_heart();
