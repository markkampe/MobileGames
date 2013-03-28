import processing.core.PVector;

/**
 * Points in a camera relative Cartesian space:
 * 	  x - mm left/right away from the center-line
 * 	  y - mm up/down away from the center line
 * 	  z - mm away from the lens
 * 
 *	We can't compute real-world distances from Kinect x/y 
 *	coordinates, because they more represent angles than
 *	distances.  To enable better proximity determination we
 *	try to correct for this and compute real-world x/y/z
 *	coordinates.
 *
 * 	The fact that the coordinate space is camera-relative is
 * 	unimportant because the only things we use them for
 *	is proximity determination.  Turning these into room
 *	coordinates would be much more work (and IMHO not worth it)
 *
 * CAVEAT EMPTOR:
 * 	The only difference between a PVector and a KinectCoord is
 * 	the translation that happens upon construction.  They are
 * 	otherwise indistinguishable.  Make sure you know which type
 * 	of value is in which variables.
 */
public class KinectCoords extends PVector {
	
	// Kinect Calibration Constants (based on specifications)
	private static final int H_RES = 640;		// X resolution in pixels
	private static final int V_RES = 480;		// Y resolution in pixels
	private static final float H_FIELD = 838;	// mm at one meter (1000 sin 57deg)
	private static final float V_FIELD = 681;	// mm at one meter (1000 sin 43deg)
	private static final float Z_UNIT = 1.3F;	// mm away from the detector
	
	/*
	 * The true dimensions of a pixel are proportional to its distance
	 * from the camera.  The proportionality constants can be inferred
	 * from the resolution and viewing angles.  This enables a simple
	 * linear transformation from Kinect to real-world coordinates.
	 */
	private static final int xCal = H_RES/2;	// center of screen
	private static final int yCal = V_RES/2;	// center of screen
	private static final float xScale = H_FIELD/H_RES;
	private static final float yScale = V_FIELD/V_RES;
	private static final float zScale = Z_UNIT;
	
	/**
	 * convert a set of camera reported coordinates into real-world 
	 * coordinates (Cartesian, millimeters, relative to the sensor).
	 *
	 * @param cx	camera-reported X coordinate
	 * @param cy	camera-reported Y coordinate
	 * @param cz	camera-reported Z coordinate
	 * 
	 * @return		PVector for inferred real world coordinates
	 */
	public KinectCoords(int cx, int cy, int cz) {
		
		/*
		 * convert the reported distance to millimeters
		 * use that distance to scale x/y distance from center
		 */
		super(	(cx - xCal) * xScale * cz * zScale / 1000,
				(cy - yCal) * yScale * cz * zScale / 1000,
				cz * zScale
				);
	}
}